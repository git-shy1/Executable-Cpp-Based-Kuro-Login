#include <unistd.h>
#include <fstream>

#include <jni.h>
#include "include/oxorany.h"
#include "include/obfuscate.h"
#include "zMemory/ZMemory.h"
#include "main.h"
#include "misc.h"

static bool isLogin = false, logginIn = false;

static std::string EXP = " ";
static std::string errMsg, key;

using json = nlohmann::json;
std::string g_Token, g_Auth;
bool bValid = false;
bool gameFound = false;
static char s[150];
using namespace ZMemory;

std::string Login(const char *user_key) {
	std::string hwid = user_key;
	hwid += GetAndroidID();
	hwid += GetDeviceModel();
	hwid += GetDeviceBrand();
	std::string UUID = GetDeviceUniqueID(hwid.c_str());
	std::string errMsg;
	
	struct MemoryStruct chunk{};
	chunk.memory = (char *) malloc(1);
	chunk.size = 0;
	
	CURL *curl;
	CURLcode res;
	curl = curl_easy_init();
	
	if (curl) {
	    std::string api_key = oxorany("https://lqvip.xyz/public/connect");
		curl_easy_setopt(curl, CURLOPT_URL, (api_key.c_str()));
		curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
		curl_easy_setopt(curl, CURLOPT_DEFAULT_PROTOCOL, "https");
		
		struct curl_slist *headers = NULL;
		headers = curl_slist_append(headers, AY_OBFUSCATE("Content-Type: application/x-www-form-urlencoded"));
		
		curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
		char data[4096];
		sprintf(data, AY_OBFUSCATE("game=PUBG&user_key=%s&serial=%s"), user_key, UUID.c_str());
		curl_easy_setopt(curl, CURLOPT_POSTFIELDS, data);
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writeCallback);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *) &chunk);
		curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
		curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);
		
		res = curl_easy_perform(curl);
		if (res == CURLE_OK) {
			try {
				json result = json::parse(chunk.memory);
				if (result[std::string(AY_OBFUSCATE("status"))] == true) {
					std::string token = result[std::string(AY_OBFUSCATE("data"))][std::string(AY_OBFUSCATE("token"))].get<std::string>();
					time_t rng = result[std::string(AY_OBFUSCATE("data"))][std::string(AY_OBFUSCATE("rng"))].get<time_t>();
					EXP = result[std::string(AY_OBFUSCATE("data"))][std::string(AY_OBFUSCATE("EXP"))].get<std::string>();
					
					
					if (rng + 30 > time(0)) {
						std::string auth = oxorany("PUBG");
						auth += std::string(AY_OBFUSCATE("-"));
						auth += user_key;
						auth += std::string(AY_OBFUSCATE("-"));
						auth += UUID;
						auth += std::string(AY_OBFUSCATE("-"));
						auth += std::string(AY_OBFUSCATE("Vm8Lk7Uj2JmsjCPVPVjrLa7zgfx3uz9E"));
						
						std::string outputAuth = CalcMD5(auth);
						g_Token = token;
						g_Auth = outputAuth;
						bValid = g_Token == g_Auth;
					}
				} else {
					errMsg = result[std::string(AY_OBFUSCATE("reason"))].get<std::string>();
				}
			} catch (json::exception &e) {
				errMsg = "{";
				errMsg += e.what();
				errMsg += "}\n{";
				errMsg += chunk.memory;
				errMsg += "}";
			}
		} else {
			errMsg = curl_easy_strerror(res);
		}
	}
	curl_easy_cleanup(curl);
	return bValid ? "OK" : errMsg;
}

int main() {
    system("clear");
    killGG();
    LogV(ERROR, STD_OBFUSCATE("[ + ] ⤷ *_*_* Wild Rift - LOADER *_*_*"));
    LogV(ERROR, STD_OBFUSCATE("[ + ] ⤷ *-*-* VN - 64Bit *-*-*\n"));
    
    std::string input;
    if (!isLogin) {
        InputKey(STD_OBFUSCATE("[ + ] ⤷ Login Key : "));
        std::cin >> input;
        std::strncpy(s, input.c_str(), sizeof(s));
        std::string errMsg = Login(s);
        //checkVPN();
        if (errMsg == "OK") {
            isLogin = bValid && g_Auth == g_Token;
        } else {
            LogV(FAILED, STD_OBFUSCATE("[ - ] ⤷ Failed: ") + errMsg);
            return 1;
        }
    }
    system("clear");
    LogV(SUCCESS, STD_OBFUSCATE("[ - ] ⤷ Login Successfully, Username: ") + input);
    while (!gameFound) {
         char packageName[] = "com.riotgames.league.wildriftvn";
         pid_t pid;
         long long base, unity;
         pid = find_pid(packageName);
         if (pid) {
             gameFound = true;
             base = find_library_base(pid, "libFEProj.so");
             unity = find_library_base(pid, "libunity.so");
             printf(("FEP found! Base: %llu\n"), base);
             patch_offset(pid, base + oxorany(0x0), "20 00 80 52 C0 03 5F D6");
             printf("done\n");
             } else {
                LogV(ERROR, oxorany("Game Not Found! Please Turn On The Game Before Run The Hack"));
                std::cout << "Press Enter to retry...";
                std::cin.ignore();
             }
        }
    return 0;
}
