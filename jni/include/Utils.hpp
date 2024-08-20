//
// Created by bapan on 20/3/2024.
//
#ifndef _BAPAN_UTILS_HPP_
#define _BAPAN_UTILS_HPP_

#include <sstream>
#include <iomanip>
#include <string>
#include <sys/system_properties.h>


inline std::string GetAndroidID() {
    char prop_value[PROP_VALUE_MAX];
    __system_property_get("ro.build.id", prop_value);
    return std::string(prop_value);
}

inline std::string GetDeviceModel() {
    char prop_value[PROP_VALUE_MAX];
    __system_property_get("ro.product.model", prop_value);
    return std::string(prop_value);
}

inline std::string GetDeviceBrand() {
    char prop_value[PROP_VALUE_MAX];
    __system_property_get("ro.product.model", prop_value);
    return std::string(prop_value);
}

inline std::string GetDeviceUniqueID(const std::string& uuid) {
    std::stringstream ss;
    for (char c : uuid) {
        ss << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(c);
    }
    return ss.str();
}

inline char *random_str() {
    char *validchars = AY_OBFUSCATE("0123456789");
    char *validcharsAlp = AY_OBFUSCATE("0123456789abcdefgh{ijkl[mnop%$#!+&krstuvxywz");
    char *novastr = "";
    int str_len, i;
    srand(time(NULL));
    str_len = (rand() % 100);
    if (str_len < 100) {
        str_len = str_len + (100 - str_len);
    }
    novastr = (char *) malloc((str_len + 1) * sizeof(char));
    for (i = 0; i < str_len; i++) {
        if (i < 9) {
            novastr[i] = validcharsAlp[rand() % strlen(validcharsAlp)];
        } else if (i>=14 && i <18) {
            novastr[i] = validcharsAlp[rand() % strlen(validcharsAlp)];
        } else if (i>=21 && i<30) {
            novastr[i] = validcharsAlp[rand() % strlen(validcharsAlp)];
        } else if (i >=64 && i<85) {
            novastr[i] = validcharsAlp[rand() % strlen(validcharsAlp)];
        } else {
            novastr[i] = validchars[rand() % strlen(validchars)];
        }
        novastr[i + 1] = 0x0;
    }
    return novastr;
}

inline std::string XOR_decryption(std::string value,std::string key) {
    std::string text = "";
    std::string::size_type k = 0;
    std::string::size_type v = 0;
    std::string::size_type klen = key.length();
    std::string::size_type vlen = value.length();
    for ( std::string::size_type v = 0; v < vlen; v ++ ) {
        text +=  value[v] ^ key[k];
        k=(++k < klen ? k : 0);
    }
    return text;
}

inline std::string XOR_encryption(const std::string& value, const std::string& key) {
    std::string encrypted = "";
    std::string::size_type klen = key.length();
    std::string::size_type vlen = value.length();
    for (std::string::size_type v = 0, k = 0; v < vlen; ++v, ++k) {
        if (k >= klen)
            k = 0;
        encrypted += value[v] ^ key[k];
    }
    return encrypted;
}

inline std::string base64_decode(const std::string &input) {
    static const std::string base64_chars =
        "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
        "abcdefghijklmnopqrstuvwxyz"
        "0123456789+/";
        
    std::string decoded;
    int val = 0, valb = -8;
    for (unsigned char c : input) {
        if (c == '=') break;
        if ((c = base64_chars.find(c)) == std::string::npos) return "";
        val = (val << 6) + c;
        valb += 6;
        if (valb >= 0) {
            decoded.push_back(char((val >> valb) & 0xFF));
            valb -= 8;
        }
    }
    return decoded;
}

#endif // _BAPAN_UTILS_HPP_