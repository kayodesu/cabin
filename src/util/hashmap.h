/*
 * Author: Jia Yang
 */

#ifndef JVM_HASHMAP_H
#define JVM_HASHMAP_H

#include <stdbool.h>

/*
 * hash map.
 * 可以存储 NULL
 */

typedef struct hashmap HashMap;

/*
 * @hash hash function, 不能为 NULL
 * @cmp value cmp function. 不能为 NULL
 */
HashMap* hashmap_create(int (*hash)(const void *), int (* cmp)(const void *, const void *), bool add_existing);

/*
 * 创建key为字符串（char*） 的hashmap
 */
HashMap* hashmap_create_str_key(bool add_existing);

/*
 * add a mapping to hashtable
 * 如已存在不会重复添加
 */
void hashmap_put(HashMap *map, const void *key, void *value);

bool hashmap_contains_key(const HashMap *map, const void *key);

/*
 * 返回key所对应的value。
 * 如返回NULL，有两种情况：
 * 1. 没有找到key的映射。
 * 2. value的值就是NULL。
 * 可以使用 hashmap_contains_key 函数区分则两种情况
 */
void* hashmap_find(const HashMap *map, const void *key);

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
