/*
 * Author: kayo
 */

#ifndef JVM_REGISTRY_H
#define JVM_REGISTRY_H

#define LOBJ "Ljava/lang/Object;"
#define LCLS "Ljava/lang/Class;"
#define LSTR "Ljava/lang/String;"

class Frame;

void initJNI();

typedef void (* native_method_t)(Frame *);

/*
 * 注册本地方法
 */
void registerNative(const char *className,
                    const char *methodName, const char *methodDescriptor, native_method_t method);

/*
 * 查找本地方法
 */
native_method_t findNative(const char *className, const char *methodName, const char *methodDescriptor);

#endif //JVM_REGISTRY_H
