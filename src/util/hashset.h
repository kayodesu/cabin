/*
 * Author: Jia Yang
 */

#ifndef JVM_HASHSET_H
#define JVM_HASHSET_H

struct hashset_entry;

typedef struct {
    struct hashset_entry *set;
    size_t len;
    size_t count;

    // compute hash of data
    int (*hash)(const void *);

    // cmp data.
    int (* cmp)(const void *, const void *);
} HashSet;

void hashset_init(HashSet *set, int (* hash)(const void *), int (* cmp)(const void *, const void *));

void hashset_put(HashSet *set, const void *data);

/*
 * return hashed data if present, otherwise return NULL
 */
const void* hashset_find(HashSet *set, const void *data);

#endif //JVM_HASHSET_H
