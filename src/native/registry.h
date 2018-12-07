/*
 * Author: Jia Yang
 */

#ifndef JVM_REGISTRY_H
#define JVM_REGISTRY_H

struct stack_frame;

// 注册所有的本地方法
void register_all_native_methods();

//typedef void (* native_method_f)(struct stack_frame *);
static void empty_method(struct stack_frame *frame) { }
static void registerNatives(struct stack_frame *frame) { }

/*
 * 注册本地方法
 */
void register_native_method(const char *class_name, const char *method_name,
                            const char *method_descriptor, void (* method)(struct stack_frame *));

/*
 * 查找本地方法
 */
void (* find_native_method(const char *class_name,
                           const char *method_name, const char *method_descriptor))(struct stack_frame *);

//void print_registered_native_methods();

#endif //JVM_REGISTRY_H
