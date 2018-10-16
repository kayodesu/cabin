/*
 * Author: Jia Yang
 */

#include "strpool.h"

struct jstrobj *pool = NULL; // hash must be declared as a NULL-initialized pointer

struct jstrobj* put_str_to_pool(struct classloader *loader, const char *str0)
{
    struct jstrobj *so;
    HASH_FIND_STR(pool, str0, so);
    if (so != NULL) {
        return so;
    }

    so = jstrobj_create(loader, str0);
    HASH_ADD(hh, pool, str, strlen(so->str), so);
    return so;
}

struct jstrobj* get_str_from_pool(struct classloader *loader, const char *str0)
{
    struct jstrobj *so;
    HASH_FIND_STR(pool, str0, so);
    return so == NULL ? put_str_to_pool(loader, str0) : so;
}