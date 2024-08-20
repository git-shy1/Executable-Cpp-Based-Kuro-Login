#include <Utils.hpp>
#include <json.h>
#include <jni.h>
#include <curl/curl.h>
#include <frnetlib/Packet.h>
#include <frnetlib/TcpListener.h>
#include <frnetlib/TcpSocket.h>
#include <frnetlib/WebSocket.h>

#include <sys/mman.h>
#include <openssl/evp.h>
#include <openssl/pem.h>
#include <openssl/rsa.h>
#include <openssl/err.h>
#include <openssl/md5.h>
#include <sys/stat.h>
#include "VictusLogger.h"

using namespace std;
using json = nlohmann::json;

fr::TcpListener listener;
fr::WebSocket<fr::TcpSocket> server;

struct Config {
    string key;
    string token;
    bool loginInit = false;
    pthread_t pt;
} config;

struct MemoryStruct {
    char *memory;
    size_t size;
} chunk;

static size_t writeCallback(void *contents, size_t size, size_t nmemb, void *userp) {
    size_t realsize = size * nmemb;
    struct MemoryStruct *mem = (struct MemoryStruct *)userp;
    mem->memory = (char *) realloc(mem->memory, mem->size + realsize + 1);
    if (mem->memory == NULL) {
        return 0;
    }
    memcpy(&(mem->memory[mem->size]), contents, realsize);
    mem->size += realsize;
    mem->memory[mem->size] = 0;
    return realsize;
}

class Request : public fr::Packetable {
public:
	enum class Opcode : uint8_t
	{
		Login = 1,
	};
	inline Opcode get_opcode() {
		return opcode;
	}
	virtual void pack(fr::Packet &o) const override {
		o << opcode;
	}
	virtual void unpack(fr::Packet &o) override {
		o >> opcode;
	}

private:
	Opcode opcode;
};

class Login : public fr::Packetable {
public:
	enum class Opcode : uint8_t {
		Login = 1,
	};
	inline void set_opcode(Opcode opcode_) {
		opcode = opcode_;
	}
	inline void set_token(string value) {
		token = value;
	}
	virtual void pack(fr::Packet &o) const override {
		o << opcode << token;
	}
	virtual void unpack(fr::Packet &o) override {
		o >> opcode >> token;
	}
	Opcode opcode;
	string token;
};

std::string CalcMD5(std::string s) {
    std::string result;

    unsigned char hash[MD5_DIGEST_LENGTH];
    char tmp[4];

    MD5_CTX md5;
    MD5_Init(&md5);
    MD5_Update(&md5, s.c_str(), s.length());
    MD5_Final(hash, &md5);
    for (unsigned char i : hash) {
        sprintf(tmp, "%02x", i);
        result += tmp;
    }
    return result;
}

void floatToHex(float number, char* hex) {
    // Assuming float is 4 bytes
    unsigned char* bytes = reinterpret_cast<unsigned char*>(&number);

    // Convert each byte to hexadecimal representation
    for (int i = 0; i < sizeof(float); ++i) {
        sprintf(&hex[i * 2], "%02X", bytes[i]);
    }
}

int chmod(const char *path, mode_t mode);

JavaVM* gJavaVM = nullptr; // Declare gJavaVM as a global variable

JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM* vm, void* reserved) {
    JNIEnv* env;
    gJavaVM = vm; // Initialize gJavaVM
    // Additional initialization code if needed
    return JNI_VERSION_1_6;
}
jobject getGlobalContext(JNIEnv* env) {
    // Find the Context class
    jclass contextClass = env->FindClass("android/content/Context");
    // Get the static method for getting the global context
    jmethodID method = env->GetStaticMethodID(contextClass, "getApplicationContext", "()Landroid/content/Context;");
    // Call the method to get the global context
    jobject context = env->CallStaticObjectMethod(contextClass, method);
    // Release local reference to the class
    env->DeleteLocalRef(contextClass);
    return context;
}

bool IsVPNEnabled() {
    JNIEnv *env;
    gJavaVM->AttachCurrentThread(&env, 0);
    jclass ctx = env->FindClass(AY_OBFUSCATE("android/content/Context"));
    jobject context = getGlobalContext(env);
    jmethodID service = env->GetMethodID(ctx, AY_OBFUSCATE("getSystemService"), AY_OBFUSCATE("(Ljava/lang/String;)Ljava/lang/Object;"));
    jstring str = env->NewStringUTF(AY_OBFUSCATE("connectivity"));
    jobject conn_service = env->CallObjectMethod(context, service, str);
    env->DeleteLocalRef(str);
    jclass connectivity = env->FindClass(AY_OBFUSCATE("android/net/ConnectivityManager"));
    jclass capabils = env->FindClass(AY_OBFUSCATE("android/net/NetworkCapabilities"));
    jmethodID has1 = env->GetMethodID(capabils, AY_OBFUSCATE("hasCapability"), AY_OBFUSCATE("(I)Z"));
    jmethodID has = env->GetMethodID(capabils, AY_OBFUSCATE("hasTransport"), AY_OBFUSCATE("(I)Z"));
    jmethodID getCapabil = env->GetMethodID(connectivity, AY_OBFUSCATE("getNetworkCapabilities"), AY_OBFUSCATE("(Landroid/net/Network;)Landroid/net/NetworkCapabilities;"));
    jmethodID getActive = env->GetMethodID(connectivity, AY_OBFUSCATE("getActiveNetwork"), AY_OBFUSCATE("()Landroid/net/Network;"));
    jobject activenetwork = env->CallObjectMethod(conn_service, getActive);
    jobject activecapabilities = env->CallObjectMethod(conn_service, getCapabil, activenetwork);
    jboolean hasvpn1 = env->CallBooleanMethod(activecapabilities, has, 4);
    jboolean hasvpn2 = env->CallBooleanMethod(activecapabilities, has1, 4);
    if (hasvpn1 || hasvpn2) {
        env->DeleteLocalRef(activenetwork);
        env->DeleteLocalRef(activecapabilities);
        env->DeleteLocalRef(conn_service);
        env->DeleteLocalRef(ctx);
        env->DeleteLocalRef(context);
        env->DeleteLocalRef(capabils);
        env->DeleteLocalRef(connectivity);
        return true;
    } else {
        env->DeleteLocalRef(activenetwork);
        env->DeleteLocalRef(activecapabilities);
        env->DeleteLocalRef(conn_service);
        env->DeleteLocalRef(ctx);
        env->DeleteLocalRef(context);
        env->DeleteLocalRef(capabils);
        env->DeleteLocalRef(connectivity);
        return false;
    }
}

int checkVPN() {
    if(!IsVPNEnabled)
        LogV(ERROR, STD_OBFUSCATE("[ ! ] ⤷ Aborted: Do not use VPN."));
        exit(0);
}
