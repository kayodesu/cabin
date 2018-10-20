/*
 * Author: Jia Yang
 */

#include "strpool.h"

static struct jobject *pool = NULL; // hash must be declared as a NULL-initialized pointer

struct jobject* put_str_to_pool(struct classloader *loader, const char *str0)
{
    struct jobject *so;
    HASH_FIND_STR(pool, str0, so);
    if (so != NULL) {
        return so;
    }

    so = jstrobj_create(loader, str0);
    HASH_ADD_KEYPTR(hh, pool, str0, strlen(str0), so);
    return so;
}

struct jobject* get_str_from_pool(struct classloader *loader, const char *str0)
{
    struct jobject *so;
    HASH_FIND_STR(pool, str0, so);
    return so == NULL ? put_str_to_pool(loader, str0) : so;
}