/*
 * Author: Jia Yang
 */

#ifndef JVM_CLASS_LOADER_H
#define JVM_CLASS_LOADER_H

#include <stdbool.h>
#include "../jvmdef.h"

ClassLoader *classloader_create(bool is_bootstrap_loader);

/*
 * @lass_name: 全限定类名，不带 .class 后缀
 *
 * class names:
 *    - primitive types: boolean, byte, int ...
 *    - primitive arrays: [Z, [B, [I ...
 *    - non-array classes: java/lang/Object ...
 *    - array classes: [Ljava/lang/Object; ...
 */
Class *load_class(ClassLoader *loader, const char *class_name);
#define load_sys_class(class_name) load_class(g_bootstrap_loader, class_name)

Class *classloader_get_jlclass(ClassLoader *loader);
Class *classloader_get_jlstring(ClassLoader *loader);

void classloader_destroy(ClassLoader *loader);

#endif //JVM_CLASS_LOADER_H
