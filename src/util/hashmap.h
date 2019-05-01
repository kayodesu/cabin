/*
 * Author: Jia Yang
 */

#ifndef JVM_HASHMAP_H
#define JVM_HASHMAP_H

#include <stdio.h>
#include <stdbool.h>

/*
 * hash map.
 * key 不可以为 NULL，value可以为NULL
 */
typedef struct hashmap {
    struct hashmap_item **table;

    // length of the table
    size_t length;

    // compute hash of key
    int (* hash)(const void *);

    // cmp key.
    int (* cmp)(const void *, const void *);

    // The number of key-value mappings contained in this hashtable
    size_t size;

    // 是否允许 add existing key
    bool add_existing;
} HashMap;

void hashmap_init(HashMap *map, int (*hash)(const void *), int (* cmp)(const void *, const void *));

/*
 * 初始化以key为字符串（char*） 的hashmap
 */
void hashmap_init_str_key(HashMap *map);

/*
 * @hash hash function, 不能为 NULL
 * @cmp value cmp function. 不能为 NULL
 */
//HashMap* hashmap_create(int (* hash)(const void *), int (* cmp)(const void *, const void *));
//
///*
// * 创建key为字符串（char*） 的hashmap
// */
//HashMap* hashmap_create_str_key();

/*
 * add a mapping to hashtable
 * 如已存在不会重复添加
 */
void hashmap_put(HashMap *map, const void *key, void *value);

const void *hashmap_contains_key(const HashMap *map, const void *key);

/*
 * 返回key所对应的value。
 * 如返回NULL，有两种情况：
 * 1. 没有找到key的映射。
 * 2. value的值就是NULL。
 * 可以使用 hashmap_contains_key 函数区分则两种情况
 */
void *hashmap_find(const HashMap *map, const void *key);

int hashmap_size(const HashMap *map);

int hashmap_values(HashMap *map, void *values[]);

/*
 * 在每个映射的value上施加一个动作。
 */
//void hashmap_act_every_value(const HashMap *map, void (*act)(void *));

void hashmap_destroy(HashMap *map);

#if (JVM_DEBUG)
void hashmap_print(const HashMap *map);
#endif

#endif //JVM_HASHMAP_H
