/*
 * Author: Jia Yang
 */

#ifndef JVM_STRPOOL_H
#define JVM_STRPOOL_H

#include "../../jvmdef.h"

void build_str_pool();

Object *put_str_to_pool(ClassLoader *loader, const char *str0);

Object *put_so_to_pool(Object *so);

Object *get_str_from_pool(ClassLoader *loader, const char *str0);

#endif //JVM_STRPOOL_H
