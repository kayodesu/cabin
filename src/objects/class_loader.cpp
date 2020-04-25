/*
 * Author: Yo Ka
 */

#include <sstream>
#include <optional>
#include <unordered_set>
#include <unordered_map>
#include <minizip/unzip.h>
#include "class_loader.h"
#include "../symbol.h"
#include "class.h"
#include "array_object.h"
#include "../interpreter/interpreter.h"
#include "../runtime/thread_info.h"
#include "prims.h"

using namespace std;
using namespace utf8;


#if TRACE_LOAD_CLASS
#define TRACE PRINT_TRACE
#else
#define TRACE(...)
#endif

static utf8_set bootPackages;
static unordered_map<const utf8_t *, Class *, utf8::Hash, utf8::Comparator> bootClasses;


enum ClassLocation {
    IN_JAR,
    IN_MODULE
};

/*
 * @param class_name: xxx/xxx/xxx
 */
static optional<pair<u1 *, size_t>> readClass(const char *path,
                                        const char *class_name, ClassLocation location)
{
    unzFile module_file = unzOpen64(path);
    if (module_file == nullptr) {
        thread_throw(new IOException(NEW_MSG("unzOpen64 failed: %s\n", path)));
    }

    if (unzGoToFirstFile(module_file) != UNZ_OK) {
        unzClose(module_file);
        thread_throw(new IOException(NEW_MSG("unzGoToFirstFile failed: %s\n", path)));
    }

    char buf[strlen(class_name) + 32]; // big enough
    if (location == IN_JAR) {
        strcat(strcpy(buf, class_name), ".class");
    } else if (location == IN_MODULE) {
        // All classes 放在 module 的 "classes" 目录下
        strcat(strcat(strcpy(buf, "classes/"), class_name), ".class");
    } else {
        jvm_abort("never goes here."); // todo
    }
// typedef int (*unzFileNameComparer)(unzFile file, const char *filename1, const char *filename2);
//    int k = unzLocateFile(module_file, buf, 1);
    int k  = unzLocateFile(module_file, buf,
                        [](unzFile file, const char *filename1, const char *filename2) {
                            return strcmp(filename1, filename2);
                        });
    if (k != UNZ_OK) {
        // not found
        unzClose(module_file);
        return nullopt;
    }

    // find out!
    if (unzOpenCurrentFile(module_file) != UNZ_OK) {
        unzClose(module_file);
        thread_throw(new IOException(NEW_MSG("unzOpenCurrentFile failed: %s\n", path)));
    }

    unz_file_info64 file_info{ };
    unzGetCurrentFileInfo64(module_file, &file_info, buf, sizeof(buf),
            nullptr, 0, nullptr, 0);

    auto bytecode = (u1 *) g_heap.allocBytecode(file_info.uncompressed_size); //new u1[file_info.uncompressed_size];
    int size = unzReadCurrentFile(module_file, bytecode, (unsigned int)(file_info.uncompressed_size));
    unzCloseCurrentFile(module_file);
    unzClose(module_file);
    if (size != file_info.uncompressed_size) {
        thread_throw(new IOException(NEW_MSG("unzReadCurrentFile failed: %s.\n", path)));
    }
    return make_pair(bytecode, file_info.uncompressed_size);
}

static void addClassToClassLoader(Object *classLoader, Class *c)
{
    assert(c != nullptr);
    if (classLoader == bootClassLoader) {
        bootClasses.insert(make_pair(c->className, c));
        return;
    }

    if (classLoader->classes == nullptr) {
        classLoader->classes = new unordered_map<const utf8_t *, Class *, utf8::Hash, utf8::Comparator>;
    }
    classLoader->classes->insert(make_pair(c->className, c));

    // Invoked by the VM to record every loaded class with this loader.
    // void addClass(Class<?> c);
//    Method *m = classLoader->clazz->getDeclaredInstMethod("addClass", "Ljava/lang/Class;");
//    assert(m != nullptr);
//    execJavaFunc(m, { (slot_t) classLoader, (slot_t) c });
}

Class *loadBootClass(const utf8_t *name)
{
    assert(name != nullptr);
    //assert(isSlashName(name));

    auto iter = bootClasses.find(name);
    if (iter != bootClasses.end()) {
        TRACE("find loaded class (%s) from pool.", className);
        return iter->second;
    }

    Class *c = nullptr;
    if (name[0] == '[' or isPrimClassName(name)) {
        c = Class::newClass(name);
    } else {
        if (g_jdk_version_9_and_upper) {
            for (auto &mod : g_jdk_modules) {
                auto content = readClass(mod.c_str(), name, IN_MODULE);
                if (content.has_value()) { // find out
                    c = defineClass(bootClassLoader, content->first,
                                    content->second); //content.value().first
                }
            }
        } else {
            for (auto &jar : jreLibJars) {
                auto content = readClass(jar.c_str(), name, IN_JAR);
                if (content.has_value()) { // find out
                    c = defineClass(bootClassLoader, content->first,
                                    content->second); //content.value().first
                }
            }
        }
    }

    if (c != nullptr) {
        bootPackages.insert(c->pkgName);
        addClassToClassLoader(bootClassLoader, c);
    }
    return c;
}

const utf8_t *getBootPackage(const utf8_t *name)
{
    auto iter = bootPackages.find(name);
    return iter != bootPackages.end() ? *iter : nullptr;
}

utf8_set &getBootPackages()
{
    return bootPackages;
}

Class *findLoadedClass(Object *classLoader, const utf8_t *name)
{
    assert(name != nullptr);
    //assert(isSlashName(name));

    if (classLoader == nullptr) {
        auto iter = bootClasses.find(name);
        return iter != bootClasses.end() ? iter->second : nullptr;
    }

    // is not boot classLoader
    if (classLoader->classes != nullptr) {
        auto iter = classLoader->classes->find(name);
        return iter != classLoader->classes->end() ? iter->second : nullptr;
    }

    // not find
    return nullptr;
}

Class *loadClass(Object *classLoader, const utf8_t *name)
{
    assert(name != nullptr);
    //assert(isSlashName(name));

    auto slashName = dots2SlashDup(name);
    Class *c = findLoadedClass(classLoader, slashName);
    if (c != nullptr)
        return c;

    // 先尝试用boot class loader load the class
    c = loadBootClass(slashName);
    if (c != nullptr || classLoader == nullptr)
        return c;

    // todo 再尝试用扩展classLoader load the class

    // public Class<?> loadClass(String name) throws ClassNotFoundException
    Method *m = classLoader->clazz->lookupInstMethod("loadClass", "(Ljava/lang/String;)Ljava/lang/Class;");
    assert(m != nullptr);

    auto dotName = slash2DotsDup(name);
    slot_t *slot = execJavaFunc(m, classLoader, newString(dotName));
    assert(slot != nullptr);
    c = (Class *) RSLOT(slot);
    addClassToClassLoader(classLoader, c);
    return c;
}

Class *defineClass(jref classLoader, u1 *bytecode, size_t len)
{
    return Class::newClass(classLoader, bytecode, len);
}

Class *defineClass(jref classLoader, jstrref name,
                   jarrref bytecode, jint off, jint len, jref protectionDomain, jstrref source)
{
    auto data = (u1 *) bytecode->data;
    return defineClass(classLoader, data + off, len);
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
    return RSLOT(execJavaFunc(get));
}

Class *objectClass;
Class *classClass = nullptr;
Class *stringClass;

void initClassLoader()
{
    objectClass = loadBootClass(S(java_lang_Object));
    classClass = loadBootClass(S(java_lang_Class));

    if (classClass->instFieldsCount > CLASS_CLASS_INST_FIELDS_COUNT) {
        jvm_abort("What the fuck! [%d]", classClass->instFieldsCount); // todo
    }

    objectClass->clazz = classClass->clazz = classClass;

    stringClass = loadBootClass(S(java_lang_String));
    stringClass->buildStrPool();
}

void printBootClassLoader()
{
    printvm("boot class loader.\n");
    for (auto iter : bootClasses) {
        printvm("%s\n", iter.first);
    }
    printvm("\n");
}

//void printClassLoader(Object *classLoader)
//{
//    if (classLoader == nullptr) {
//        printBootClassLoader();
//        return;
//    }
//
//    for (auto iter : classes) {
//        if (iter.first == classLoader) {
//            printvm("class loader %p.\n", classLoader);
//            for (auto k : iter.second) {
//                printvm("%s\n", k.first);
//            }
//            printvm("\n");
//        }
//    }
//}
//
//void printAllClassLoaders()
//{
//    printBootClassLoader();
//
//    for (auto iter : classes) {
//        printvm("class loader %p.\n", iter.first);
//        for (auto k : iter.second) {
//            printvm("%s\n", k.first);
//        }
//        printvm("\n");
//    }
//}

