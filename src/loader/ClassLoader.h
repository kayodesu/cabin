/*
 * Author: Jia Yang
 */

#ifndef JVM_CLASS_LOADER_H
#define JVM_CLASS_LOADER_H

#include <stdbool.h>

struct classloader;

struct classloader* classloader_create(bool is_bootstrap_loader);

/*
 * @lass_name: 全限定类名
 *
 * class names:
 *    - primitive types: boolean, byte, int ...
 *    - primitive arrays: [Z, [B, [I ...
 *    - non-array classes: java/lang/Object ...
 *    - array classes: [Ljava/lang/Object; ...
 */
struct jclass* classloader_load_class(struct classloader *loader, const char *class_name);

struct jclass* classloader_get_jlclass(struct classloader *loader);
struct jclass* classloader_get_jlstring(struct classloader *loader);

void classloader_destroy(struct classloader *loader);

#endif //JVM_CLASS_LOADER_H
