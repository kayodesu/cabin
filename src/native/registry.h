/*
 * Author: Jia Yang
 */

#ifndef JVM_REGISTRY_H
#define JVM_REGISTRY_H

#include "../interpreter/stack_frame.h"

// 注册所有的本地方法
void register_all_native_methods();

typedef void (* native_method_f)(struct stack_frame *);

/*
 * 注册本地方法
 */
void register_native_method(const char *class_name, const char *method_name,
                            const char *method_descriptor, void (* method)(struct stack_frame *));

/*
 * 查找本地方法
 */
native_method_f find_native_method(const char *class_name, const char *method_name, const char *method_descriptor);

//void printRegisteredNativeMethods();

#endif //JVM_REGISTRY_H
