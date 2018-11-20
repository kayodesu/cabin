/*
 * Author: Jia Yang
 */

#ifndef JVM_CLASS_LOADER_H
#define JVM_CLASS_LOADER_H

struct classloader;

struct classloader* classloader_create();

/*
 * @lass_name: 全限定类名
 */
struct jclass* classloader_load_class(struct classloader *loader, const char *class_name);

void classloader_destroy(struct classloader *loader);

#endif //JVM_CLASS_LOADER_H
