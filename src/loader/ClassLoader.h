/*
 * Author: Jia Yang
 */

#ifndef JVM_CLASS_LOADER_H
#define JVM_CLASS_LOADER_H

#include "../../lib/uthash/uthash.h"

struct classloader {
    struct jclass *jclass_class; // java.lang.Class 的类

    struct jclass *loaded_class_pool;

    struct jobject *classobj_pool; // java.lang.Class 类的对象池
};

struct classloader* classloader_create();

/*
 * @lass_name: 全限定类名
 */
struct jclass* classloader_load_class(struct classloader *loader, const char *class_name);

void classloader_destroy(struct classloader *loader);

#endif //JVM_CLASS_LOADER_H
