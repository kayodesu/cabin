/*
 * Author: Jia Yang
 */

#include <assert.h>
#include <string.h>
#include "../jvm.h"
#include "hashmap.h"

typedef struct hashmap_item {
    int hash; // hash value of key
    const void *key;
    void *value;
    struct hashmap_item *next;
} Item;

static inline Item* item_create(int hash, const void *key, void *value, Item *next)
{
    Item *item = vm_malloc(sizeof(Item));
    item->hash = hash;
    item->key = key;
    item->value = value;
    item->next = next;
    return item;
}

// The default initial capacity - MUST be a power of two.
#define DEFAULT_INITIAL_CAPACITY (1 << 6) // aka 64

// The load factor used when none specified in constructor.
//#define DEFAULT_LOAD_FACTOR 0.75f

static int hash_string(const char *str);

void hashmap_init(HashMap *map, int (* hash)(const void *), int (* cmp)(const void *, const void *))
{
    assert(map != NULL);
    map->length = DEFAULT_INITIAL_CAPACITY;
    map->table = vm_calloc(sizeof(*(map->table)), map->length);
    map->size = 0;
    map->hash = hash;
    map->cmp = cmp;
}

void hashmap_init_str_key(HashMap *map)
{
    assert(map != NULL);
    hashmap_init(map, (int (*)(const void *)) hash_string, (int (*)(const void *, const void *)) strcmp);
}

//HashMap* hashmap_create(int (*hash)(const void *), int (* cmp)(const void *, const void *))
//{
//    assert(hash != NULL);
//    assert(cmp != NULL);
//
//    HashMap *map = vm_malloc(sizeof(HashMap));
////    map->length = DEFAULT_INITIAL_CAPACITY;
////    map->table = vm_calloc(sizeof(*(map->table)), map->length);
////    map->size = 0;
////    map->hash = hash;
////    map->cmp = cmp;
////    map->add_existing = add_existing;
//    hashmap_init(map, hash, cmp);
//    return map;
//}
//
//struct hashmap* hashmap_create_str_key()
//{
//    return hashmap_create((int (*)(const void *)) hash_string,
//                          (int (*)(const void *, const void *)) strcmp);
//}

static const Item *get_item(const HashMap*map, const void *key)
{
    assert(map != NULL);

    int hash = map->hash(key);
    int index = hash % map->length;
    assert(0 <= index && index < map->length);

    if (map->table[index] == NULL) { // 位置为空
        return NULL;
    }

    for (Item *curr = map->table[index]; curr != NULL; curr = curr->next) {
        // 先判断 hash 提速，如 key's hash 不等，则 key 肯定不相等
        if (curr->hash == hash && map->cmp(curr->key, key) == 0) { // existing
            return curr;
        }
    }

    return NULL;
}

int hashmap_size(const HashMap *map)
{
    assert(map != NULL);
    return map->size;
}

const void *hashmap_contains_key(const HashMap *map, const void *key)
{
    assert(map != NULL);
    const Item *item = get_item(map, key);
    if (item != NULL)
        return item->key;
    return NULL;
//    return get_item(map, key) != NULL;
}

void hashmap_put(HashMap *map, const void *key, void *value)
{
    assert(map != NULL);

    int hash = map->hash(key);
    int index = hash % map->length;
    assert(0 <= index && index < map->length);

    if (map->table[index] == NULL) { // 位置为空
        map->table[index] = item_create(hash, key, value, NULL);
        map->size++;
        return;
    }

    for (Item *curr = map->table[index]; curr != NULL; curr = curr->next) {
        // 先判断 hash 提速，如 key's hash 不等，则 key 肯定不相等
        if (curr->hash == hash && map->cmp(curr->key, key) == 0) { // existing
            if (!map->add_existing) { // 不允许 add existing key
                vm_unknown_error("key already existed");
            }
            return;
        }
    }

    map->table[index] = item_create(hash, key, value, map->table[index]); // don't exist, add to head.
    map->size++;
}

void* hashmap_find(const HashMap *map, const void *key)
{
    assert(map != NULL);
    const Item *item = get_item(map, key);
    return item != NULL ? item->value : NULL;
}

int hashmap_values(HashMap *map, void *values[])
{
    assert(map != NULL);

    for (int i = 0, k = 0; i < map->length; i++) {
        for (Item *item = map->table[i]; item != NULL; item = item->next) {
            values[k++] = item->value;
        }
    }
    return map->size;
}

#if (JVM_DEBUG)
void hashmap_print(const HashMap *map)
{
    assert(map != NULL);
    for (int i = 0; i < map->length; i++) {
        printf("\nindex = %d --------------------------- \n", i);
        for (Item *item = map->table[i]; item != NULL; item = item->next) {
            printf("%s\n", (char *) item->key);
        }
    }
}
#endif

void hashmap_destroy(HashMap *map)
{
    assert(map != NULL);
    free(map->table);
    free(map);
}

/*
 * 计算字符串的hash值
 * todo 函数是啥意思
 */
static int hash_string(const char *str)
{
    if (str == NULL)
        return 0; // NULL 值的 hashcode 为0

    int h = 0;
    for (; *str != 0; str++) {
        h = 31 * h + (*str & 0xff);
    }
    return h;
}
