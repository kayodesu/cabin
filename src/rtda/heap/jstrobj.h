/*
 * Author: Jia Yang
 */

#ifndef JVM_JSTROBJ_H
#define JVM_JSTROBJ_H

#include "jobject.h"
#include "../../../lib/uthash/uthash.h"

/*
 * The Object of java.lang.String
 */
struct jstrobj {
    struct jobject *obj;

    const char *str; // key in hash table
    const jchar *wstr;

    UT_hash_handle hh; // makes this structure hashable
};

struct jstrobj* jstrobj_create0(struct classloader *, const char *);
struct jstrobj* jstrobj_create1(struct classloader *, const jchar *);

#define jstrobj_create(loader, str) \
    _Generic((str), \
        char *: jstrobj_create0, \
        const char *: jstrobj_create0, \
        jchar *: jstrobj_create1, \
        const jchar *: jstrobj_create1 \
    )(loader, str)


//#define jstrobj_create(loader, str) \
//    do { \
//        struct jstrobj* jstrobj_create0(struct classloader *, const char *); \
//        struct jstrobj* jstrobj_create1(struct classloader *, const jchar *); \
//        _Generic((str), const char *: jstrobj_create0,  const jchar *: jstrobj_create1)(loader, str); \
//    } while (0)


//const jchar* jstrobj_value(struct jstrobj *so);

#endif //JVM_JSTROBJ_H
