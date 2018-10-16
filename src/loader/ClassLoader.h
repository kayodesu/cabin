/*
 * Author: Jia Yang
 */

#ifndef JVM_CLASS_LOADER_H
#define JVM_CLASS_LOADER_H

#include "../../lib/uthash/uthash.h"

//struct loaded_class {
//    const char *class_name;
//    struct jclass *classe;
//    UT_hash_handle hh;
//};
//
//struct clsobj {
//    const char *class_name;
//    struct jclassobj *obj;
//    UT_hash_handle hh;
//};

struct classloader {
    struct jclass *jclass_class; // java.lang.Class 的类

    struct jclass *loaded_class_pool;

    struct jclassobj *classobj_pool; // java.lang.Class 类的对象池     std::set<JClassObj *>
};

struct classloader* classloader_create();

struct jclass* classloader_load_class(struct classloader *loader, const char *class_name);

void classloader_destroy(struct classloader *loader);

#endif //JVM_CLASS_LOADER_H
