#ifndef KAYOVM_INVOKE_H
#define KAYOVM_INVOKE_H

#include "../jvmstd.h"

/*
 * java/lang/invoke 包下类的便利操作函数
 *
 * Author: Yo Ka
 */

class Method;
class Array;
class ClassObject;

// java/lang/invoke/MethodType 类的便利操作函数
namespace method_type {
    /*
     * new MethodType
     */
    jref fromMethodDescriptor(const utf8_t *descriptor, jref loader);

    /*
     * new MethodType
     */
    jref methodType(Method *m);

    /*
     * new MethodType
     */
    jref methodType(ClassObject *rtype, Array *ptypes);

    Array *parameterTypes(jref methodType);

    jstrref toMethodDescriptor(jref methodType);
}

// java/lang/invoke/MemberName 类的便利操作函数
namespace member_name {
    /*
     * new MemberName
     */
    jref memberName(Method *m, jbyte refKind);

    jbyte getRefKind(jref memberName);

    bool isMethod(jref memberName);

    bool isConstructor(jref memberName);

    bool isField(jref memberName);

    bool isType(jref memberName);

    bool isStatic(jref memberName);
}

// java/lang/invoke/MethodHandles 类的便利操作函数
namespace method_handles {
    jref getCaller();
}

#endif //KAYOVM_INVOKE_H
