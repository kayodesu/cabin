/*
 * Author: kayo
 */

#ifndef JVM_REGISTRY_H
#define JVM_REGISTRY_H

#define LOBJ "Ljava/lang/Object;"
#define LCLS "Ljava/lang/Class;"
#define LSTR "Ljava/lang/String;"

class Frame;

// 注册所有的本地方法
void register_all_native_methods();

typedef void (* native_method_t)(Frame *);

/*
 * 注册本地方法
 */
void register_native_method(
        const char *class_name, const char *method_name, const char *method_descriptor, native_method_t method);

/*
 * 查找本地方法
 */
native_method_t findNativeMethod(const char *class_name, const char *method_name, const char *method_descriptor);

#endif //JVM_REGISTRY_H
