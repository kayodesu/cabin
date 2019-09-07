/*
 * Author: kayo
 */

#ifndef JVM_UTF8_H
#define JVM_UTF8_H

#include <cstring>
#include <cassert>

/*
 * save a utf8 string.
 * @utf8 必须是可持久存在的，不能是临时变量等等。
 */
const char *save_utf8(const char *utf8);
const char *find_saved_utf8(const char *utf8);

#define utf8_equals(s, t) (((s) == (t)) || (strcmp(s, t) == 0))

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

int utf8Len(const unsigned char *utf8);


struct Utf8Hash {
    /*
     * 计算字符串的hash值
     * todo 函数是啥意思
     */
    size_t operator()(const char *str) const
    {
        if (str == nullptr)
            return 0; // nullptr 的 hashcode 为0

        size_t h = 0;
        for (; *str != 0; str++) {
            h = 31 * h + (*str & 0xff);
        }
        return h;
    }
};

struct Utf8Comparator {
    bool operator()(const char *s1, const char *s2) const
    {
        assert(s1 != nullptr && s2 != nullptr);
        return utf8_equals(s1, s2);
    }
};

#endif //JVM_UTF8_H
