/*
 * Author: Jia Yang
 */

#ifndef JVM_STRPOOL_H
#define JVM_STRPOOL_H

struct classloader;
struct object;

void build_str_pool();

struct object* put_str_to_pool(struct classloader *loader, const char *str0);

struct object* put_so_to_pool(struct object *so);

struct object* get_str_from_pool(struct classloader *loader, const char *str0);

#endif //JVM_STRPOOL_H
