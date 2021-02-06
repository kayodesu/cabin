#include <iostream>
#include <sstream>
#include <optional>
#include <unordered_set>
#include <unordered_map>
#include <minizip/unzip.h>
#include "class_loader.h"
#include "../symbol.h"
#include "../metadata/class.h"
#include "array.h"
#include "../interpreter/interpreter.h"
#include "../runtime/vm_thread.h"
#include "prims.h"
#include "java_classes.h"
#include "../classpath/classpath.h"

using namespace std;
using namespace utf8;


#if TRACE_LOAD_CLASS
#define TRACE PRINT_TRACE
#else
#define TRACE(...)
#endif

static utf8_set boot_packages;
static unordered_map<const utf8_t *, Class *, utf8::Hash, utf8::Comparator> boot_classes;

// vm中所有存在的 class loaders，include "boot class loader".
static unordered_set<const Object *> loaders;

static void addClassToClassLoader(Object *class_loader, Class *c)
{
    assert(c != nullptr);
    if (class_loader == BOOT_CLASS_LOADER) {
        boot_classes.insert(make_pair(c->class_name, c));
        return;
    }

    loaders.insert(class_loader);

    if (class_loader->classes == nullptr) {
        class_loader->classes = new unordered_map<const utf8_t *, Class *, utf8::Hash, utf8::Comparator>;
    }
    class_loader->classes->insert(make_pair(c->class_name, c));

    // Invoked by the VM to record every loaded class with this loader.
    // void addClass(Class<?> c);
//    Method *m = classLoader->clazz->getDeclaredInstMethod("addClass", "Ljava/lang/Class;");
//    assert(m != nullptr);
//    execJavaFunc(m, { (slot_t) classLoader, (slot_t) c });
}

Class *loadBootClass(const utf8_t *name)
{
    assert(name != nullptr);
    assert(isSlashName(name));
    assert(name[0] != '['); // don't load array class

    auto iter = boot_classes.find(name);
    if (iter != boot_classes.end()) {
        TRACE("find loaded class (%s) from pool.", name);
        return iter->second;
    }

    Class *c = nullptr;
    if (isPrimClassName(name)) {
        c = new Class(name);
    } else {
        auto content = readBootClass(name);
        if (content.has_value()) { // find out
            c = defineClass(BOOT_CLASS_LOADER, content->first, content->second);
        }
    }

    if (c != nullptr) {
        boot_packages.insert(c->pkg_name);        
        checkInjectedFields(c);
        addClassToClassLoader(BOOT_CLASS_LOADER, c);        
    }
    return c;
}

Class *loadArrayClass(Object *loader, const utf8_t *arr_class_name)
{
    assert(arr_class_name != nullptr);
    assert(arr_class_name[0] == '['); // must be array class name

    string ele_class_name = arrClassName2eleClassName(arr_class_name);
    Class *c = loadClass(loader, ele_class_name.c_str());
    if (c == nullptr)
        return nullptr; // todo

    /* Array Class 用它的元素的类加载器加载 */

    Class *arr_class = findLoadedClass(c->loader, arr_class_name);
    if (arr_class != nullptr)
        return arr_class; // find out
    
    arr_class = new Class(c->loader, arr_class_name);
    assert(arr_class != nullptr);
    if (arr_class->loader == BOOT_CLASS_LOADER)
        boot_packages.insert(arr_class->pkg_name); // todo array class 的pkg_name是啥
    addClassToClassLoader(arr_class->loader, arr_class);
    return arr_class;
}

Class *loadTypeArrayClass(ArrayType type)
{
    const char *arr_class_name;

    switch (type) {
        case JVM_AT_BOOLEAN: arr_class_name = S(array_Z); break;
        case JVM_AT_CHAR:    arr_class_name = S(array_C); break;
        case JVM_AT_FLOAT:   arr_class_name = S(array_F); break;
        case JVM_AT_DOUBLE:  arr_class_name = S(array_D); break;
        case JVM_AT_BYTE:    arr_class_name = S(array_B); break;
        case JVM_AT_SHORT:   arr_class_name = S(array_S); break;
        case JVM_AT_INT:     arr_class_name = S(array_I); break;
        case JVM_AT_LONG:    arr_class_name = S(array_J); break;
        default:
            SIGNAL_EXCEPTION(S(java_lang_UnknownError), "Invalid array type: %d\n", type);
            return nullptr;
    }

    return loadArrayClass(arr_class_name);
}

const utf8_t *getBootPackage(const utf8_t *name)
{
    auto iter = boot_packages.find(name);
    return iter != boot_packages.end() ? *iter : nullptr;
}

utf8_set &getBootPackages()
{
    return boot_packages;
}

Class *findLoadedClass(Object *class_loader, const utf8_t *name)
{
    assert(name != nullptr);
    assert(isSlashName(name));

    if (class_loader == nullptr) {
        auto iter = boot_classes.find(name);
        return iter != boot_classes.end() ? iter->second : nullptr;
    }

    // is not boot classLoader
    if (class_loader->classes != nullptr) {
        auto iter = class_loader->classes->find(name);
        return iter != class_loader->classes->end() ? iter->second : nullptr;
    }

    // not find
    return nullptr;
}

Class *loadClass(Object *class_loader, const utf8_t *name)
{
    assert(name != nullptr);
//    assert(isSlashName(name));

    auto slash_name = dot2SlashDup(name);
    Class *c = findLoadedClass(class_loader, slash_name);
    if (c != nullptr)
        return c;

    if (slash_name[0] == '[')
        return loadArrayClass(class_loader, slash_name);

    // 先尝试用boot class loader load the class
    c = loadBootClass(slash_name);
    if (c != nullptr || class_loader == nullptr)
        return c;

    // todo 再尝试用扩展classLoader load the class

    // public Class<?> loadClass(String name) throws ClassNotFoundException
    Method *m = class_loader->clazz->lookupInstMethod(S(loadClass), S(_java_lang_String__java_lang_Class));
    assert(m != nullptr);

    auto dot_name = slash2DotDup(name);
    slot_t *slot = execJavaFunc(m, { class_loader, newString(dot_name) });
    assert(slot != nullptr);
    auto co = (ClsObj *) slot::getRef(slot);
    assert(co != nullptr && co->jvm_mirror != nullptr);
    c = co->jvm_mirror;
    addClassToClassLoader(class_loader, c);
    return c;
}

Class *defineClass(jref class_loader, u1 *bytecode, size_t len)
{
    return new Class(class_loader, bytecode, len);
}

Class *defineClass(jref class_loader, jref name,
                   Array *bytecode, jint off, jint len, jref protection_domain, jref source)
{
    auto data = (u1 *) bytecode->data;
    return defineClass(class_loader, data + off, len);
}

Class *initClass(Class *c)
{
    assert(c != nullptr);

    // todo
    c->clinit();

    c->state = Class::INITED;
    return c;
}

Class *linkClass(Class *c)
{
    assert(c != nullptr);

    // todo

    c->state = Class::LINKED;
    return c;
}

Object *getSystemClassLoader()
{
    Class *scl = loadBootClass(S(java_lang_ClassLoader));
    assert(scl != nullptr);

    // public static ClassLoader getSystemClassLoader();
    Method *get = scl->getDeclaredStaticMethod(S(getSystemClassLoader), S(___java_lang_ClassLoader));
    return slot::getRef(execJavaFunc(get));
}

Class *g_object_class;
Class *g_class_class = nullptr;
Class *g_string_class;

void initClassLoader()
{
    g_object_class = loadBootClass(S(java_lang_Object));
    g_class_class = loadBootClass(S(java_lang_Class));

    // g_class_class 至此创建完成。
    // 在 g_class_class 创建完成之前创建的 Class 都没有设置 java_mirror 字段，现在设置下。
    for (auto iter: boot_classes) {
        Class *c = iter.second;
        c->generateClassObject();
    }

    g_string_class = loadBootClass(S(java_lang_String));
    g_string_class->buildStrPool();

    loaders.insert(BOOT_CLASS_LOADER);
}

unordered_map<const utf8_t *, Class *, utf8::Hash, utf8::Comparator> *getAllBootClasses()
{
    return &boot_classes;
}

const unordered_set<const Object *> &getAllClassLoaders()
{
    return loaders;
}

void printBootLoadedClasses()
{
    cout << "boot class loader." << endl;
    for (auto iter : boot_classes) {
        cout << iter.first << endl;
    }
}

void printClassLoader(Object *class_loader)
{
   if (class_loader == BOOT_CLASS_LOADER) {
       printBootLoadedClasses();
       return;
   }
   
   for (auto iter : *(class_loader->classes)) {
       cout << iter.first << endl;
   }
}
