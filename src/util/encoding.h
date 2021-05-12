#ifndef CABIN_ENCODING_H
#define CABIN_ENCODING_H

#include <stdint.h>
#include <stddef.h>
#include <assert.h>
#include "../cabin.h"

/*
 * jvm内部使用的utf8是一种改进过的utf8，与标准的utf8有所不同，
 * // todo 说明null
 * 具体参考 jvms。
 *
 * this vm 操作的utf8字符串，要求以'\0'结尾并且不包含utf8的结束符.
 *
 * todo java.lang.String 内部是用utf16表示的，和Unicode有什么区别？？
 */

void init_utf8_pool();

// save a utf8 string to pool.
// 如不存在，返回新插入的值
// 如果已存在，则返回池中的值。
const utf8_t *save_utf8(const utf8_t *utf8);

// get utf8 from pool, return null if not exist.
const utf8_t *find_utf8(const utf8_t *utf8);

size_t utf8_hash(const utf8_t *utf8);
size_t utf8_length(const utf8_t *utf8);
bool utf8_equals(const utf8_t *p1, const utf8_t *p2);
utf8_t *utf8_dup(const utf8_t *utf8);

utf8_t *dot_to_slash(utf8_t *utf8);
utf8_t *dot_to_slash_dup(const utf8_t *utf8);

utf8_t *slash_to_dot(utf8_t *utf8);
utf8_t *slash_to_dot_dup(const utf8_t *utf8);

// 不会在buf后面添加字符串结束符'\u0000'
unicode_t *utf8_to_unicode(const utf8_t *utf8, size_t len);

// 由调用者 delete[] utf8 string
utf8_t *unicode_to_utf8(const unicode_t *unicode, size_t len);

// struct Utf8Hash {
//         size_t operator()(const utf8_t *utf8) const {
//             return utf8_hash(utf8);
//         }
//     };

// struct Utf8Comparator {
//     bool operator()(const utf8_t *s1, const utf8_t *s2) const {
//         assert(s1 != NULL && s2 != NULL);
//         return utf8_equals(s1, s2);
//     }
// };

#endif //CABIN_ENCODING_H