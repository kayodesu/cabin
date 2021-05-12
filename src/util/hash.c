#include <assert.h>
#include <stdlib.h>
#include "hash.h"

/**
 * The default initial capacity - MUST be a power of two.
 */
#define DEFAULT_INITIAL_CAPACITY (1 << 4) // aka 16

/**
 * The maximum capacity, used if a higher value is implicitly specified
 * by either of the constructors with arguments.
 * MUST be a power of two <= 1<<30.
 */
#define MAXIMUM_CAPACITY (1 << 30)

/**
 * The load factor used when none specified in constructor.
 */
#define DEFAULT_LOAD_FACTOR 0.75f

typedef struct point_hash_map_node Node;

static inline void node_init(Node *n, size_t hash, const void *key, void *value)
{
    assert(n != NULL);
    n->hash = hash;
    n->key = key;
    n->value = value;
    n->next = NULL;
}

static inline Node *node_create(size_t hash, const void *key, void *value)
{
    Node *n = malloc(sizeof(Node));
    node_init(n, hash, key, value);
    return n;
}

#undef HASH
#undef EQUALS
#undef POS

#define HASH(_key) (this->key_hash == NULL ? (size_t) (_key) : this->key_hash(_key))
#define EQUALS(_k1, _k2) (this->key_equals == NULL ? ((_k2) == (_k2)) : this->key_equals(_k1, _k2))
#define POS(_hash_value) ((_hash_value) % (this)->capacity)

void phm_init(PHM *this, point_hash_func key_hash, point_equal_func key_equals)
{
    assert(this != NULL);

    this->key_hash = key_hash;
    this->key_equals = key_equals;
    this->capacity = DEFAULT_INITIAL_CAPACITY;
    this->table = calloc(this->capacity, sizeof(Node *)); // todo vm_calloc
    this->size = 0;
}

PHM *phm_create(point_hash_func key_hash, point_equal_func key_equals)
{
    PHM *this = malloc(sizeof(PHM)); // todo vm_malloc
    phm_init(this, key_hash, key_equals);
    return this;
}

Node *find_node(const PHM *this, const void *key)
{
    assert(this != NULL);
    for (Node *n = this->table[POS(HASH(key))]; n != NULL; n = n->next) {
        if (EQUALS(n->key, key)) {
            return n;
        }
    }
    return NULL; // not find
}

Node *phm_insert(PHM *this, const void *key, void *value)
{
    assert(this != NULL);
    Node *n = find_node(this, key);
    if (n != NULL) {
        n->value = value; // key is exist, replace the value
        return n;
    }

    // key is not exist
    size_t h = HASH(key);
    Node *new_node = node_create(h, key, value);

    size_t i = POS(h);
    if (this->table[i] == NULL) {
        this->table[i] = new_node;
    } else {
        new_node->next = this->table[i]->next;
        this->table[i]->next = new_node;
    }

    this->size++;
    return new_node;
}

void *phm_find(const PHM *this, const void *key)
{
    assert(this != NULL);

    Node *n = find_node(this, key);
    return n != NULL ? n->value : NULL;
}

void phm_touch_values(const PHM *this, void (* touch)(void *))
{
    assert(this != NULL && touch != NULL);

    for (int i = 0; i < this->capacity; i++) {
        for (Node *n = this->table[i]; n != NULL; n = n->next) {
            touch(n->value);
        }
    }
}

void phm_release(PHM *this)
{
    assert(this != NULL);
    // todo free Node
    free(this->table);
}

void phm_destroy(PHM *this)
{
    assert(this != NULL);
    phm_release(this);
    free(this);
}

// -------------------------------------------------------------------------------------------------

// Dummy value to associate with an Object in the backing Map
#define PRESENT NULL

void phs_init(PHS *phs, point_hash_func hash, point_equal_func equals)
{
    phm_init(&phs->phm, hash, equals);
}

void phs_release(PHS *phs)
{
    // todo
}

PHS *phs_create(point_hash_func hash, point_equal_func equals)
{
    PHS *phs = malloc(sizeof(PHS)); // todo vm_malloc
    phs_init(phs, hash, equals);
    return phs;
}

void phs_destroy(PHS *phs)
{
    phs_release(phs);
    free(phs);
}

const void *phs_add(PHS *phs, const void *ele)
{
    return phm_insert(&phs->phm, ele, PRESENT)->key;
}

bool phs_contain(const PHS *phs, const void *ele)
{
    struct point_hash_map_node *n = find_node(&phs->phm, ele);
    return n != NULL;
}

const void *phs_find(const PHS *phs, const void *ele)
{
    struct point_hash_map_node *n = find_node(&phs->phm, ele);
    return n != NULL ? n->key : NULL;
}
