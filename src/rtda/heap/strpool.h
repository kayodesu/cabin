/*
 * Author: Jia Yang
 */

#ifndef JVM_STRPOOL_H
#define JVM_STRPOOL_H

struct classloader;
struct jobject;

void build_str_pool();

struct jobject* put_str_to_pool(struct classloader *loader, const char *str0);

struct jobject* put_so_to_pool(struct jobject *so);

struct jobject* get_str_from_pool(struct classloader *loader, const char *str0);

#endif //JVM_STRPOOL_H
