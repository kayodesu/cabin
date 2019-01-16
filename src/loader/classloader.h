/*
 * Author: Jia Yang
 */

#ifndef JVM_CLASS_LOADER_H
#define JVM_CLASS_LOADER_H

#include <stdbool.h>

struct class;
struct object;
struct classloader;

struct classloader* classloader_create(bool is_bootstrap_loader);

/*
 * @lass_name: 全限定类名，不带 .class 后缀
 *
 * class names:
 *    - primitive types: boolean, byte, int ...
 *    - primitive arrays: [Z, [B, [I ...
 *    - non-array classes: java/lang/Object ...
 *    - array classes: [Ljava/lang/Object; ...
 */
struct class* classloader_load_class(struct classloader *loader, const char *class_name);

#define load_sys_class(class_name) classloader_load_class(g_bootstrap_loader, class_name)

struct class* classloader_find_class(const struct classloader *loader, const char *class_name);
struct class* classloader_get_jlclass(struct classloader *loader);
struct class* classloader_get_jlstring(struct classloader *loader);

void classloader_destroy(struct classloader *loader);

#endif //JVM_CLASS_LOADER_H
