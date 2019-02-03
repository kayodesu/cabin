/*
 * Author: Jia Yang
 */

#include <string.h>
#include "utf8.h"
#include "util/hashset.h"

static struct hashset set;

void utf8_init()
{
    hashset_init(&set, (int (*)(const void *)) utf8_hash, (int (*)(const void *, const void *)) strcmp);
}

const char* find_hashed_utf8(const char *utf8, bool add_if_absent)
{
    const void *data = hashset_find(&set, utf8);
    if (data == NULL && add_if_absent) {
        hashset_put(&set, utf8);
        return utf8;
    } else {
        return data;
    }
}

/*
 * 计算utf8的hash值
 * todo 函数是啥意思
 */
int utf8_hash(const char *utf8)
{
    if (utf8 == NULL)
        return 0; // NULL 值的 hashcode 为0

    int h = 0;
    for (; *utf8 != 0; utf8++) {
        h = 31 * h + (*utf8 & 0xff);
    }
    return h;
}