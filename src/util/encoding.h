
#ifndef KAYOVM_ENCODING_H
#define KAYOVM_ENCODING_H

#include <cstdint>
#include <cstddef>
#include <cassert>
#include "../jvmstd.h"

/*
 * jvm内部使用的utf8是一种改进过的utf8，与标准的utf8有所不同，
 * // todo 说明null
 * 具体参考 jvms。
 *
 * this vm 操作的utf8字符串，要求以'\0'结尾并且不包含utf8的结束符.
 *
 * todo java.lang.String 内部是用utf16表示的，和Unicode有什么区别？？
 *
 * Author: Yo Ka
 */

namespace utf8 {
    // save a utf8 string to pool.
    const utf8_t *save(const utf8_t *utf8);

    // get utf8 from pool, return null if not exist.
    const utf8_t *find(const utf8_t *utf8);

    size_t hash(const utf8_t *utf8);

    size_t length(const utf8_t *utf8);

    bool equals(const utf8_t *p1, const utf8_t *p2);

    utf8_t *dup(const utf8_t *utf8);

    utf8_t *dots2Slash(utf8_t *utf8);
    utf8_t *dots2SlashDup(const utf8_t *utf8);

    utf8_t *slash2Dots(utf8_t *utf8);
    utf8_t *slash2DotsDup(const utf8_t *utf8);

    // 不会在buf后面添加字符串结束符'\u0000'
    unicode_t *toUnicode(const utf8_t *utf8, unicode_t *buf = nullptr);
    unicode_t *toUnicode(const utf8_t *utf8, jsize utf8_len, unicode_t *buf = nullptr);

    struct Hash {
        size_t operator()(const utf8_t *utf8) const {
            return hash(utf8);
        }
    };

    struct Comparator {
        bool operator()(const utf8_t *s1, const utf8_t *s2) const {
            assert(s1 != nullptr && s2 != nullptr);
            return equals(s1, s2);
        }
    };
}

namespace unicode {
    // 由调用者 delete[] utf8 string
    utf8_t *toUtf8(const unicode_t *unicode, size_t len);
}

#endif //KAYOVM_ENCODING_H
