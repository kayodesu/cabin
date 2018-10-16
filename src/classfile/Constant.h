/*
 * Author: Jia Yang
 */

#ifndef JVM_CONSTANT_H
#define JVM_CONSTANT_H

#include "../jtypes.h"
#include "../util/bytecode_reader.h"

#define CLASS_CONSTANT					7
#define FIELD_REF_CONSTANT				9
#define DOUBLE_CONSTANT					6
#define METHOD_REF_CONSTANT             10
#define INTERFACE_METHOD_REF_CONSTANT   11
#define STRING_CONSTANT					8
#define INTEGER_CONSTANT                3
#define FLOAT_CONSTANT					4
#define LONG_CONSTANT					5
#define NAME_AND_TYPE_CONSTANT          12
#define UTF8_CONSTANT					1
#define METHOD_HANDLE_CONSTANT          15
#define METHOD_TYPE_CONSTANT            16
#define INVOKE_DYNAMIC_CONSTANT         18

/*
 * 常量池中的每一项都具备相同的格式特征——第一个字节作为类型标记用于识别该项是哪种类型的常量，称为“tag byte”。
 * 常量池的索引范围是1至constant_pool_count−1。
 */
struct class_constant {
    u1 tag;
    u2 name_index;
};

struct member_ref_constant {
    u1 tag;
    u2 class_index;
    u2 name_and_type_index;
};//, field_ref_constant, method_ref_constant, interface_method_ref_constant;

struct string_constant {
    u1 tag;
    u2 string_index;
};

struct four_bytes_num_constant {
    u1 tag;
    u1 bytes[4];
};//, integer_constant, float_constant;

struct eight_bytes_num_constant {
    u1 tag;
    u1 bytes[8];
}; //, long_constant, double_constant;

struct name_and_type_constant {
    u1 tag;
    u2 name_index;
    u2 descriptor_index;
};

struct utf8_constant {
    u1 tag;
    u2 length;
    u1 bytes[];
};

struct method_handle_constant {
    u1 tag;
    u1 reference_kind;
    u2 reference_index;
};

struct method_type_constant {
    u1 tag;
    u2 descriptor_index;
};

struct invoke_dynamic_constant {
    u1 tag;
    u2 bootstrap_method_attr_index;
    u2 name_and_type_index;
};

#define CONSTANT_TAG(constant_point) (*(u1 *)(constant_point))

void* parse_constant(struct bytecode_reader *reader);

#endif //JVM_CONSTANT_H
