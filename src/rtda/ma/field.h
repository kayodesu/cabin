/*
 * Author: Jia Yang
 */

#ifndef JVM_JFIELD_H
#define JVM_JFIELD_H

#include <stdbool.h>
#include "access.h"
#include "../../slot.h"
#include "../../jvm.h"
#include "../../jvmdef.h"
#include "../../util/bytecode_reader.h"

struct field {
    Class *clazz; // which class this field belongs to
    const char *name;
    const char *descriptor;

    // the declared type(class object) of this field
    // like, int k; the type of k is int.class
    NO_ACCESS Object *type;

    u2 access_flags;
    bool category_two;

    int id;

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

void field_init(Field *field, Class *c, BytecodeReader *reader);

bool field_is_accessible_to(const Field *field, const Class *visitor);

Object *field_get_type(Field *field);

void field_release(Field *field);

char *field_to_string(const Field *field);

#endif //JVM_JFIELD_H
