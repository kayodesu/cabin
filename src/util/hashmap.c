/*
 * Author: Jia Yang
 */

#include <assert.h>
#include <string.h>
#include "../jvm.h"

struct item {
    int hash; // hash value of key
    const void *key;
    void *value;
    struct item *next;
};

static inline struct item* item_create(int hash, const void *key, void *value, struct item *next)
{
    VM_MALLOC(struct item, item);
    item->hash = hash;
    item->key = key;
    item->value = value;
    item->next = next;
    return item;
}

struct hashmap {
    struct item **table;

    // length of the table
    size_t length;

    // compute hash of key
    int (*hash)(const void *);

    // cmp key.
    int (* cmp)(const void *, const void *);

    // The number of key-value mappings contained in this hashtable
    size_t size;

    // 是否允许 add existing key
    bool add_existing;
};

// The default initial capacity - MUST be a power of two.
#define DEFAULT_INITIAL_CAPACITY (1 << 4) // aka 16

// The load factor used when none specified in constructor.
//#define DEFAULT_LOAD_FACTOR 0.75f

static int hash_string(const char *str);

struct hashmap* hashmap_create(int (*hash)(const void *), int (* cmp)(const void *, const void *), bool add_existing)
{
    assert(hash != NULL);
    assert(cmp != NULL);

    VM_MALLOC(struct hashmap, map);
    map->length = DEFAULT_INITIAL_CAPACITY;
    map->table = calloc(sizeof(*(map->table)), map->length); // todo NULL
    map->size = 0;
    map->hash = hash;
    map->cmp = cmp;
    map->add_existing = add_existing;
    return map;
}

struct hashmap* hashmap_create_str_key(bool add_existing)
{
    return hashmap_create((int (*)(const void *)) hash_string,
                          (int (*)(const void *, const void *)) strcmp,
                          add_existing);
}

static const struct item* get_item(const struct hashmap *map, const void *key)
{
    assert(map != NULL);

    int hash = map->hash(key);
    int index = hash % map->length;
    assert(0 <= index && index < map->length);

    if (map->table[index] == NULL) { // 位置为空
        return NULL;
    }

    for (struct item *curr = map->table[index]; curr != NULL; curr = curr->next) {
        // 先判断 hash 提速，如 key's hash 不等，则 key 肯定不相等
        if (curr->hash == hash && map->cmp(curr->key, key) == 0) { // existing
            return curr;
        }
    }

    return NULL;
}

int hashmap_size(const struct hashmap *map)
{
    assert(map != NULL);
    return map->size;
}

bool hashmap_contains_key(const struct hashmap *map, const void *key)
{
    assert(map != NULL);
    return get_item(map, key) != NULL;
}

void hashmap_put(struct hashmap *map, const void *key, void *value)
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

    for (struct item *curr = map->table[index]; curr != NULL; curr = curr->next) {
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

void* hashmap_find(const struct hashmap *map, const void *key)
{
    assert(map != NULL);
    const struct item *item = get_item(map, key);
    return item != NULL ? item->value : NULL;
}

int hashmap_values(struct hashmap *map, void *values[])
{
    assert(map != NULL);

    for (int i = 0, k = 0; i < map->length; i++) {
        for (struct item *item = map->table[i]; item != NULL; item = item->next) {
            values[k++] = item->value;
        }
    }
    return map->size;
}

#if (JVM_DEBUG)
void hashmap_print(const struct hashmap *map)
{
    assert(map != NULL);
    for (int i = 0; i < map->length; i++) {
        printf("\nindex = %d --------------------------- \n", i);
        for (struct item *item = map->table[i]; item != NULL; item = item->next) {
            printf("%s\n", (char *) item->key);
        }
    }
}
#endif

void hashmap_destroy(struct hashmap *map)
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
