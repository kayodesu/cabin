/*
 * Author: kayo
 */

#ifndef JVM_CONSTANT_H
#define JVM_CONSTANT_H


#define CONSTANT_Invalid                0 // invalid constant
#define CONSTANT_Utf8                   1
#define CONSTANT_Integer                3
#define CONSTANT_Float                  4
#define CONSTANT_Long                   5
#define CONSTANT_Double                 6
#define CONSTANT_Class                  7
#define CONSTANT_String                 8
#define CONSTANT_Fieldref               9
#define CONSTANT_Methodref              10
#define CONSTANT_InterfaceMethodref     11
#define CONSTANT_NameAndType            12
#define CONSTANT_MethodHandle           15
#define CONSTANT_MethodType             16
#define CONSTANT_Dynamic                17
#define CONSTANT_InvokeDynamic          18
#define CONSTANT_Module                 19
#define CONSTANT_Package                20

// 以下为自定义常量，数值不同于以上定义的常量即可。
#define CONSTANT_ResolvedMethod         (INT8_MAX - 4)
#define CONSTANT_ResolvedField          (INT8_MAX - 3)
#define CONSTANT_ResolvedClass          (INT8_MAX - 2)
#define CONSTANT_ResolvedString         (INT8_MAX - 1)
#define CONSTANT_Placeholder            INT8_MAX // long 和 double 的占位符


// Bytecode Behaviors and Method Descriptors for Method Handles
//      Description              | Kind   | Interpretation                           | Method descriptor
#define REF_KIND_GET_FIELD          1    // getfield C.f:T                           | (C)T
#define REF_KIND_GET_STATIC         2    // getstatic C.f:T                          | ()T
#define REF_KIND_PUT_FIELD          3    // putfield C.f:T                           | (C,T)V
#define REF_KIND_PUT_STATIC         4    // putstatic C.f:T                          | (T)V
#define REF_KIND_INVOKE_VIRTUAL     5    // invokevirtual C.m:(A*)T                  | (C,A*)T
#define REF_KIND_INVOKE_STATIC      6    // invokestatic C.m:(A*)T                   | (A*)T
#define REF_KIND_INVOKE_SPECIAL     7    // invokespecial C.m:(A*)T                  | (C,A*)T
#define REF_KIND_NEW_INVOKE_SPECIAL 8    // new C; dup; invokespecial C.<init>:(A*)V | (A*)C
#define REF_KIND_INVOKE_INTERFACE   9    // invokeinterface C.m:(A*)T                | (C,A*)T

#endif //JVM_CONSTANT_H
