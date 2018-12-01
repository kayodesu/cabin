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
    const struct jclass *jclass; // which class this field belongs to
    const char *name;
    const char *descriptor;
    /*
     * the declared type(class object) of this field
     */
    struct jobject *type;

    u2 access_flags;

    // todo type_name 是什么东西
    const char *type_name;
    bool category_two;

    int id;

    // 如果field的值已经在常量池中了，@constant_value_index 表示值在常量池中的索引。
    int constant_value_index;
};

#define INVALID_CONSTANT_VALUE_INDEX (-1)

struct jfield* jfield_create(const struct jclass *c, const struct member_info *info);

bool jfield_is_accessible_to(const struct jfield *field, const struct jclass *visitor);

struct jobject* jfield_get_type(struct jfield *field);

void jfield_destroy(struct jfield *field);

char* jfield_to_string(const struct jfield *field);

#endif //JVM_JFIELD_H
