#ifndef CABIN_HASH_H
#define CABIN_HASH_H

#include <stdbool.h>

typedef size_t (*point_hash_func)(const void *);
typedef bool (*point_equal_func)(const void *, const void *);


/*
 * A hash map of points.
 */

typedef struct point_hash_map PointHashMap;
typedef PointHashMap PHM;

struct point_hash_map {
    point_hash_func key_hash;
    point_equal_func key_equals;

    /*
     * The table, initialized on first use, and resized as
     * necessary. When allocated, length is always a power of two.
     * (We also tolerate length zero in some operations to allow
     * bootstrapping mechanics that are currently not needed.)
     */
    struct point_hash_map_node {
        size_t hash; // hash value of the key
        const void *key;
        void *value;
        struct point_hash_map_node *next;
    } **table; 
    int capacity;       

    // The number of key-value mappings contained in this PHM.
    int size; 
};

void phm_init(PHM *, point_hash_func key_hash, point_equal_func key_equals);
void phm_release(PHM *);

PHM *phm_create(point_hash_func key_hash, point_equal_func key_equals);
void phm_destroy(PHM *);

// 如不存在，返回新插入的Node
// 如果已存在，则返回池中被修改的Node。
struct point_hash_map_node *phm_insert(PHM *, const void *key, void *value);

void *phm_find(const PHM *, const void *key);

void phm_touch_values(const PHM *, void (* touch)(void *));

#if 0
#define PHM_TRAVERSAL(_phm, _value_type, _value_name, _do_value) \
    for (int _i = 0; _i < _phm->capacity; _i++) { \
        for (struct point_hash_map_node *_node = _phm->table[_i]; _node != NULL; _node = _node->next) { \
            _value_type _value_name = (_value_type) _node->value; \
            _do_value \
        } \
    }
#endif

// -------------------------------------------------------------------------------------------------

/*
 * A hash set of points.
 * 不支持save NULL
 */

typedef struct point_hash_set PointHashSet;
typedef PointHashSet PHS;

struct point_hash_set {
    PHM phm;
};

void phs_init(PHS *pset, point_hash_func hash, point_equal_func equals);
void phs_release(PHS *pset);

PHS *phs_create(point_hash_func hash, point_equal_func equals);
void phs_destroy(PHS *pset);

#define phs_size(phs) ((phs)->phm.size)

// 如不存在，返回新插入的值
// 如果已存在，则返回 set 中的值。
const void *phs_add(PHS *pset, const void *ele);

bool phs_contain(const PHS *pset, const void *ele);

/*
 * 查找 @pset，如果存在返回 @pset 中值，如不存在返回NULL
 */
const void *phs_find(const PHS *pset, const void *ele);

#define PHS_TRAVERSAL(_phs, _key_type, _name, _do_value) \
    do { \
        for (int _i = 0; _i < (_phs)->phm.capacity; _i++) { \
            for (struct point_hash_map_node *_node = (_phs)->phm.table[_i]; _node != NULL; _node = _node->next) { \
                _key_type _name = (_key_type) _node->key; \
                _do_value \
            } \
        } \
    } while(false)

#endif //CABIN_HASH_H
