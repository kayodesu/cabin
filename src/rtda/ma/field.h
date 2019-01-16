/*
 * Author: Jia Yang
 */

#ifndef JVM_JFIELD_H
#define JVM_JFIELD_H

#include <stdbool.h>
#include "access.h"
#include "../../slot.h"

struct class;
struct bytecode_reader;

struct field {
    struct class *clazz; // which class this field belongs to
    const char *name;
    const char *descriptor;

    // the declared type(class object) of this field
    // like, int k; the type of k is int.class
    NO_ACCESS struct object *type;

    u2 access_flags;
    bool category_two;

    int id;
//    union {
//        union {
//            jint i;
//            jfloat f;
//            jlong l;
//            jdouble d;
//            jref r;
//            uintptr_t u;
//        } static_value;
//        u4 offset;
//    } v; // value

    // 如果field的值已经在常量池中了，@constant_value_index 表示值在常量池中的索引。
    int constant_value_index;

    bool deprecated;
    const char *signature;

#if 0
    struct annotation *runtime_visible_annotations;
    u2 runtime_visible_annotations_num;

    struct annotation *runtime_invisible_annotations;
    u2 runtime_invisible_annotations_num;
#endif
};

#define INVALID_CONSTANT_VALUE_INDEX (-1)

void field_init(struct field *field, struct class *c, struct bytecode_reader *reader);

bool field_is_accessible_to(const struct field *field, const struct class *visitor);

struct object* field_get_type(struct field *field);

void field_release(struct field *field);

char* field_to_string(const struct field *field);

#endif //JVM_JFIELD_H
