/*
 * Author: Jia Yang
 */

#ifndef JVM_CONSTANT_H
#define JVM_CONSTANT_H

#include "../jtypes.h"

#define INVALID_CONSTANT                0 // invalid constant
#define UTF8_CONSTANT					1
#define INTEGER_CONSTANT                3
#define FLOAT_CONSTANT					4
#define LONG_CONSTANT					5
#define DOUBLE_CONSTANT					6
#define CLASS_CONSTANT					7
#define STRING_CONSTANT					8
#define FIELD_REF_CONSTANT				9
#define METHOD_REF_CONSTANT             10
#define INTERFACE_METHOD_REF_CONSTANT   11
#define NAME_AND_TYPE_CONSTANT          12
#define METHOD_HANDLE_CONSTANT          15
#define METHOD_TYPE_CONSTANT            16
#define INVOKE_DYNAMIC_CONSTANT         18
#define PLACEHOLDER_CONSTANT            INT8_MAX // long 和 double 的占位符，数值不同于以上定义的常量即可。



// Bytecode Behaviors and Method Descriptors for Method Handles
//      Description              | Kind   | Interpretation                           | Method descriptor
#define REF_KIND_GET_FIELD          1    // getfield C.f:T                           | (C)T
#define REF_KIND_GET_STATIC         2    // getstatic C.f:T                          | ()T
#define REF_KIND_PUT_FIELD          3    // putfield C.f:T                           | (C,T)V
#define REF_KIND_PUT_STATIC         4    // putstatic C.f:T                          | (T)V
#define REF_KIND_INVOKE_VIRTUAL     5    // invokevirtual C.m:(A*)T                  | (C,A*)T
#define REF_KIND_INVOKE_STATIC      6    // invokestatic C.m:(A*)T                   | (A*)T
#define REF_KIND_INVOKE_SPECIAL     7    // invokespecial C.m:(A*)T                  | (C,A*)T
#define REF_KIND_NEW_INVOKE_SPECIAL 8    // new C; dup; invokespecial C.<init>:(A*)V | (A*)C
#define REF_KIND_INVOKE_INTERFACE   9    // invokeinterface C.m:(A*)T                | (C,A*)T

/*
 * 常量池中的每一项都具备相同的格式特征：
 * 第一个字节作为类型标记用于识别该项是哪种类型的常量，称为“tag byte”。
 * 常量池的索引范围是 1 至 constant_pool_count − 1
 */
struct constant {
    u1 tag;

    union {
        u2 class_name_index; // class constant

        struct {
            u2 class_index;
            u2 name_and_type_index;
        } ref_constant, field_ref_constant, method_ref_constant, interface_method_ref_constant;

        u2 string_index;    // string constant

        u1 bytes4[4]; // integer constant, float constant;
        u1 bytes8[8]; // long constant, double constant

        struct {
            u2 name_index;
            u2 descriptor_index;
        } name_and_type_constant;

        struct {
            u2 length;
            u1 *bytes;
        } utf8_constant;

        struct {
            // reference_kind项的值必须在1至9之间（包括1和9），它决定了方法句柄的类型。
            // 方法句柄类型的值表示方法句柄的字节码行为。
            u1 reference_kind;
            /*
             * From jvms11:
             * The value of the reference_index item must be a valid index into the
             * constant_pool table. The constant_pool entry at that index must be as
             * follows:
             * 1. If the value of the reference_kind item is 1 (REF_getField), 2
             *    (REF_getStatic), 3 (REF_putField), or 4 (REF_putStatic), then the
             *    constant_pool entry at that index must be a CONSTANT_Fieldref_info
             *    structure representing a field for which a method handle is to be created.
             * 2. If the value of the reference_kind item is 5 (REF_invokeVirtual) or 8
             *    (REF_newInvokeSpecial), then the constant_pool entry at that index must
             *    be a CONSTANT_Methodref_info structure representing a class's
             *    method or constructor for which a method handle is to be created.
             * 3. If the value of the reference_kind item is 6 (REF_invokeStatic)
             *    or 7 (REF_invokeSpecial), then if the class file version number
             *    is less than 52.0, the constant_pool entry at that index must be
             *    a CONSTANT_Methodref_info structure representing a class's method
             *    for which a method handle is to be created; if the class file
             *    version number is 52.0 or above, the constant_pool entry at that
             *    index must be either a CONSTANT_Methodref_info structure or a
             *    CONSTANT_InterfaceMethodref_info structure representing a
             *    class's or interface's method for which a method handle is to be created.
             * 4. If the value of the reference_kind item is 9 (REF_invokeInterface),
             *    then the constant_pool entry at that index must be a
             *    CONSTANT_InterfaceMethodref_info structure representing an interface's
             *    method for which a method handle is to be created.
             *
             * If the value of the reference_kind item is 5 (REF_invokeVirtual), 6
             * (REF_invokeStatic), 7 (REF_invokeSpecial), or 9 (REF_invokeInterface),
             * the name of the method represented by a CONSTANT_Methodref_info structure
             * or a CONSTANT_InterfaceMethodref_info structure must not be <init> or <clinit>.
             * If the value is 8 (REF_newInvokeSpecial), the name of the method represented
             * by a CONSTANT_Methodref_info structure must be <init>.
             */
            u2 reference_index;
        } method_handle_constant;

        u2 method_descriptor_index; // method_type_constant

        /*
         * 用于表示invokedynamic指令所使用到的
         * 引导方法（Bootstrap Method）、
         * 引导方法使用到动态调用名称（Dynamic Invocation Name）、
         * 参数和请求返回类型、
         * 以及可以选择性的附加被称为静态参数（Static Arguments）的常量序列。
         */
        struct {
            // bootstrap_method_attr_index项的值必须是对当前Class文件中引导方法表的bootstrap_methods[]数组的有效索引。
            u2 bootstrap_method_attr_index;
            u2 name_and_type_index;
        } invoke_dynamic_constant;
    } u;
};

#endif //JVM_CONSTANT_H
