#ifndef CABIN_ATTRIBUTE_H
#define CABIN_ATTRIBUTE_H

#include "cabin.h"
#include "slot.h"

struct constant_pool;
struct element_value_pair;
struct bytecode_reader;

typedef struct annotation {
    // The value of the type_index item must be a valid index into the constant_pool table.
    // The constant_pool entry at that index must be a CONSTANT_Utf8_info structure representing a Field type.
    // The Field type denotes the type of the annotation represented  by this annotation structure.
    u2 type_index;
    
    struct element_value_pair *element_value_pairs;
    u2 element_value_pairs_count;
} Annotation;

void annotation_init(Annotation *anno, struct bytecode_reader *r);

/*
 The tag item uses a single ASCII character to indicate the type of the value of
 the element-value pair. This determines which item of the value union is in use.

 Interpretation of tag values as types
 tag Item    Type           value Item               constant Type
 ----------------------------------------------------------------------
 B          byte          const_value_index           CONSTANT_Integer
 C          char          const_value_index           CONSTANT_Integer
 D          double        const_value_index           CONSTANT_Double
 F          float         const_value_index           CONSTANT_Float
 I          int           const_value_index           CONSTANT_Integer
 J          long          const_value_index           CONSTANT_Long
 S          short         const_value_index           CONSTANT_Integer
 Z          boolean       const_value_index           CONSTANT_Integer
 s          String        const_value_index           CONSTANT_Utf8
 e          Enum type     enum_const_value            Not applicable
 c          Class         class_info_index            Not applicable
 @          Annotation    type annotation_value       Not applicable
 [          Array type    array_value                 Not applicable
 */
typedef struct element_value {
    u1 tag;

    union {
        u2 const_value_index;

        struct {
            u2 type_name_index;
            u2 const_name_index;
        } enum_const_value;
        
        u2 class_info_index;

        Annotation annotation_value;

        struct {
            struct element_value *value;
            u2 len;
        } array_value;
    };
} ElementValue;

TJE void element_value_init(ElementValue *ev, struct bytecode_reader *r); 

typedef struct element_value_pair {
    // The value of the element_name_index item must be a valid index into the constant_pool table.
    // The constant_pool entry at that index must be a CONSTANT_Utf8_info structure.
    // The constant_pool entry denotes the name of the element of the element-value pair
    // represented by this element_value_pairs entry.
    // In other words, the entry denotes an element of the annotation type specified by type_index.
    u2 element_name_index;
    ElementValue value;
} ElementValuePair;

void element_value_pair_init(ElementValuePair *pair, struct bytecode_reader *r);

typedef struct inner_class {
    u2 inner_class_info_index;
    u2 outer_class_info_index;
    u2 inner_name_index;
    u2 inner_class_access_flags;
} InnerClass;

void inner_class_init(InnerClass *ic, struct bytecode_reader *r);

typedef struct bootstrap_method {
    /*
     * bootstrap_method_ref 项的值必须是一个对常量池的有效索引。
     * 常量池在该索引处的值必须是一个 CONSTANT_MethodHandle_info 结构。
     * 注意：此CONSTANT_MethodHandle_info结构的reference_kind项应为值6（REF_invokeStatic）或8（REF_newInvokeSpecial），
     * 否则在invokedynamic指令解析调用点限定符时，引导方法会执行失败。
     */
    u2 bootstrap_method_ref;

    /*
     * bootstrap_arguments 数组的每个成员必须是一个对常量池的有效索引。
     * 常量池在该索引出必须是下列结构之一：
     * CONSTANT_String_info, CONSTANT_Class_info, CONSTANT_Integer_info, CONSTANT_Long_info,
     * CONSTANT_Float_info, CONSTANT_Double_info, CONSTANT_MethodHandle_info, CONSTANT_MethodType_info。
     */
    u2 *args;
    u2 args_count;
} BootstrapMethod;

void bootstrap_method_init(BootstrapMethod *bm, struct bytecode_reader *r);
slot_t *resolve_bootstrap_method_args(BootstrapMethod *bm, struct constant_pool *cp, slot_t *result);

typedef struct line_number_table {
    // The value of the start_pc item must indicate the index into the code array
    // at which the code for a new line in the original source file begins.
    // The value of start_pc must be less than the value of the code_length
    // item of the Code attribute of which this LineNumberTable is an attribute.
    u2 start_pc;
    // The value of the line_number item must give the corresponding line number in the original source file.
    u2 line_number;
} LineNumberTable;

void line_number_table_init(LineNumberTable *t, struct bytecode_reader *r);

typedef struct method_parameter {
    const utf8_t *name;
    u2 access_flags;
} MethodParameter;

void method_parameter_init(MethodParameter *mp, struct constant_pool *cp, struct bytecode_reader *r);

typedef struct local_variable_table {
    u2 start_pc;
    u2 length;
    u2 name_index;
    u2 descriptor_index;
    u2 index;
} LocalVariableTable;

void local_variable_table_init(LocalVariableTable *t, struct bytecode_reader *r);

typedef struct local_variable_type_table {
    u2 start_pc;
    u2 length;
    u2 name_index;
    u2 signature_index;
    u2 index;
} LocalVariableTypeTable;

void local_variable_type_table_init(LocalVariableTypeTable *t, struct bytecode_reader *r);

typedef struct module {
    const utf8_t *module_name;
    u2 module_flags;
    const utf8_t *module_version;

    struct module_require {
        const utf8_t *require_module_name;
        u2 flags;
        // If requires_version is NULL, then no version information about the current module is present.
        const utf8_t *version;
    } *requires;
    u2 requires_count;

    struct module_export {
        const utf8_t *export_package_name;
        u2 flags;

        const utf8_t **exports_to;
        u2 exports_to_count;
    } *exports;
    u2 exports_count;

    struct module_open {
        const utf8_t *open_package_name;
        u2 flags;

        const utf8_t **opens_to;
        u2 opens_to_count;
    } *opens;
    u2 opens_count;

    const utf8_t **uses;
    u2 uses_count;

    struct module_provide {
        const utf8_t *class_name;

        const utf8_t **provides_with;
        u2 provides_with_count;
    } *provides;
    u2 provides_count;
} Module;

Module *module_create(struct constant_pool *cp, struct bytecode_reader *r);

#endif //CABIN_ATTRIBUTE_H
