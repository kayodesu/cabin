/*
 * Author: Jia Yang
 */

#ifndef JVM_ENCODING_H
#define JVM_ENCODING_H

#include <cstring>
#include <string>
#include <codecvt>
#include <locale>

#include "../jvmtype.h"

static inline std::string decodeMUTF8(uint8_t *bytes, size_t len) {
    // todo 此函数的实现是错的，先这么做
    char *p = new char[len + 1];
    memcpy(p, bytes, len);
    p[len] = 0;
    return std::string(p);
}

// todo 实现对不对
static inline jstring strToJstr(const std::string &src) {
    std::wstring_convert<std::codecvt_utf8_utf16<char16_t>, char16_t> utf16conv;
    std::u16string utf16 = utf16conv.from_bytes(src);
    return utf16;
}

// todo 实现对不对
static inline std::string jstrToStr(const jstring &src) {
    std::wstring_convert<std::codecvt_utf8_utf16<char16_t>, char16_t> utf16conv;
    std::string str = utf16conv.to_bytes(src);
    return str;
}

#endif //JVM_ENCODING_H
