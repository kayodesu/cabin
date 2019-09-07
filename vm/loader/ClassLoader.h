/*
 * Author: kayo
 */

#ifndef JVM_CLASS_LOADER_H
#define JVM_CLASS_LOADER_H

#include <unordered_map>
#include <cstring>
#include <cassert>
#include "../kayo.h"
#include "../utf8.h"
#include "bootstrap_class_loader.h"

class Class;
class ArrayClass;


class ClassLoader {
    std::unordered_map<const char *, Class *, Utf8Hash, Utf8Comparator> loadedClasses;

    Class *loading(const char *className);
    Class *loadNonArrClass(const char *class_name);

public:

    ClassLoader();
    ~ClassLoader();

    /*
     * @className: 全限定类名，不带 .class 后缀
     *
     * class names:
     *    - primitive types: boolean, byte, int ...
     *    - primitive arrays: [Z, [B, [I ...
     *    - non-array classes: java/lang/Object ...
     *    - array classes: [Ljava/lang/Object; ...
     */
    Class *loadClass(const char *className);

    void putToPool(const char *className, Class *c);
};

static inline Class *loadSysClass(const char *className)
{
    assert(className != nullptr);
    return bootClassLoader->loadClass(className);
};

static inline ArrayClass *loadArrayClass(const char *arrClassName)
{
    assert(arrClassName != nullptr);
    assert(arrClassName[0] == '['); // must be array class name
    return (ArrayClass *) bootClassLoader->loadClass(arrClassName);
};

#endif //JVM_CLASS_LOADER_H
