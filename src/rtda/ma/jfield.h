/*
 * Author: Jia Yang
 */

#ifndef JVM_JFIELD_H
#define JVM_JFIELD_H

#include <stdbool.h>
#include "access.h"
#include "../../classfile/classfile.h"
#include "../../slot.h"

struct jclass;

struct jfield {
    const struct jclass *jclass; // which class this member belongs to
    const char *name;
    const char *descriptor;

    u2 access_flags;

    // todo type_name 是什么东西
    const char *type_name;
//    JClassObj *type = nullptr;  todo
    bool category_two;// = false;
//    struct slot value; // value of this field.

    int id;

    // 如果field的值已经在常量池中了，@constant_value_index 表示值在常量池中的索引。
    int constant_value_index;
//    const static int INVALID_CONSTANT_VALUE_INDEX = -1;
};

#define INVALID_CONSTANT_VALUE_INDEX (-1)

struct jfield* jfield_create(const struct jclass *c, const struct member_info *info);

bool jfield_is_accessible_to(const struct jfield *field, const struct jclass *visitor);

struct jclassobj* jfield_get_type(struct jfield *field);

#endif //JVM_JFIELD_H
