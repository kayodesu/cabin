#include <assert.h>
#include <string.h>
#include <pthread.h>
#include "encoding.h"
#include "../cabin.h"
#include "hash.h"

static PHS utf8_set;

static pthread_rwlock_t lock = PTHREAD_RWLOCK_INITIALIZER;

void init_utf8_pool()
{
    phs_init(&utf8_set, (point_hash_func) utf8_hash, (point_equal_func) utf8_equals);
}

const utf8_t *save_utf8(const utf8_t *utf8)
{
    assert(utf8 != NULL);

    pthread_rwlock_wrlock(&lock);

    // const utf8_t *s = *utf8Set.insert(utf8).first;
    const utf8_t *s = (const utf8_t *) phs_add(&utf8_set, utf8);

    pthread_rwlock_unlock(&lock);
    return s;
}

const utf8_t *find_utf8(const utf8_t *utf8)
{
    assert(utf8 != NULL);

    pthread_rwlock_rdlock(&lock);

    // auto iter = utf8Set.find(utf8);
    // const utf8_t *s = iter == utf8Set.end() ? NULL : *iter;

    const utf8_t *s = (const utf8_t *) phs_find(&utf8_set, utf8);

    pthread_rwlock_unlock(&lock);
    return s;
}

static inline unicode_t getUtf8Char(const utf8_t *utf8, int *utf8_char_count)
{
    assert(utf8 != NULL && utf8_char_count != NULL);
    *utf8_char_count = 0;

    unicode_t x = *utf8++;
    (*utf8_char_count)++;
    if (x & 0x80) {
        unicode_t y = *utf8++;
        (*utf8_char_count)++;
        if (x & 0x20) {
            unicode_t z = *utf8++;
            (*utf8_char_count)++;
            return (unicode_t) (((x & 0xf) << 12) + ((y & 0x3f) << 6) + (z & 0x3f));
        } else {
            return (unicode_t) (((x & 0x1f) << 6) + (y & 0x3f));
        }
    }

    return x;
}

size_t utf8_hash(const utf8_t *utf8)
{
    if (utf8 == NULL)
        return 0;

    size_t hash = 0;
    while (*utf8) {
        int utf8_char_count;
        hash = hash * 37 + getUtf8Char(utf8, &utf8_char_count);
        utf8 += utf8_char_count;
    }

    return hash;
}

size_t utf8_length(const utf8_t *utf8)
{
    assert(utf8 != NULL);

    size_t count;
    for(count = 0; *utf8; count++) {
        int x = *utf8;
        utf8 += (x & 0x80) ? ((x & 0x20) ? 3 : 2) : 1;
    }

    return count;
}

#if 0
bool utf8_equals(const utf8_t *p1, const utf8_t *p2)
{
    assert(p1 != NULL && p2 != NULL);

    if (p1 == p2)
        return true;

    while(*p1 && *p2) {
        if(getUtf8Char(p1) != getUtf8Char(p2))
            return false;
    }

    return !(*p1 || *p2);
}
#endif

bool utf8_equals(const utf8_t *p1, const utf8_t *p2)
{
    assert(p1 != NULL && p2 != NULL);
    return strcmp(p1, p2) == 0;
}


utf8_t *utf8_dup(const utf8_t *utf8)
{
    assert(utf8 != NULL);
    // 因为jvm改进的utf8中除结束符外不包含'\0'，所以用strcpy即可。
    // char *p = (char *) malloc((strlen(utf8) + 1)*sizeof(char));
    // strcpy(p, utf8);
    // return p;
    return strdup(utf8);
}

utf8_t *dot_to_slash(utf8_t *utf8)
{
    assert(utf8 != NULL);

    for(utf8_t *tmp = utf8; *tmp; tmp++) {
        if (*tmp == '.')
            *tmp = '/';
    }

    return utf8;
}

utf8_t *dot_to_slash_dup(const utf8_t *utf8)
{
    assert(utf8 != NULL);
    return dot_to_slash(utf8_dup(utf8));
}

utf8_t *slash_to_dot(utf8_t *utf8)
{
    assert(utf8 != NULL);

    for(utf8_t *tmp = utf8; *tmp; tmp++) {
        if (*tmp == '/')
            *tmp = '.';
    }

    return utf8;
}

utf8_t *slash_to_dot_dup(const utf8_t *utf8)
{
    assert(utf8 != NULL);
    return slash_to_dot(utf8_dup(utf8));
}

unicode_t *utf8_to_unicode(const utf8_t *utf8, size_t len)
{
    assert(utf8 != NULL);

    unicode_t *buf = vm_malloc(sizeof(unicode_t) * (len + 1));
    buf[len] = 0;

    unicode_t *tmp = buf;
    while (*utf8) {
        int utf8_char_count;
        *tmp++ = getUtf8Char(utf8, &utf8_char_count);
        utf8 += utf8_char_count;
    }

    assert(buf[len] == 0);
    return buf;
}

// unicode_t *utf8::toUnicode(const utf8_t *utf8, jsize utf8_len, unicode_t *buf)
// {
//     assert(utf8 != NULL);
//     if (buf == NULL) {
//         buf = new unicode_t[utf8_len + 1];
//     }

//     JVM_PANIC("not implement.");  //  todo
// }

// 将此unicode转化为utf8时，有多少字节
static size_t utf8ByteCount(const unicode_t *unicode, size_t len)
{
    assert(unicode != NULL);
    size_t count = 0;

    for(; len > 0; len--) {
        unicode_t c = *unicode++;
        count += (c == 0 || c > 0x7f) ? (c > 0x7ff ? 3 : 2) : 1;
    }

    return count;
}

utf8_t *unicode_to_utf8(const unicode_t *unicode, size_t len)
{
    assert(unicode != NULL);

    utf8_t *utf8 = vm_malloc(sizeof(utf8_t) * (utf8ByteCount(unicode, len) + 1)); 
    utf8_t *p = utf8;

    for(; len > 0; len--) {
        unicode_t c = *unicode++;
        if((c == 0) || (c > 0x7f)) {
            if(c > 0x7ff) {
                *p++ = (utf8_t) ((c >> 12) | 0xe0);
                *p++ = (utf8_t) (((c >> 6) & 0x3f) | 0x80);
            } else {
                *p++ = (utf8_t) ((c >> 6) | 0xc0);
            }
            *p++ = (utf8_t) ((c & 0x3f) | 0x80);
        } else {
            *p++ = (utf8_t) (c);
        }
    }

    *p = 0;
    return utf8;
}
