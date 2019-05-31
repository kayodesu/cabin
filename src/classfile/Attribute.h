/*
 * Author: Jia Yang
 */

#ifndef JVM_ATTRIBUTE_H
#define JVM_ATTRIBUTE_H

#include <vector>
#include "../jtypes.h"

struct ElementValuePair;
class BytecodeReader;

struct Annotation {
    // The value of the type_index item must be a valid index into the constant_pool table.
    // The constant_pool entry at that index must be a CONSTANT_Utf8_info structure representing a Field descriptor.
    // The Field descriptor denotes the type of the annotation represented  by this annotation structure.
    u2 type_index;
    std::vector<ElementValuePair> elementValuePairs;

    Annotation() = default;
    ~Annotation() = default;

    void read(BytecodeReader &r);
};

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
struct ElementValue {
    u1 tag;

//    struct enum_const_value {
//        u2 type_name_index;
//        u2 const_name_index;
//    };

//    struct array_value {
//        u2 num_values;
//        struct element_value *values;
//    };

//    union {
//        u2 const_value_index;
//
//        struct {
//            u2 type_name_index;
//            u2 const_name_index;
//        } enum_const_value;
//
//        u2 class_info_index;
//
////        Annotation annotation_value;
////        struct array_value array_value;
//
////        std::vector<ElementValue> arrayValue;
//    } value;
    u2 const_value_index;
    u2 class_info_index;
    struct {
        u2 type_name_index;
        u2 const_name_index;
    } enum_const_value;

    Annotation annotation_value;

    std::vector<ElementValue> arrayValue;

    explicit ElementValue(BytecodeReader &r);
    ~ElementValue();
};

struct ElementValuePair {
    // The value of the element_name_index item must be a valid index into the constant_pool table.
    // The constant_pool entry at that index must be a CONSTANT_Utf8_info structure.
    // The constant_pool entry denotes the name of the element of the element-value pair
    // represented by this element_value_pairs entry.
    // In other words, the entry denotes an element of the annotation type specified by type_index.
    u2 element_name_index;
    ElementValue value;

    explicit ElementValuePair(BytecodeReader &r);
};

struct BootstrapMethod {
    /*
     * bootstrap_method_ref 项的值必须是一个对常量池的有效索引。
     * 常量池在该索引处的值必须是一个CONSTANT_MethodHandle_info结构。
     * 注意：此CONSTANT_MethodHandle_info结构的reference_kind项应为值6（REF_invokeStatic）或8（REF_newInvokeSpecial），
     * 否则在invokedynamic指令解析调用点限定符时，引导方法会执行失败。
     */
    u2 bootstrapMethodRef;
//    u2 num_bootstrap_arguments;
    /*
     * bootstrap_arguments 数组的每个成员必须是一个对常量池的有效索引。
     * 常量池在该索引出必须是下列结构之一：
     * CONSTANT_String_info, CONSTANT_Class_info, CONSTANT_Integer_info, CONSTANT_Long_info,
     * CONSTANT_Float_info, CONSTANT_Double_info, CONSTANT_MethodHandle_info, CONSTANT_MethodType_info。
     */
//    u2 *bootstrap_arguments;
    std::vector<u2> bootstrapArguments;

    explicit BootstrapMethod(BytecodeReader &r);
};

//struct code_attribute_exception_table {
//    /*
//     * The values of the two items start_pc and end_pc indicate the ranges in the
//code array at which the exception handler is active. The value of start_pc
//must be a valid index into the code array of the opcode of an instruction.
//The value of end_pc either must be a valid index into the code array of the
//opcode of an instruction or must be equal to code_length, the length of the
//code array. The value of start_pc must be less than the value of end_pc.
//The start_pc is inclusive and end_pc is exclusive; that is, the exception
//handler must be active while the program counter is within the interval
//[start_pc, end_pc).
//     */
//    u2 start_pc;
//    u2 end_pc;
//    /*
//     * The value of the handler_pc item indicates the start of the exception
//handler. The value of the item must be a valid index into the code array
//and must be the index of the opcode of an instruction.
//     */
//    u2 handler_pc;
//    /*
//     * If the value of the catch_type item is nonzero, it must be a valid index
//into the constant_pool table. The constant_pool entry at that index
//must be a CONSTANT_Class_info structure (§4.4.1) representing a class of
//exceptions that this exception handler is designated to catch. The exception
//handler will be called only if the thrown exception is an instance of the
//given class or one of its subclasses.
//The verifier checks that the class is Throwable or a subclass of Throwable (§4.9.2).
//If the value of the catch_type item is zero, this exception handler is called
//for all exceptions.
//This is used to implement finally (§3.13).
//     */
//    u2 catch_type;
//};

//struct inner_class {
//    /*
//     * The value of the inner_class_info_index item must be a valid index into
//the constant_pool table. The constant_pool entry at that index must be
//a CONSTANT_Class_info structure representing C. The remaining items in
//the classes array entry give information about C.
//     */
//    u2 inner_class_info_index;
//    /*
//     * If C is not a member of a class or an interface (that is, if C is a top-level
//class or interface (JLS §7.6) or a local class (JLS §14.3) or an anonymous
//class (JLS §15.9.5)), the value of the outer_class_info_index item must
//be zero.
//Otherwise, the value of the outer_class_info_index item must be a valid
//index into the constant_pool table, and the entry at that index must be
//a CONSTANT_Class_info structure representing the class or interface of
//which C is a member.
//     */
//    u2 outer_class_info_index;
//    u2 inner_name_index;
//    /*
//     * If C is anonymous (JLS §15.9.5), the value of the inner_name_index item
//must be zero.
//Otherwise, the value of the inner_name_index item must be a valid index
//into the constant_pool table, and the entry at that index must be a
//CONSTANT_Utf8_info structure (§4.4.7) that represents the original simple
//name of C, as given in the source code from which this class file was
//compiled.
//     */
//    u2 inner_class_access_flags;
//};

struct LineNumberTable {
    // The value of the start_pc item must indicate the index into the code array
    // at which the code for a new line in the original source file begins.
    // The value of start_pc must be less than the value of the code_length
    // item of the Code attribute of which this LineNumberTable is an attribute.
    u2 start_pc;
    // The value of the line_number item must give the corresponding line number in the original source file.
    u2 line_number;

    explicit LineNumberTable(BytecodeReader &r);
};

//struct local_variable_table {
//    u2 start_pc;
//    u2 length;
//    u2 name_index;
//    u2 descriptor_index;
//    u2 index;
//};
//
//struct local_variable_type_table {
//    u2 start_pc;
//    u2 length;
//    u2 name_index;
//    u2 signature_index;
//    u2 index;
//};
//
//struct parameter {
//    u2 name_index;
//    u2 access_flags;
//};
//
//struct parameter_annotation {
//    u2 num_annotations;
//    struct annotation *annotations;
//};

//void read_annotation(struct bytecode_reader *reader, struct annotation *a);
//void read_element_value(struct bytecode_reader *reader, struct element_value *ev);

#endif //JVM_ATTRIBUTE_H
