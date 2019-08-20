/*
 * Author: kayo
 */

#ifndef JVM_JFIELD_H
#define JVM_JFIELD_H

#include "Access.h"
#include "../../slot.h"
#include "../../jvm.h"
#include "../../util/BytecodeReader.h"
#include "Class.h"
#include "Member.h"

class Field: public Member {
    // the declared type(class Object) of this field
    // like, int k; the type of k is int.class
    ClassObject *type = nullptr;
public:
    bool categoryTwo;

    int id;

    // 如果field的值已经在常量池中了，@constant_value_index 表示值在常量池中的索引。
    int constant_value_index;

#if 0
    struct annotation *runtime_visible_annotations;
    u2 runtime_visible_annotations_num;

    struct annotation *runtime_invisible_annotations;
    u2 runtime_invisible_annotations_num;
#endif

public:
    Field(Class *c, BytecodeReader &r);
    ClassObject *getType();
    std::string toString() const;
};

#define INVALID_CONSTANT_VALUE_INDEX (-1)

#endif //JVM_JFIELD_H
