/*
 * Author: Jia Yang
 */

#include "strpool.h"
#include "../../util/hashmap.h"
#include "jobject.h"

static struct hashmap *pool; // string pool

void build_str_pool()
{
    pool = hashmap_create_str_key();
}

struct jobject* put_str_to_pool(struct classloader *loader, const char *str0)
{
    assert(str0 != NULL);
    struct jobject *so = hashmap_find(pool, str0);
    if (so == NULL) {
        so = jstrobj_create(str0);
        hashmap_put(pool, jstrobj_value(so), so);
    }
    return so;
}

struct jobject* put_so_to_pool(struct jobject *so)
{
    const char *str = jstrobj_value(so);
    assert(str != NULL);
    struct jobject *tmp = hashmap_find(pool, str);
    if (tmp == NULL) {
        hashmap_put(pool, str, so);
    }
    return so;
}

struct jobject* get_str_from_pool(struct classloader *loader, const char *str0)
{
    assert(str0 != NULL);
    struct jobject *so = hashmap_find(pool, str0);
    return so == NULL ? put_str_to_pool(loader, str0) : so;
}