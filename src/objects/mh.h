#ifndef CABIN_MH_H
#define CABIN_MH_H

#include "../cabin.h"

/*
 * MethodHandle 相关的操作函数
 */

class Method;
class Array;

void initMethodHandle();

jref findMethodType(const utf8_t *desc, jref loader);
jref findMethodType(ClsObj *rtype, Array *ptypes);

jref linkMethodHandleConstant(Class *caller_class, int ref_kind,
                              Class *defining_class, const char *name, Object *type);

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

void initMemberName(jref member_name, jref target);
void expandMemberName(jref member_name);
jref resolveMemberName(jref member_name, Class *caller);

// java/lang/invoke/MethodHandles 类的便利操作函数
namespace method_handles {
    jref getCaller();
}

#endif //CABIN_MH_H
