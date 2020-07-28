/*
 * Author: Yo Ka
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
extern Class *g_object_class;
extern Class *g_class_class;
extern Class *g_string_class;


void initClassLoader();

#define BOOT_CLASS_LOADER ((jref) nullptr)

/*
 * 加载 JDK 类库中的类，不包括Array Class.
 */
Class *loadBootClass(const utf8_t *name);

Class *loadArrayClass(Object *loader, const utf8_t *arr_class_name);

static inline Class *loadArrayClass(const utf8_t *arr_class_name)
{
    assert(arr_class_name != nullptr);
    assert(arr_class_name[0] == '['); // must be array class name
    return loadArrayClass(g_system_class_loader, arr_class_name);
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
Class *loadClass(Object *class_loader, const utf8_t *name);
Class *findLoadedClass(Object *class_loader, const utf8_t *name);

Class *defineClass(jref class_loader, u1 *bytecode, size_t len);

Class *defineClass(jref class_loader, jref name,
                   Array *bytecode, jint off, jint len, jref protection_domain, jref source = nullptr);

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

static inline bool isSlashName(const utf8_t *class_name)
{
    return strchr(class_name, '.') == nullptr;
}

static inline bool isDotName(const utf8_t *class_name)
{
    return strchr(class_name, '/') == nullptr;
}

void printBootClassLoader();
void printClassLoader(Object *class_loader);

#endif //KAYOVM_CLASS_LOADER_H
