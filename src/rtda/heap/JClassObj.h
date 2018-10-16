/*
 * Author: Jia Yang
 */

#ifndef JVM_JCLASSOBJ_H
#define JVM_JCLASSOBJ_H

#include <string.h>
#include "../ma/jclass.h"
#include "jobject.h"
#include "mm/halloc.h"
#include "../../../lib/uthash/uthash.h"

struct jclassobj {
    struct jobject *obj;

    // 必须是全限定类名，用作 hash 表中的 key
    char *class_name;
    UT_hash_handle hh; // makes this structure hashable
};

/*
 * @jclass_class: class of java/lang/Class
 */
struct jclassobj* jclassobj_create(struct jclass *jclass_class, const char *class_name);

void jclassobj_destroy(struct jclassobj *co);


#endif //JVM_JCLASSOBJ_H
