/*
 * Author: kayo
 */

#ifndef KAYOVM_CLASS_LOADER_H
#define KAYOVM_CLASS_LOADER_H

#include <cassert>
#include <cstring>
#include <unordered_set>
#include "../util/encoding.h"

class Object;
class Class;

// Cache 常用的类
extern Class *objectClass;
extern Class *classClass;
extern Class *stringClass;

// java.lang.Class 类中实例变量的 slots count
const static int CLASS_CLASS_INST_FIELDS_COUNT = 12; // 12 slots

void initClassLoader();

#define bootClassLoader ((jref) nullptr)

Class *loadBootClass(const utf8_t *name);

static inline Class *loadArrayClass(const utf8_t *arrClassName)
{
    assert(arrClassName != nullptr);
    assert(arrClassName[0] == '['); // must be array class name
    return loadBootClass(arrClassName);
};

using utf8_set = std::unordered_set<const utf8_t *, utf8::Hash, utf8::Comparator>;

const utf8_t *getBootPackage(const utf8_t *name);
utf8_set &getBootPackages();

/*
 * @name: 全限定类名，不带 .class 后缀
 *
 * class names:
 *    - primitive types: boolean, byte, int ...
 *    - primitive arrays: [Z, [B, [I ...
 *    - non-array classes: java/lang/Object ...
 *    - array classes: [Ljava/lang/Object; ...
 */
Class *loadClass(Object *classLoader, const utf8_t *name);
Class *findLoadedClass(Object *classLoader, const utf8_t *name);

Class *defineClass(jref classLoader, u1 *bytecode, size_t len);

Class *defineClass(jref classLoader, jstrref name,
                   jarrref bytecode, jint off, jint len, jref protectionDomain, jstrref source = nullptr);

Class *initClass(Class *c);

Class *linkClass(Class *c);

/*
 * 返回 System Class Loader(sun/misc/Launcher$AppClassLoader) to loader user classes.
 *
 * 继承体系为：
 * java/lang/Object
 *     java/lang/ClassLoader
 *         java/security/SecureClassLoader
 *             java/net/URLClassLoader
 *                 sun/misc/Launcher$AppClassLoader
 */
Object *getSystemClassLoader();

/* some methods for testing */

static inline bool isSlashName(const utf8_t *className)
{
    return strchr(className, '.') == nullptr;
}

static inline bool isDotName(const utf8_t *className)
{
    return strchr(className, '/') == nullptr;
}

void printBootClassLoader();
//void printClassLoader(Object *classLoader);
//void printAllClassLoaders();

#endif //KAYOVM_CLASS_LOADER_H
