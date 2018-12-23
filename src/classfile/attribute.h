/*
 * Author: Jia Yang
 */

#ifndef JVM_ATTRIBUTE_H
#define JVM_ATTRIBUTE_H

#include "../jtypes.h"
#include "../util/bytecode_reader.h"

/* attribute name ******************************************************************** 可以出现的位置 */
#define Code "Code"                                                                  //                 method
#define ConstantValue "ConstantValue"                                                //           field
#define Signature "Signature"                                                        // ClassFile field method
#define Synthetic "Synthetic"                                                        // ClassFile       method
#define Deprecated "Deprecated"                                                      // ClassFile field method
#define LineNumberTable "LineNumberTable"                                            //                        Code
#define StackMapTable "StackMapTable"                                                //                        Code
#define LocalVariableTable "LocalVariableTable"                                      //                        Code
#define LocalVariableTypeTable "LocalVariableTypeTable"                              //                        Code
#define Exceptions "Exceptions"                                                      //                 method
#define AnnotationDefault "AnnotationDefault"                                        //                 method
#define SourceFile "SourceFile"                                                      // ClassFile
#define InnerClasses "InnerClasses"                                                  // ClassFile
#define EnclosingMethod "EnclosingMethod"                                            // ClassFile
#define SourceDebugExtension "SourceDebugExtension"                                  // ClassFile
#define BootstrapMethods "BootstrapMethods"                                          // ClassFile
#define MethodParameters "MethodParameters"                                          //                 method
#define RuntimeVisibleAnnotations "RuntimeVisibleAnnotations"                        // ClassFile field method
#define RuntimeInvisibleAnnotations "RuntimeInvisibleAnnotations"                    // ClassFile field method
#define RuntimeVisibleParameterAnnotations "RuntimeVisibleParameterAnnotations"      //                 method
#define RuntimeInvisibleParameterAnnotations "RuntimeInvisibleParameterAnnotations"  //                 method

/*****************************************************/
struct element_value;

struct element_value_pair {
    /*
     * The value of the element_name_index item must be a valid index into
the constant_pool table. The constant_pool entry at that index must
be a CONSTANT_Utf8_info structure (§4.4.7). The constant_pool
entry denotes the name of the element of the element-value pair
represented by this element_value_pairs entry.
In other words, the entry denotes an element of the annotation type specified by
type_index.
     */
    u2 element_name_index;
    struct element_value *value; // one
};

struct annotation {
    /*
     * The value of the type_index item must be a valid index into the
constant_pool table. The constant_pool entry at that index must be
a CONSTANT_Utf8_info structure (§4.4.7) representing a field descriptor
(§4.3.2). The field descriptor denotes the type of the annotation represented
by this annotation structure.
     */
    u2 type_index;

    u2 num_element_value_pairs;
    struct element_value_pair *element_value_pairs;
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
struct element_value {
    u1 tag;

    struct enum_const_value {
        u2 type_name_index;
        u2 const_name_index;
    };

    struct array_value {
        u2 num_values;
        struct element_value *values;
    };

    union {
        u2 const_value_index;
        struct enum_const_value enum_const_value;
        u2 class_info_index;
        struct annotation annotation_value;
        struct array_value array_value;
    } value;
};


/*
 * The AnnotationDefault attribute is a variable-length attribute in the attributes
 * table of certain method_info structures , namely those representing elements
 * of annotation types . The AnnotationDefault attribute records the
 * default value for the element represented by the method_info
 * structure. The Java Virtual Machine must make this default value available so it
 * can be applied by appropriate reflective APIs.

 * There may be at most one AnnotationDefault attribute in the attributes table
 * of a method_info structure which represents an element of an annotation type.
 */
//struct annotation_default_attribute {
//    struct attribute_common common;
//    struct element_value *default_value;
//};


/*****************************************************/
struct bootstrap_method {
    /*
     * bootstrap_method_ref 项的值必须是一个对常量池的有效索引。
     * 常量池在该索引处的值必须是一个CONSTANT_MethodHandle_info结构。
     * 注意：此CONSTANT_MethodHandle_info结构的reference_kind项应为值6（REF_invokeStatic）或8（REF_newInvokeSpecial），
     * 否则在invokedynamic指令解析调用点限定符时，引导方法会执行失败。
     */
    u2 bootstrap_method_ref;
    u2 num_bootstrap_arguments;
    /*
     * bootstrap_arguments 数组的每个成员必须是一个对常量池的有效索引。
     * 常量池在该索引出必须是下列结构之一：
     * CONSTANT_String_info, CONSTANT_Class_info, CONSTANT_Integer_info, CONSTANT_Long_info,
     * CONSTANT_Float_info, CONSTANT_Double_info, CONSTANT_MethodHandle_info, CONSTANT_MethodType_info。
     */
    u2 *bootstrap_arguments;
};

//struct bootstrap_methods_attribute {
//    struct attribute_common common;
//
//    u2 num_bootstrap_methods;
//    struct bootstrap_method *bootstrap_methods;
//};
/*****************************************************/
struct code_attribute_exception_table {
    /*
     * The values of the two items start_pc and end_pc indicate the ranges in the
code array at which the exception handler is active. The value of start_pc
must be a valid index into the code array of the opcode of an instruction.
The value of end_pc either must be a valid index into the code array of the
opcode of an instruction or must be equal to code_length, the length of the
code array. The value of start_pc must be less than the value of end_pc.
The start_pc is inclusive and end_pc is exclusive; that is, the exception
handler must be active while the program counter is within the interval
[start_pc, end_pc).
     */
    u2 start_pc;
    u2 end_pc;
    /*
     * The value of the handler_pc item indicates the start of the exception
handler. The value of the item must be a valid index into the code array
and must be the index of the opcode of an instruction.
     */
    u2 handler_pc;
    /*
     * If the value of the catch_type item is nonzero, it must be a valid index
into the constant_pool table. The constant_pool entry at that index
must be a CONSTANT_Class_info structure (§4.4.1) representing a class of
exceptions that this exception handler is designated to catch. The exception
handler will be called only if the thrown exception is an instance of the
given class or one of its subclasses.
The verifier checks that the class is Throwable or a subclass of Throwable (§4.9.2).
If the value of the catch_type item is zero, this exception handler is called
for all exceptions.
This is used to implement finally (§3.13).
     */
    u2 catch_type;
};

//struct code_attribute {
//    struct attribute_common common;
//
//    u2 max_stack;
//    u2 max_locals;
//
//    u4 code_length;
//    u1 *code;
//    /*
//     * todo
//     * code[]数组给出了实现当前方法的Java虚拟机字节码。
//     * code[]数组以按字节寻址的方式读入机器内存，
//     * 如果code[]数组的第一个字节是按以4字节边界对齐的话，
//     * 那么tableswitch和lookupswitch指令中所有涉及到的32位偏移量也都是按4字节长度对齐的
//     * （关于code[]数组边界对齐对字节码的影响，请参考相关的指令描述）。
//     * 本规范对关于code[]数组内容的详细约束有很多，将在后面单独章节（§4.9）中列出。
//     */
//
//    u2 exception_tables_length;
//    /*
//     * exception_table[]数组的每个成员表示code[]数组中的一个异常处理器（Exception Handler）。
//     * exception_table[]数组中，异常处理器顺序是有意义的（不能随意更改），详细内容见2.10节。
//     */
//    struct code_attribute_exception_table *exception_tables;
//
//    u2 attributes_count;
//    struct attribute_common **attributes;
//};
/*****************************************************/
/*
 * The ConstantValue attribute is a fixed-length attribute in the attributes table of
 * a field_info structure. A ConstantValue attribute represents the value of
 * a constant expression, and is used as follows:
 * If the ACC_STATIC flag in the access_flags item of the field_info structure is
 * set, then the field represented by the field_info structure is assigned the value
 * represented by its ConstantValue attribute as part of the initialization of the
 * class or interface declaring the field. This occurs prior to the invocation
 * of the class or interface initialization method of that class or interface.
 *
 * Otherwise, the Java Virtual Machine must silently ignore the attribute.
 *
 * The value of the constantvalue_index item must be a valid index into
 * the constant_pool table. The constant_pool entry at that index gives the
 * constant value represented by this attribute. The constant_pool entry must be
 * of a type appropriate to the field.
    ------------------------------------------
    Constant value attribute types
    ------------------------------------------
    Field Type                      | Entry Type
    ------------------------------------------
    long                            | CONSTANT_Long
    float                           | CONSTANT_Float
    double                          | CONSTANT_Double
    int, short, char, byte, boolean | CONSTANT_Integer
    String                          | CONSTANT_String
    ------------------------------------------
 */
//struct constant_value_attribute {
//    struct attribute_common common;
//    u2 constant_value_index;
//};
///*****************************************************/
//struct deprecated_attribute {
//    struct attribute_common common;
//};

/*****************************************************/
/*
 * The EnclosingMethod attribute is a fixed-length attribute in the attributes table
of a ClassFile structure (§4.1). A class must have an EnclosingMethod attribute
if and only if it represents a local class or an anonymous class (JLS §14.3, JLS
§15.9.5).
There may be at most one EnclosingMethod attribute in the attributes table of
a ClassFile structure.
 */
//struct enclosing_method_attribute {
//    struct attribute_common common;
//    /*
//     * The value of the class_index item must be a valid index into the
//constant_pool table. The constant_pool entry at that index must be a
//CONSTANT_Class_info structure (§4.4.1) representing the innermost class that
//encloses the declaration of the current class.
//     */
//    u2 class_index;
//    /*
//     * If the current class is not immediately enclosed by a method or constructor,
//then the value of the method_index item must be zero.
//Otherwise, the value of the method_index item must be a valid index into
//the constant_pool table. The constant_pool entry at that index must be a
//CONSTANT_NameAndType_info structure (§4.4.6) representing the name and
//type of a method in the class referenced by the class_index attribute above.
//     */
//    u2 method_index;
//};
/*****************************************************/
//struct exceptions_attribute {
//    struct attribute_common common;
//
//    u2 number_of_exceptions;
//    /*
//     * Each value in the exception_index_table array must be a valid index into
//     * the constant_pool table.
//     * The constant_pool entry at that index must be a CONSTANT_Class_info structure
//     * representing a class type that this method is declared to throw.
//     */
//    u2 *exception_index_table; // [number_of_exceptions];
//};
/*****************************************************/
struct inner_class {
    /*
     * The value of the inner_class_info_index item must be a valid index into
the constant_pool table. The constant_pool entry at that index must be
a CONSTANT_Class_info structure representing C. The remaining items in
the classes array entry give information about C.
     */
    u2 inner_class_info_index;
    /*
     * If C is not a member of a class or an interface (that is, if C is a top-level
class or interface (JLS §7.6) or a local class (JLS §14.3) or an anonymous
class (JLS §15.9.5)), the value of the outer_class_info_index item must
be zero.
Otherwise, the value of the outer_class_info_index item must be a valid
index into the constant_pool table, and the entry at that index must be
a CONSTANT_Class_info structure representing the class or interface of
which C is a member.
     */
    u2 outer_class_info_index;
    u2 inner_name_index;
    /*
     * If C is anonymous (JLS §15.9.5), the value of the inner_name_index item
must be zero.
Otherwise, the value of the inner_name_index item must be a valid index
into the constant_pool table, and the entry at that index must be a
CONSTANT_Utf8_info structure (§4.4.7) that represents the original simple
name of C, as given in the source code from which this class file was
compiled.
     */
    u2 inner_class_access_flags;
};

//struct inner_classes_attribute {
//    struct attribute_common common;
//
//    u2 number_of_classes;
//    struct inner_class *classes;
//};

/*****************************************************/
struct line_number_table {
    /*
     * The value of the start_pc item must indicate the index into the code array
at which the code for a new line in the original source file begins.
The value of start_pc must be less than the value of the code_length
item of the Code attribute of which this LineNumberTable is an attribute.
     */
    u2 start_pc;
    // The value of the line_number item must give the corresponding line number in the original source file.
    u2 line_number;
};
/*
 * The LineNumberTable attribute is an optional variable-length attribute in the
attributes table of a Code attribute (ยง4.7.3). It may be used by debuggers to
determine which part of the code array corresponds to a given line number in the
original source file.
If multiple LineNumberTable attributes are present in the attributes table of a
Code attribute, then they may appear in any order.
There may be more than one LineNumberTable attribute per line of a source file
in the attributes table of a Code attribute. That is, LineNumberTable attributes
may together represent a given line of a source file, and need not be one-to-one
with source lines.
 */
//struct line_number_table_attribute  {
//    struct attribute_common common;
//
//    u2 line_number_table_length;
//    struct line_number_table *line_number_tables;
//};

/*****************************************************/
struct local_variable_table {
    u2 start_pc;
    u2 length;
    u2 name_index;
    u2 descriptor_index;
    u2 index;
};
/*
 * The LocalVariableTable attribute is an optional variable-length attribute in the
attributes table of a Code attribute (ยง4.7.3). It may be used by debuggers to
determine the value of a given local variable during the execution of a method.
If multiple LocalVariableTable attributes are present in the attributes table of
a Code attribute, then they may appear in any order.
There may be no more than one LocalVariableTable attribute per local variable
in the attributes table of a Code attribute.
 */
//struct local_variable_table_attribute   {
//    struct attribute_common common;
//    u2 local_variable_table_length;
//    struct local_variable_table *local_variable_tables;
//};

/*****************************************************/
struct local_variable_type_table {
    u2 start_pc;
    u2 length;
    u2 name_index;
    u2 signature_index;
    u2 index;
};
/*
 *The LocalVariableTypeTable attribute is an optional variable-length attribute in
the attributes table of a Code attribute (ยง4.7.3). It may be used by debuggers to
determine the value of a given local variable during the execution of a method.
If multiple LocalVariableTypeTable attributes are present in the attributes
table of a given Code attribute, then they may appear in any order.
There may be no more than one LocalVariableTypeTable attribute per local
variable in the attributes table of a Code attribute.
The LocalVariableTypeTable attribute differs from the LocalVariableTable
attribute (ยง4.7.13) in that it provides signature information rather than descriptor
information. This difference is only significant for variables whose type uses a type variable
or parameterized type. Such variables will appear in both tables, while variables of other
types will appear only in LocalVariableTable.
 */
//struct local_variable_type_table_attribute  {
//    struct attribute_common common;
//
//    u2 local_variable_type_table_length;
//    struct local_variable_type_table *local_variable_type_tables;
//};
//

/*****************************************************/
struct parameter {
    u2 name_index;
    u2 access_flags;
};
/*
 * The MethodParameters attribute is a variable-length attribute in the attributes
 * table of a method_info structure . A MethodParameters attribute records
 * information about the formal parameters of a method, such as their names.
 * There may be at most one MethodParameters attribute in the attributes table of a method_info structure.
 */
//struct method_parameters_attribute {
//    struct attribute_common common;
//
//    u1 parameters_count;
//    struct parameter *parameters;
//};
/*****************************************************/

/*
 * The RuntimeVisibleAnnotations attribute records run-time visible
 * annotations on the declaration of the corresponding class, field, or method.
 * The Java Virtual Machine must make these annotations available so they can be
 * returned by the appropriate reflective APIs.
 *
 * The RuntimeInvisibleAnnotations attribute records run-time
 * invisible annotations on the declaration of the corresponding class, method, or field.
 * There may be at most one RuntimeInvisibleAnnotations attribute in the
 * attributes table of a ClassFile, field_info, or method_info structure.

 * The RuntimeInvisibleAnnotations attribute is similar to the
 * RuntimeVisibleAnnotations attribute (ยง4.7.16), except that the annotations
 * represented by a RuntimeInvisibleAnnotations attribute must not be made available
 * for return by reflective APIs, unless the Java Virtual Machine has been instructed to retain
 * these annotations via some implementation-specific mechanism such as a command line
 * flag. In the absence of such instructions, the Java Virtual Machine ignores this attribute.
 */
//struct runtime_annotations_attribute {
//    struct attribute_common common;
//
//    u2 num_annotations;
//    struct annotation **annotations;
//};// runtime_annotations_attr;//, runtime_visible_annotations_attr, runtime_invisible_annotations_attr;

/*****************************************************/
struct parameter_annotation {
    u2 num_annotations;
    struct annotation *annotations;
};
/*
 * The RuntimeVisibleParameterAnnotations attribute records run-time visible
 * annotations on the declarations of formal parameters of the corresponding method.
 * The Java Virtual Machine must make these annotations available so they can be
 * returned by the appropriate reflective APIs.

 * There may be at most one RuntimeVisibleParameterAnnotations attribute in
 * the attributes table of a method_info structure.

 * The RuntimeInvisibleParameterAnnotations attribute records run-time invisible
 * annotations on the declarations of formal parameters of the corresponding method.

 * There may be at most one RuntimeInvisibleParameterAnnotations attribute in
 * the attributes table of a method_info structure.

 * The RuntimeInvisibleParameterAnnotations attribute is similar to the
 * RuntimeVisibleParameterAnnotations attribute (ยง4.7.18), except that the
 * annotations represented by a RuntimeInvisibleParameterAnnotations attribute
 * must not be made available for return by reflective APIs, unless the Java Virtual Machine
 * has specifically been instructed to retain these annotations via some implementation-specific
 * mechanism such as a command line flag. In the absence of such instructions, the
 * Java Virtual Machine ignores this attribute.
 */
//struct runtime_parameter_annotations_attribute {
//    struct attribute_common common;
//
//    u2 num_parameters;
//    struct parameter_annotation *parameter_annotations;
//};//, runtime_visible_parameter_annotations_attr, runtime_invisible_parameter_annotations_attr;

/*****************************************************/
/*
 * The Signature attribute is a fixed-length attribute in the attributes table
of a ClassFile, field_info, or method_info structure (§4.1, §4.5, §4.6). A
Signature attribute records a signature (§4.7.9.1) for a class, interface, constructor,
method, or field whose declaration in the Java programming language uses type
variables or parameterized types. See The Java Language Specification, Java SE
8 Edition for details about these types.
 */
//struct signature_attribute {
//    struct attribute_common common;
//    /*
//     * the value of the signature_index item must be a valid index into the
//constant_pool table. the constant_pool entry at that index must be a
//constant_utf8_info structure (§4.4.7) representing a class signature if this
//signature attribute is an attribute of a classfile structure; a method
//signature if this signature attribute is an attribute of a method_info structure;
//or a field signature otherwise.
//     */
//    u2 signature_index;
//};

/*****************************************************/
/*
 * The SourceDebugExtension attribute is an optional attribute in the attributes
table of a ClassFile structure (ยง4.1).
There may be at most one SourceDebugExtension attribute in the attributes
table of a ClassFile structure.
 */
//struct source_debug_extension_attribute {
//    struct attribute_common common;
//    /*
//     * The debug_extension array holds extended debugging information which has
//no semantic effect on the Java Virtual Machine. The information is represented
//using a modified UTF-8 string (ยง4.4.7) with no terminating zero byte.
//
//Note that the debug_extension array may denote a string longer than that which can be
//represented with an instance of class String.
//     */
//    u1 *debug_extension;
//};


/*****************************************************/
/*
 * 如果一个类成员没有在源文件中出现，则必须标记带有Synthetic属性，或者设置ACC_SYNTHETIC标志。
 * 唯一的例外是某些与人工实现无关的、由编译器自动产生的方法，
 * 也就是说，Java编程语言的默认的实例初始化方法（无参数的实例初始化方法）、类初始化方法，
 * 以及Enum.values()和Enum.valueOf()等方法是不用使用Synthetic属性或ACC_SYNTHETIC标记的。
 */
//struct synthetic_attribute {
//    struct attribute_common common;
//};

/*****************************************************/
//// 不认识的attr，忽略之。
//struct unknown_attribute {
//    struct attribute_common common;
//};
//


struct attribute {
//    enum attr_type type;

    union {
        /*
         * The AnnotationDefault attribute is a variable-length attribute in the attributes
         * table of certain method_info structures , namely those representing elements
         * of annotation types . The AnnotationDefault attribute records the
         * default value for the element represented by the method_info
         * structure. The Java Virtual Machine must make this default value available so it
         * can be applied by appropriate reflective APIs.

         * There may be at most one AnnotationDefault attribute in the attributes table
         * of a method_info structure which represents an element of an annotation type.
         */
        struct element_value annotation_default_value; // annotation default attribute

        struct {
            u2 num;
            struct bootstrap_method *methods;
        } bootstrap_methods;

        struct {
            u2 max_stack;
            u2 max_locals;

            u4 code_length;
            u1 *code;
            /*
             * todo
             * code[]数组给出了实现当前方法的Java虚拟机字节码。
             * code[]数组以按字节寻址的方式读入机器内存，
             * 如果code[]数组的第一个字节是按以4字节边界对齐的话，
             * 那么tableswitch和lookupswitch指令中所有涉及到的32位偏移量也都是按4字节长度对齐的
             * （关于code[]数组边界对齐对字节码的影响，请参考相关的指令描述）。
             * 本规范对关于code[]数组内容的详细约束有很多，将在后面单独章节（§4.9）中列出。
             */

            u2 exception_tables_length;
            /*
             * exception_table[]数组的每个成员表示code[]数组中的一个异常处理器（Exception Handler）。
             * exception_table[]数组中，异常处理器顺序是有意义的（不能随意更改），详细内容见2.10节。
             */
            struct code_attribute_exception_table *exception_tables;

            u2 attributes_count;
            struct attribute *attributes;
        } code;

        u2 constant_value_index; // constant value attribute

        /*
         * The EnclosingMethod attribute is a fixed-length attribute in the attributes table
        of a ClassFile structure (§4.1). A class must have an EnclosingMethod attribute
        if and only if it represents a local class or an anonymous class (JLS §14.3, JLS
        §15.9.5).
        There may be at most one EnclosingMethod attribute in the attributes table of
        a ClassFile structure.
         */
        struct {
            /*
             * The value of the class_index item must be a valid index into the
            constant_pool table. The constant_pool entry at that index must be a
            CONSTANT_Class_info structure (§4.4.1) representing the innermost class that
            encloses the declaration of the current class.
             */
            u2 class_index;
            /*
             * If the current class is not immediately enclosed by a method or constructor,
            then the value of the method_index item must be zero.
            Otherwise, the value of the method_index item must be a valid index into
            the constant_pool table. The constant_pool entry at that index must be a
            CONSTANT_NameAndType_info structure (§4.4.6) representing the name and
            type of a method in the class referenced by the class_index attribute above.
             */
            u2 method_index;
        } enclosing_method;

        struct {
            u2 num;
            /*
             * Each value in the exception_index_table array must be a valid index into the constant_pool table.
             * The constant_pool entry at that index must be a CONSTANT_Class_info structure
             * representing a class type that this method is declared to throw.
             */
            u2 *exception_index_table;
        } exceptions;

        struct {
            u2 num;
            struct inner_class *classes;
        } inner_classes;

        /*
         * The LineNumberTable attribute is an optional variable-length attribute in the
        attributes table of a Code attribute (ยง4.7.3). It may be used by debuggers to
        determine which part of the code array corresponds to a given line number in the
        original source file.
        If multiple LineNumberTable attributes are present in the attributes table of a
        Code attribute, then they may appear in any order.
        There may be more than one LineNumberTable attribute per line of a source file
        in the attributes table of a Code attribute. That is, LineNumberTable attributes
        may together represent a given line of a source file, and need not be one-to-one
        with source lines.
         */
        struct {
            u2 num;
            struct line_number_table *tables;
        } line_number_table;

        /*
         * The LocalVariableTable attribute is an optional variable-length attribute in the
        attributes table of a Code attribute (ยง4.7.3). It may be used by debuggers to
        determine the value of a given local variable during the execution of a method.
        If multiple LocalVariableTable attributes are present in the attributes table of
        a Code attribute, then they may appear in any order.
        There may be no more than one LocalVariableTable attribute per local variable
        in the attributes table of a Code attribute.
         */
        struct {
            u2 num;
            struct local_variable_table *tables;
        } local_variable_table;

        /*
         *The LocalVariableTypeTable attribute is an optional variable-length attribute in
        the attributes table of a Code attribute (ยง4.7.3). It may be used by debuggers to
        determine the value of a given local variable during the execution of a method.
        If multiple LocalVariableTypeTable attributes are present in the attributes
        table of a given Code attribute, then they may appear in any order.
        There may be no more than one LocalVariableTypeTable attribute per local
        variable in the attributes table of a Code attribute.
        The LocalVariableTypeTable attribute differs from the LocalVariableTable
        attribute (ยง4.7.13) in that it provides signature information rather than descriptor
        information. This difference is only significant for variables whose type uses a type variable
        or parameterized type. Such variables will appear in both tables, while variables of other
        types will appear only in LocalVariableTable.
         */
        struct {
            u2 num;
            struct local_variable_type_table *tables;
        } local_variable_type_table;

        /*
         * The MethodParameters attribute is a variable-length attribute in the attributes
         * table of a method_info structure . A MethodParameters attribute records
         * information about the formal parameters of a method, such as their names.
         * There may be at most one MethodParameters attribute in the attributes table of a method_info structure.
         */
        struct {
            u1 num; // 这里确实是 u1, 不是 u2
            struct parameter *parameters;
        } method_parameters;

        /*
         * The RuntimeVisibleAnnotations attribute records run-time visible
         * annotations on the declaration of the corresponding class, field, or method.
         * The Java Virtual Machine must make these annotations available so they can be
         * returned by the appropriate reflective APIs.
         *
         * The RuntimeInvisibleAnnotations attribute records run-time
         * invisible annotations on the declaration of the corresponding class, method, or field.
         * There may be at most one RuntimeInvisibleAnnotations attribute in the
         * attributes table of a ClassFile, field_info, or method_info structure.

         * The RuntimeInvisibleAnnotations attribute is similar to the
         * RuntimeVisibleAnnotations attribute (ยง4.7.16), except that the annotations
         * represented by a RuntimeInvisibleAnnotations attribute must not be made available
         * for return by reflective APIs, unless the Java Virtual Machine has been instructed to retain
         * these annotations via some implementation-specific mechanism such as a command line
         * flag. In the absence of such instructions, the Java Virtual Machine ignores this attribute.
         */
        struct {
            u2 num;
            struct annotation *annotations;
        } runtime_annotations, runtime_visible_annotations, runtime_invisible_annotations;

        /*
         * The RuntimeVisibleParameterAnnotations attribute records run-time visible
         * annotations on the declarations of formal parameters of the corresponding method.
         * The Java Virtual Machine must make these annotations available so they can be
         * returned by the appropriate reflective APIs.

         * There may be at most one RuntimeVisibleParameterAnnotations attribute in
         * the attributes table of a method_info structure.

         * The RuntimeInvisibleParameterAnnotations attribute records run-time invisible
         * annotations on the declarations of formal parameters of the corresponding method.

         * There may be at most one RuntimeInvisibleParameterAnnotations attribute in
         * the attributes table of a method_info structure.

         * The RuntimeInvisibleParameterAnnotations attribute is similar to the
         * RuntimeVisibleParameterAnnotations attribute (ยง4.7.18), except that the
         * annotations represented by a RuntimeInvisibleParameterAnnotations attribute
         * must not be made available for return by reflective APIs, unless the Java Virtual Machine
         * has specifically been instructed to retain these annotations via some implementation-specific
         * mechanism such as a command line flag. In the absence of such instructions, the
         * Java Virtual Machine ignores this attribute.
         */
        struct {
            u2 num;
            struct parameter_annotation *parameter_annotations;
        } runtime_parameter_annotations, runtime_visible_parameter_annotations, runtime_invisible_parameter_annotations;

        /*
         * The Signature attribute is a fixed-length attribute in the attributes table
        of a ClassFile, field_info, or method_info structure (§4.1, §4.5, §4.6). A
        Signature attribute records a signature (§4.7.9.1) for a class, interface, constructor,
        method, or field whose declaration in the Java programming language uses type
        variables or parameterized types. See The Java Language Specification, Java SE
        8 Edition for details about these types.
         */
        /*
             * the value of the signature_index item must be a valid index into the
        constant_pool table. the constant_pool entry at that index must be a
        constant_utf8_info structure (§4.4.7) representing a class signature if this
        signature attribute is an attribute of a classfile structure; a method
        signature if this signature attribute is an attribute of a method_info structure;
        or a field signature otherwise.
             */
        u2 signature_index; // signature_attribute

        /*
         * The SourceDebugExtension attribute is an optional attribute in the attributes
        table of a ClassFile structure (ยง4.1).
        There may be at most one SourceDebugExtension attribute in the attributes
        table of a ClassFile structure.
         */
        /*
            * The debug_extension array holds extended debugging information which has
       no semantic effect on the Java Virtual Machine. The information is represented
       using a modified UTF-8 string (ยง4.4.7) with no terminating zero byte.

       Note that the debug_extension array may denote a string longer than that which can be
       represented with an instance of class String.
            */
        u1 *source_debug_extension; // source_debug_extension_attribute

        u2 source_file_index; // source_file_attribute. 常量池索引， 指向CONSTANT_Utf8_info常量。


        /*
         * The StackMapTable attribute is a variable-length attribute
         * in the attributes table of a Code attribute.
         * A StackMapTable attribute is used during the process of verification by type checking
         */
        struct {
            u2 number_of_entries;
//          stack_map_frame entries[number_of_entries];   // todo
        } stack_map_table;
    } u;
};

void read_annotation(struct bytecode_reader *reader, struct annotation *a);
void read_element_value(struct bytecode_reader *reader, struct element_value *ev);

#endif //JVM_ATTRIBUTE_H
