/*
 * Author: Jia Yang
 */

#include "jstrobj.h"
#include "mm/halloc.h"

struct jstrobj* jstrobj_create0(struct classloader *loader, const char *str)
{
    HEAP_ALLOC(struct jstrobj, so);

    so->obj = jobject_create(classloader_load_class(loader, "java/lang/String"));
    so->str = str;
    so->wstr = NULL; // todo
    return so;
}

struct jstrobj* jstrobj_create1(struct classloader *loader, const jchar *wstr)
{
    HEAP_ALLOC(struct jstrobj, so);

    so->obj = jobject_create(classloader_load_class(loader, "java/lang/String"));
    so->str = NULL; // todo
    so->wstr = wstr;
    return so;
}
