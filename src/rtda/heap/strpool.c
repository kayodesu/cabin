/*
 * Author: Jia Yang
 */

#include "strpool.h"
#include "../../util/hashmap.h"
#include "object.h"
#include "strobj.h"

static HashMap *pool; // string pool

void build_str_pool()
{
    pool = hashmap_create_str_key(false);
}

Object* put_str_to_pool(ClassLoader *loader, const char *str0)
{
    assert(str0 != NULL);
    Object *so = hashmap_find(pool, str0);
    if (so == NULL) {
        so = strobj_create(str0);
        hashmap_put(pool, strobj_value(so), so);
    }
    return so;
}

Object* put_so_to_pool(Object *so)
{
    const char *str = strobj_value(so);
    assert(str != NULL);
    Object *tmp = hashmap_find(pool, str);
    if (tmp == NULL) {
        hashmap_put(pool, str, so);
    }
    return so;
}

Object* get_str_from_pool(ClassLoader *loader, const char *str0)
{
    assert(str0 != NULL);
    Object *so = hashmap_find(pool, str0);
    return so == NULL ? put_str_to_pool(loader, str0) : so;
}