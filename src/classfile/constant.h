/*
 * Author: kayo
 */

#ifndef JVM_CONSTANT_H
#define JVM_CONSTANT_H

#include <cassert>

// All compile-time constants go here.
// from java.lang.invoke.MethodHandleNatives.java

// Constant pool entry types.
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
//#define CONSTANT_ResolvedMethodHandle (INT8_MAX - 6)
#define CONSTANT_ResolvedInterfaceMethod (INT8_MAX - 5)
#define CONSTANT_ResolvedMethod          (INT8_MAX - 4)
#define CONSTANT_ResolvedField           (INT8_MAX - 3)
#define CONSTANT_ResolvedClass           (INT8_MAX - 2)
#define CONSTANT_ResolvedString          (INT8_MAX - 1)
#define CONSTANT_Placeholder             INT8_MAX // long 和 double 的占位符

// MemberName
#define MN_IS_METHOD             0x00010000 // method (not constructor)
#define MN_IS_CONSTRUCTOR        0x00020000 // constructor
#define MN_IS_FIELD              0x00040000 // field
#define MN_IS_TYPE               0x00080000 // nested type
#define MN_CALLER_SENSITIVE      0x00100000 // @CallerSensitive annotation detected
#define MN_REFERENCE_KIND_SHIFT  24 // refKind
#define MN_REFERENCE_KIND_MASK   (0x0F000000 >> MN_REFERENCE_KIND_SHIFT)
// The SEARCH_* bits are not for MN.flags but for the matchFlags argument of MHN.getMembers:
#define MN_SEARCH_SUPERCLASSES   0x00100000
#define MN_SEARCH_INTERFACES     0x00200000

// Basic types as encoded in the JVM.
#define T_BOOLEAN    4
#define T_CHAR       5
#define T_FLOAT      6
#define T_DOUBLE     7
#define T_BYTE       8
#define T_SHORT      9
#define T_INT       10
#define T_LONG      11
#define T_OBJECT    12
//#define T_ARRAY   13
#define T_VOID      14
//#define T_ADDRESS 15
#define T_ILLEGAL   99

// Basic array types as encoded in the JVM.
#define AT_BOOLEAN   4
#define AT_CHAR      5
#define AT_FLOAT     6
#define AT_DOUBLE    7
#define AT_BYTE      8
#define AT_SHORT     9
#define AT_INT      10
#define AT_LONG     11

// Constant pool reference-kind codes, as used by CONSTANT_MethodHandle CP entries.
#define REF_NONE              0
#define REF_getField          1
#define REF_getStatic         2
#define REF_putField          3
#define REF_putStatic         4
#define REF_invokeVirtual     5
#define REF_invokeStatic      6
#define REF_invokeSpecial     7
#define REF_newInvokeSpecial  8
#define REF_invokeInterface   9
#define REF_LIMIT            10

static inline bool refKindIsValid(jint refKind)
{
    return (refKind > REF_NONE && refKind < REF_LIMIT);
}

static inline bool refKindIsField(jbyte refKind)
{
    assert(refKindIsValid(refKind));
    return (refKind <= REF_putStatic);
}

static inline bool refKindIsGetter(jbyte refKind)
{
    assert(refKindIsValid(refKind));
    return (refKind <= REF_getStatic);
}

static inline bool refKindIsSetter(jbyte refKind)
{
    return refKindIsField(refKind) && !refKindIsGetter(refKind);
}

static inline bool refKindIsMethod(jbyte refKind)
{
    return !refKindIsField(refKind) && (refKind != REF_newInvokeSpecial);
}

static inline bool refKindIsConstructor(jbyte refKind)
{
    return (refKind == REF_newInvokeSpecial);
}

static inline bool refKindHasReceiver(jbyte refKind)
{
    assert(refKindIsValid(refKind));
    return (refKind & 1) != 0;
}

static inline bool refKindIsStatic(jbyte refKind)
{
    return !refKindHasReceiver(refKind) && (refKind != REF_newInvokeSpecial);
}

static inline bool refKindDoesDispatch(jbyte refKind)
{
    assert(refKindIsValid(refKind));
    return (refKind == REF_invokeVirtual || refKind == REF_invokeInterface);
}

//static {
//final int HR_MASK = ((1 << REF_getField) |
//                     (1 << REF_putField) |
//                     (1 << REF_invokeVirtual) |
//                     (1 << REF_invokeSpecial) |
//                     (1 << REF_invokeInterface)
//);
//for (byte refKind = REF_NONE+1; refKind < REF_LIMIT; refKind++) {
//assert(refKindHasReceiver(refKind) == (((1<<refKind) & HR_MASK) != 0)) : refKind;
//}
//}

//static const char *refKindName(byte refKind)
// {
//    assert(refKindIsValid(refKind));
//    switch (refKind) {
//        case REF_getField:          return "getField";
//        case REF_getStatic:         return "getStatic";
//        case REF_putField:          return "putField";
//        case REF_putStatic:         return "putStatic";
//        case REF_invokeVirtual:     return "invokeVirtual";
//        case REF_invokeStatic:      return "invokeStatic";
//        case REF_invokeSpecial:     return "invokeSpecial";
//        case REF_newInvokeSpecial:  return "newInvokeSpecial";
//        case REF_invokeInterface:   return "invokeInterface";
//        default:                    return "REF_???";
//    }
//}

#endif //JVM_CONSTANT_H
