/*
 * 不支持存储 NULL
 * Author: Jia Yang
 */

#include <stdio.h>
#include <assert.h>
#include "hashset.h"
#include "../jvm.h"


// The default initial capacity - MUST be a power of two.
#define DEFAULT_INITIAL_CAPACITY (1 << 6) // aka 64

void hashset_init(struct hashset *set, int (* hash)(const void *), int (* cmp)(const void *, const void *))
{
    assert(set != NULL && hash != NULL && cmp != NULL);
    set->set = vm_calloc(DEFAULT_INITIAL_CAPACITY, sizeof(struct hashset_entry));
    set->len = DEFAULT_INITIAL_CAPACITY;
    set->count = 0;
    set->hash = hash;
    set->cmp = cmp;
}

static struct hashset_entry* hashset_find_entry(struct hashset *set, int hash, const void *data)
{
    assert(set != NULL);
    assert(data != NULL);

    int index = hash % set->len;
    struct hashset_entry *e = set->set + index;
    if (e->data == NULL) { // empty
       return NULL;
    }

    for (; e != NULL; e = e->next) {
        if (e->hash == hash && set->cmp(e->data, data) == 0)
            return e;
    }
    return NULL;
}

const void* hashset_find(struct hashset *set, const void *data)
{
    struct hashset_entry *e = hashset_find_entry(set, set->hash(data), data);
    return e != NULL ? e->data : NULL;
}

static inline void entry_init(struct hashset_entry *e, int hash, const void *data, struct hashset_entry *next)
{
    e->hash = hash;
    e->data = data;
    e->next = next;
}

void hashset_put(struct hashset *set, const void *data)
{
    assert(set != NULL);
    assert(data != NULL);

    int hash = set->hash(data);
    struct hashset_entry *e = hashset_find_entry(set, hash, data);
    if (e != NULL) { // already existed
        return;
    }

    int index = hash % set->len;
    e = set->set + index;
    if (e->data == NULL) { // empty
        entry_init(e, hash, data, NULL);
    } else {
        struct hashset_entry *t = vm_malloc(sizeof(struct hashset_entry));
        entry_init(t, hash, data, e->next);
        e->next = t;
    }
}
