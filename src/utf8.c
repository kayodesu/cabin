/*
 * Author: Jia Yang
 */

#include "utf8.h"
#include "util/hashmap.h"

//static HashSet set;
static HashMap map;

void utf8_init()
{
//    hashset_init(&set, (int (*)(const void *)) utf8_hash, (int (*)(const void *, const void *)) strcmp);
    hashmap_init_str_key(&map);
}

const char *find_hashed_utf8(const char *utf8, bool add_if_absent)
{
    const void *key = hashmap_contains_key(&map, utf8);
    if (key == NULL && add_if_absent) {
        hashmap_put(&map, utf8, NULL);
        return utf8;
    } else {
        return key;
    }

//    const void *data = hashset_find(&set, utf8);
//    if (data == NULL && add_if_absent) {
//        hashset_put(&set, utf8);
//        return utf8;
//    } else {
//        return data;
//    }

}

///*
// * 计算utf8的hash值
// * todo 函数是啥意思
// */
//int utf8_hash(const char *utf8)
//{
//    if (utf8 == NULL)
//        return 0; // NULL 值的 hashcode 为0
//
//    int h = 0;
//    for (; *utf8 != 0; utf8++) {
//        h = 31 * h + (*utf8 & 0xff);
//    }
//    return h;
//}
