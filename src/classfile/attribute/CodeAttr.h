/*
 * Author: Jia Yang
 */

#ifndef JVM_CODEATTR_H
#define JVM_CODEATTR_H

#include "Attribute.h"
#include "../Constant.h"

struct CodeAttr: public Attribute {
    u2 maxStack;
    u2 maxLocals;

    u4 codeLength;
    u1 *code; // [code_length];
    /*
     * todo
     * code[]数组给出了实现当前方法的Java虚拟机字节码。
     * code[]数组以按字节寻址的方式读入机器内存，
     * 如果code[]数组的第一个字节是按以4字节边界对齐的话，
     * 那么tableswitch和lookupswitch指令中所有涉及到的32位偏移量也都是按4字节长度对齐的
     * （关于code[]数组边界对齐对字节码的影响，请参考相关的指令描述）。
     * 本规范对关于code[]数组内容的详细约束有很多，将在后面单独章节（§4.9）中列出。
     */

    u2 exceptionTableLength;
    /*
     * exception_table[]数组的每个成员表示code[]数组中的一个异常处理器（Exception Handler）。
     * exception_table[]数组中，异常处理器顺序是有意义的（不能随意更改），详细内容见2.10节。
     */
    struct ExceptionTable {
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
        u2 startPc;
        u2 endPc;
        /*
         * The value of the handler_pc item indicates the start of the exception
handler. The value of the item must be a valid index into the code array
and must be the index of the opcode of an instruction.
         */
        u2 handlerPc;
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
        u2 catchType;
    }; // [exception_table_length];
    ExceptionTable *exceptionTables;

    u2 attributesCount;
    Attribute **attributes; // [attributes_count];

    CodeAttr(BytecodeReader &reader, Constant **constantPool);
};
#endif //JVM_CODEATTR_H
