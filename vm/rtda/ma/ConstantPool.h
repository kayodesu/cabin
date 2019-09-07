/*
 * Author: kayo
 */

#ifndef KAYOVM_CONSTANTPOOL_H
#define KAYOVM_CONSTANTPOOL_H

#include "../../slot.h"
#include "../../util/BytecodeReader.h"

struct ConstantPool {
    u1 *type;
    slot_t *info;
};

// Macros for accessing constant pool entries
#define CP_TYPE(cp, i)                   ((cp).type[i])
#define CP_INFO(cp, i)                   ((cp).info[i])

//#define CP_METHOD_CLASS(cp, i)           (u2)(cp).info[i]
//#define CP_METHOD_NAME_TYPE(cp, i)       (u2)((cp).info[i]>>16)
#define CP_INTERFACE_CLASS(cp, i)        (u2)(cp).info[i]
#define CP_INTERFACE_NAME_TYPE(cp, i)    (u2)((cp).info[i]>>16)
#undef CP_UTF8
#define CP_UTF8(cp, i)                   (char *)((cp).info[i])
#define CP_STRING(cp, i)                 CP_UTF8(cp, (u2)(cp).info[i])
#define CP_CLASS_NAME(cp, i)             CP_UTF8(cp, (u2)(cp).info[i])
#define CP_NAME_TYPE_NAME(cp, i)         CP_UTF8(cp, (u2)(cp).info[i])
#define CP_NAME_TYPE_TYPE(cp, i)         CP_UTF8(cp, (u2)((cp).info[i]>>16))

#define CP_FIELD_CLASS(cp, i)       (u2)(cp).info[i]
#define CP_FIELD_CLASS_NAME(cp, i)  CP_UTF8(cp, (u2)(cp).info[i])
#define CP_FIELD_NAME(cp, i)        CP_NAME_TYPE_NAME(cp, (u2)((cp).info[i]>>16))
#define CP_FIELD_TYPE(cp, i)        CP_NAME_TYPE_TYPE(cp, (u2)((cp).info[i]>>16))

#define CP_METHOD_CLASS       CP_FIELD_CLASS
#define CP_METHOD_CLASS_NAME  CP_FIELD_CLASS_NAME
#define CP_METHOD_NAME        CP_FIELD_NAME
#define CP_METHOD_TYPE        CP_FIELD_TYPE

#define CP_INT(cp, i)                    ISLOT((cp).info + (i))
#define CP_FLOAT(cp, i)                  FSLOT((cp).info + (i))
#define CP_LONG(cp, i)                   LSLOT((cp).info + (i))
#define CP_DOUBLE(cp, i)                 DSLOT((cp).info + (i))


#endif //KAYOVM_CONSTANTPOOL_H
