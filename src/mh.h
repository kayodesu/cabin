#ifndef CABIN_MH_H
#define CABIN_MH_H

#include "cabin.h"

void init_method_handle();

jref findMethodType(const utf8_t *desc, jref loader);

jref linkMethodHandleConstant(Class *caller_class, int ref_kind,
                              Class *defining_class, const char *name, jref type);

// java/lang/invoke/MemberName 类的便利操作函数
// namespace member_name {
//     /*
//      * new MemberName
//      */
//     jref memberName(Method *m, jbyte refKind);

//     jbyte getRefKind(jref memberName);

//     bool isMethod(jref memberName);

//     bool isConstructor(jref memberName);

//     bool isField(jref memberName);

//     bool isType(jref memberName);

//     bool isStatic(jref memberName);
// }

TJE void init_member_name(jref member_name, jref target); 
TJE void expand_member_name(jref member_name); 
TJE jref resolve_member_name(jref member_name, Class *caller); 
jint get_members(jclsRef defc, jstrRef match_name,
                jstrRef match_sig, jint match_flags, jclsRef caller, jint skip, jref _results);
TJE jlong member_name_object_field_offset(jref member_name); 

// java/lang/invoke/MethodHandles 类的便利操作函数
// namespace method_handles {
    jref getCaller();
// }

// class java_lang_invoke_MemberName {
//     static Field *vmindex;
// public:
//     void init();
// };

#endif // CABIN_MH_H