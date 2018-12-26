/*
 * Author: Jia Yang
 */

#ifndef JVM_SYMREF_H
#define JVM_SYMREF_H


#include "../../jtypes.h"

/*
 * 定义及解析类，字段，方法的符号引用
 */

struct field_ref {
    const char *class_name;
    const char *name;
    const char *descriptor;

    struct class *resolved_class;
    struct field *resolved_field;
};

struct method_ref {
    const char *class_name;
    const char *name;
    const char *descriptor;

    struct class *resolved_class;
    struct method *resolved_method;
};

struct method_handle {
    u1 kind;
    union {
        struct field_ref *fr;
        struct method_ref *mr;
    } ref;
};

struct invoke_dynamic_ref {
    struct method_handle *handle;

    const struct name_and_type *nt;

    int argc;
    int args[]; // index in rtcp
};

struct class* resolve_class(const struct class *visitor, const char *class_name);

struct field* resolve_static_field_ref(const struct class *visitor, struct field_ref *ref);
struct field* resolve_non_static_field_ref(const struct class *visitor, struct field_ref *ref);

struct method* resolve_static_method_ref(const struct class *visitor, struct method_ref *ref);
struct method* resolve_non_static_method_ref(const struct class *visitor, struct method_ref *ref);


#endif //JVM_SYMREF_H
