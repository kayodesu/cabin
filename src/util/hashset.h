/*
 * Author: Jia Yang
 */

#ifndef JVM_HASHSET_H
#define JVM_HASHSET_H


struct hashset_entry {
    int hash;
    const void *data;
    struct hashset_entry *next;
};

struct hashset {
    struct hashset_entry *set;
    size_t len;
    size_t count;

    // compute hash of data
    int (*hash)(const void *);

    // cmp data.
    int (* cmp)(const void *, const void *);
};

void hashset_init(struct hashset *set, int (* hash)(const void *), int (* cmp)(const void *, const void *));

void hashset_put(struct hashset *set, const void *data);

/*
 * return hashed data if present, otherwise return NULL
 */
const void* hashset_find(struct hashset *set, const void *data);

#endif //JVM_HASHSET_H
