#ifndef CABIN_CLASS_LOADER_H
#define CABIN_CLASS_LOADER_H

#include "cabin.h"
#include "hash.h"
#include "constants.h"

void set_bootstrap_classpath(const char *bcp);

void set_classpath(const char *cp);
const char *get_classpath();

// Cache 常用的类
extern Class *g_object_class;
extern Class *g_class_class;
extern Class *g_string_class;

void init_class_loader();

/*
 * 加载 JDK 类库中的类，不包括Array Class.
 * xxx/xxx/xxx
 */
Class *load_boot_class(const utf8_t *name);

Class *load_array_class(Object *loader, const utf8_t *arr_class_name);

// Load byte[].class, boolean[].class, char[].class, short[].class, 
//      int[].class, float[].class, long[].class, double[].class.
TJE Class *load_type_array_class(ArrayType type);

const utf8_t *get_boot_package(const utf8_t *name);
struct point_hash_set *get_boot_packages();

/*
 * @name: 全限定类名，不带 .class 后缀
 *
 * class names:
 *    - primitive types: boolean, byte, int ...
 *    - primitive arrays: [Z, [B, [I ...
 *    - non-array classes: java/lang/Object ...
 *    - array classes: [Ljava/lang/Object; ...
 */
Class *load_class(Object *class_loader, const utf8_t *name);

Class *find_loaded_class(Object *class_loader, const utf8_t *name);

TJE Class *define_class(jref class_loader, u1 *bytecode, size_t len);

Class *define_class1(jref class_loader, jref name,
                     jarrRef bytecode, jint off, jint len, jref protection_domain, jref source);

/*
 * 类的初始化在下列情况下触发：
 * 1. 执行new指令创建类实例，但类还没有被初始化。
 * 2. 执行 putstatic、getstatic 指令存取类的静态变量，但声明该字段的类还没有被初始化。
 * 3. 执行 invokestatic 调用类的静态方法，但声明该方法的类还没有被初始化。
 * 4. 当初始化一个类时，如果类的超类还没有被初始化，要先初始化类的超类。
 * 5. 执行某些反射操作时。
 *
 * 每个类的此方法只会执行一次。
 *
 * 调用类的类初始化方法。
 * clinit are the static initialization blocks for the class, and static Field initialization.
 */
Class *init_class(Class *c);

Class *link_class(Class *c);

jref get_platform_class_loader();

/* todo
 * 返回 System Class Loader(sun/misc/Launcher$AppClassLoader) to loader user classes.
 *
 * 继承体系为：
 * java/lang/Object
 *     java/lang/ClassLoader
 *         java/security/SecureClassLoader
 *             java/net/URLClassLoader
 *                 sun/misc/Launcher$AppClassLoader
 */
jref get_app_class_loader();

/* some methods for testing */

#define IS_SLASH_CLASS_NAME(class_name) (strchr(class_name, '.') == NULL)
#define IS_DOT_CLASS_NAME(class_name) (strchr(class_name, '/') == NULL)

struct point_hash_map *get_all_boot_classes();

struct point_hash_set *get_all_class_loaders();

// void printBootLoadedClasses();
// void printClassLoader(Object *class_loader);

#endif // CABIN_CLASS_LOADER_H