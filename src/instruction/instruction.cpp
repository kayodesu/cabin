/*
 * Author: Jia Yang
 */

#include "comparisons.h"
#include "constants.h"
#include "controls.h"
#include "loads.h"
#include "maths.h"
#include "references.h"
#include "stacks.h"
#include "stores.h"
#include "extended.h"

#include "../native/registry.h"

using namespace std;

static void notUsed(StackFrame *frame) {
    jvmAbort("This instruction isn't used.\n");
}

static void debuggerUsed(StackFrame *frame) {
    jprintf("debuggerUsed instructions\n");
    // todo
}

template <typename T> static T __add(T v1, T v2) { return v1 + v2; }
template <typename T> static T __sub(T v1, T v2) { return v1 - v2; }
template <typename T> static T __mul(T v1, T v2) { return v1 * v2; }
template <typename T> static T __div(T v1, T v2) { return v1 / v2; }
template <typename T> static T __and(T v1, T v2) { return v1 & v2; }
template <typename T> static T __or(T v1, T v2) { return v1 | v2; }
template <typename T> static T __xor(T v1, T v2) { return v1 ^ v2; }

template <typename T> static bool __eq(T v1, T v2) { return v1 == v2; }
template <typename T> static bool __ne(T v1, T v2) { return v1 != v2; }
template <typename T> static bool __lt(T v1, T v2) { return v1 < v2; }
template <typename T> static bool __ge(T v1, T v2) { return v1 >= v2; }
template <typename T> static bool __gt(T v1, T v2) { return v1 > v2; }
template <typename T> static bool __le(T v1, T v2) { return v1 <= v2; }


#define OS_POP_INT &OperandStack::popInt

// 指令集
tuple<int, const char *, void (*)(StackFrame *)> instructions[] = {
        make_tuple(0x00, "nop", [](StackFrame *){}),

        make_tuple(0x01, "aconst_null", [](StackFrame *frame){ frame->operandStack.push(nullptr); }),
		
        make_tuple(0x02, "iconst_ml", [](StackFrame *frame){ frame->operandStack.push((jint) -1); }),
        make_tuple(0x03, "iconst_0", [](StackFrame *frame){ frame->operandStack.push((jint) 0); }),
        make_tuple(0x04, "iconst_1", [](StackFrame *frame){ frame->operandStack.push((jint) 1); }),
        make_tuple(0x05, "iconst_2", [](StackFrame *frame){ frame->operandStack.push((jint) 2); }),
        make_tuple(0x06, "iconst_3", [](StackFrame *frame){ frame->operandStack.push((jint) 3); }),
        make_tuple(0x07, "iconst_4", [](StackFrame *frame){ frame->operandStack.push((jint) 4); }),
        make_tuple(0x08, "iconst_5", [](StackFrame *frame){ frame->operandStack.push((jint) 5); }),

        make_tuple(0x09, "lconst_0", [](StackFrame *frame){ frame->operandStack.push((jlong) 0); }),
        make_tuple(0x0a, "lconst_1", [](StackFrame *frame){ frame->operandStack.push((jlong) 1); }),

        make_tuple(0x0b, "fconst_0", [](StackFrame *frame){ frame->operandStack.push((jfloat) 0); }),
        make_tuple(0x0c, "fconst_1", [](StackFrame *frame){ frame->operandStack.push((jfloat) 1); }),
        make_tuple(0x0d, "fconst_2", [](StackFrame *frame){ frame->operandStack.push((jfloat) 2); }),

        make_tuple(0x0e, "dconst_0", [](StackFrame *frame){ frame->operandStack.push((jdouble) 0); }),
        make_tuple(0x0f, "dconst_1", [](StackFrame *frame){ frame->operandStack.push((jdouble) 1); }),

        make_tuple(0x10, "bipush", [](StackFrame *frame){
            frame->operandStack.push((jint) frame->reader->reads1());
        }),  // Byte Integer push

        make_tuple(0x11, "sipush", [](StackFrame *frame){
            frame->operandStack.push((jint) frame->reader->reads2()); }
        ),  // Short Integer push

        make_tuple(0x12, "ldc", __ldc),  // LoaD constant
        make_tuple(0x13, "ldc_w", __ldc<2>),
        make_tuple(0x14, "ldc2_w", __ldc<2, true>),

        // Loads
        make_tuple(0x15, "iload", __tload<PRIMITIVE_INT>),
        make_tuple(0x16, "lload", __tload<PRIMITIVE_LONG>),
        make_tuple(0x17, "fload", __tload<PRIMITIVE_FLOAT>),
        make_tuple(0x18, "dload", __tload<PRIMITIVE_DOUBLE>),
        make_tuple(0x19, "aload", __tload<REFERENCE>),

        make_tuple(0x1a, "iload_0", __tload<PRIMITIVE_INT, 0>),
        make_tuple(0x1b, "iload_1", __tload<PRIMITIVE_INT, 1>),
        make_tuple(0x1c, "iload_2", __tload<PRIMITIVE_INT, 2>),
        make_tuple(0x1d, "iload_3", __tload<PRIMITIVE_INT, 3>),

        make_tuple(0x1e, "lload_0", __tload<PRIMITIVE_LONG, 0>),
        make_tuple(0x1f, "lload_1", __tload<PRIMITIVE_LONG, 1>),
        make_tuple(0x20, "lload_2", __tload<PRIMITIVE_LONG, 2>),
        make_tuple(0x21, "lload_3", __tload<PRIMITIVE_LONG, 3>),

        make_tuple(0x22, "fload_0", __tload<PRIMITIVE_FLOAT, 0>),
        make_tuple(0x23, "fload_1", __tload<PRIMITIVE_FLOAT, 1>),
        make_tuple(0x24, "fload_2", __tload<PRIMITIVE_FLOAT, 2>),
        make_tuple(0x25, "fload_3", __tload<PRIMITIVE_FLOAT, 3>),

        make_tuple(0x26, "dload_0", __tload<PRIMITIVE_DOUBLE, 0>),
        make_tuple(0x27, "dload_1", __tload<PRIMITIVE_DOUBLE, 1>),
        make_tuple(0x28, "dload_2", __tload<PRIMITIVE_DOUBLE, 2>),
        make_tuple(0x29, "dload_3", __tload<PRIMITIVE_DOUBLE, 3>),

        make_tuple(0x2a, "aload_0", __tload<REFERENCE, 0>),
        make_tuple(0x2b, "aload_1", __tload<REFERENCE, 1>),
        make_tuple(0x2c, "aload_2", __tload<REFERENCE, 2>),
        make_tuple(0x2d, "aload_3", __tload<REFERENCE, 3>),

        make_tuple(0x2e, "iaload", __taload<&Jclass::isPrimitiveIntArray, jint>),
        make_tuple(0x2f, "laload", __taload<&Jclass::isPrimitiveLongArray, jlong>),
        make_tuple(0x30, "faload", __taload<&Jclass::isPrimitiveFloatArray, jfloat>),
        make_tuple(0x31, "daload", __taload<&Jclass::isPrimitiveDoubleArray, jdouble>),
        make_tuple(0x32, "aaload", __taload<&Jclass::isOneDimensionReferenceArray, jreference>), // todo 这里的isOneDimensionReferenceArray貌似没有考虑多维的情况
        make_tuple(0x33, "baload", __taload<&Jclass::isPrimitiveBoolOrByteArray, jbyte>),
        make_tuple(0x34, "caload", __taload<&Jclass::isPrimitiveCharArray, jchar>),
        make_tuple(0x35, "saload", __taload<&Jclass::isPrimitiveShortArray, jshort>),

        // Stores
        make_tuple(0x36, "istore", __tstore<PRIMITIVE_INT>),
        make_tuple(0x37, "lstore", __tstore<PRIMITIVE_LONG>),
        make_tuple(0x38, "fstore", __tstore<PRIMITIVE_FLOAT>),
        make_tuple(0x39, "dstore", __tstore<PRIMITIVE_DOUBLE>),
        make_tuple(0x3a, "astore", __tstore<REFERENCE>),

        make_tuple(0x3b, "istore_0", __tstore<PRIMITIVE_INT, 0>),
        make_tuple(0x3c, "istore_1", __tstore<PRIMITIVE_INT, 1>),
        make_tuple(0x3d, "istore_2", __tstore<PRIMITIVE_INT, 2>),
        make_tuple(0x3e, "istore_3", __tstore<PRIMITIVE_INT, 3>),

        make_tuple(0x3f, "lstore_0", __tstore<PRIMITIVE_LONG, 0>),
        make_tuple(0x40, "lstore_1", __tstore<PRIMITIVE_LONG, 1>),
        make_tuple(0x41, "lstore_2", __tstore<PRIMITIVE_LONG, 2>),
        make_tuple(0x42, "lstore_3", __tstore<PRIMITIVE_LONG, 3>),

        make_tuple(0x43, "fstore_0", __tstore<PRIMITIVE_FLOAT, 0>),
        make_tuple(0x44, "fstore_1", __tstore<PRIMITIVE_FLOAT, 1>),
        make_tuple(0x45, "fstore_2", __tstore<PRIMITIVE_FLOAT, 2>),
        make_tuple(0x46, "fstore_3", __tstore<PRIMITIVE_FLOAT, 3>),

        make_tuple(0x47, "dstore_0", __tstore<PRIMITIVE_DOUBLE, 0>),
        make_tuple(0x48, "dstore_1", __tstore<PRIMITIVE_DOUBLE, 1>),
        make_tuple(0x49, "dstore_2", __tstore<PRIMITIVE_DOUBLE, 2>),
        make_tuple(0x4a, "dstore_3", __tstore<PRIMITIVE_DOUBLE, 3>),

        make_tuple(0x4b, "astore_0", __tstore<REFERENCE, 0>),
        make_tuple(0x4c, "astore_1", __tstore<REFERENCE, 1>),
        make_tuple(0x4d, "astore_2", __tstore<REFERENCE, 2>),
        make_tuple(0x4e, "astore_3", __tstore<REFERENCE, 3>),

        make_tuple(0x4f, "iastore", __tastore<jint, OS_POP_INT, &Jclass::isPrimitiveIntArray, jint>),
        make_tuple(0x50, "lastore", __tastore<jlong, &OperandStack::popLong, &Jclass::isPrimitiveLongArray, jlong>),
        make_tuple(0x51, "fastore", __tastore<jfloat, &OperandStack::popFloat, &Jclass::isPrimitiveFloatArray, jfloat>),
        make_tuple(0x52, "dastore", __tastore<jdouble, &OperandStack::popDouble, &Jclass::isPrimitiveDoubleArray, jdouble>),
        make_tuple(0x53, "aastore", __tastore<jreference, &OperandStack::popReference, &Jclass::isOneDimensionReferenceArray, jreference>),
        make_tuple(0x54, "bastore", __tastore<jint, &OperandStack::popInt, &Jclass::isPrimitiveBoolOrByteArray, jbyte>),
        make_tuple(0x55, "castore", __tastore<jint, &OperandStack::popInt, &Jclass::isPrimitiveCharArray, jchar>),
        make_tuple(0x56, "sastore", __tastore<jint, &OperandStack::popInt, &Jclass::isPrimitiveShortArray, jshort>),

        // Stack
        make_tuple(0x57, "pop",     pop),
        make_tuple(0x58, "pop2",    pop2),
        make_tuple(0x59, "dup",     dup),
        make_tuple(0x5a, "dup_x1",  dupX1),
        make_tuple(0x5b, "dup_x2",  dupX2),
        make_tuple(0x5c, "dup2",    dup2),
        make_tuple(0x5d, "dup2_x1", dup2X1),
        make_tuple(0x5e, "dup2_x2", dup2X2),
        make_tuple(0x5f, "swap",    __swap),

        // Math
        make_tuple(0x60, "iadd", mathcal<jint, &OperandStack::popInt, __add>),
        make_tuple(0x61, "ladd", mathcal<jlong, &OperandStack::popLong, __add>),
        make_tuple(0x62, "fadd", mathcal<jfloat, &OperandStack::popFloat, __add>),
        make_tuple(0x63, "dadd", mathcal<jdouble, &OperandStack::popDouble, __add>),

        make_tuple(0x64, "isub", mathcal<jint, &OperandStack::popInt, __sub>),
        make_tuple(0x65, "lsub", mathcal<jlong, &OperandStack::popLong, __sub>),
        make_tuple(0x66, "fsub", mathcal<jfloat, &OperandStack::popFloat, __sub>),
        make_tuple(0x67, "dsub", mathcal<jdouble, &OperandStack::popDouble, __sub>),

        make_tuple(0x68, "imul", mathcal<jint, &OperandStack::popInt, __mul>),
        make_tuple(0x69, "lmul", mathcal<jlong, &OperandStack::popLong, __mul>),
        make_tuple(0x6a, "fmul", mathcal<jfloat, &OperandStack::popFloat, __mul>),
        make_tuple(0x6b, "dmul", mathcal<jdouble, &OperandStack::popDouble, __mul>),

        make_tuple(0x6c, "idiv", mathcal<jint, &OperandStack::popInt, __div>),
        make_tuple(0x6d, "ldiv", mathcal<jlong, &OperandStack::popLong, __div>),
        make_tuple(0x6e, "fdiv", mathcal<jfloat, &OperandStack::popFloat, __div>),
        make_tuple(0x6f, "ddiv", mathcal<jdouble, &OperandStack::popDouble, __div>),

        make_tuple(0x70, "irem", __irem),
        make_tuple(0x71, "lrem", __lrem),
        make_tuple(0x72, "frem", __frem),
        make_tuple(0x73, "drem", __drem_), // todo

        make_tuple(0x74, "ineg", __neg<PRIMITIVE_INT>),
        make_tuple(0x75, "lneg", __neg<PRIMITIVE_LONG>),
        make_tuple(0x76, "fneg", __neg<PRIMITIVE_FLOAT>),
        make_tuple(0x77, "dneg", __neg<PRIMITIVE_DOUBLE>),

        make_tuple(0x78, "ishl", [](StackFrame *frame) {
            auto t2 = frame->operandStack.popInt();
            auto t1 = frame->operandStack.popInt();
            frame->operandStack.push((jint)(t1 << (t2 & 0x1f))); // t2低5位表示位移距离，位移距离实际上被限制在0到31之间。
        }),

        make_tuple(0x79, "lshl", [](StackFrame *frame) {
            auto t2 = frame->operandStack.popInt();
            auto t1 = frame->operandStack.popLong();
            frame->operandStack.push((jlong)(t1 << (t2 & 0x3f))); // t2低6位表示位移距离，位移距离实际上被限制在0到63之间。
        }),

        make_tuple(0x7a, "ishr", __shr<jint, &OperandStack::popInt, true>),
        make_tuple(0x7b, "lshr", __shr<jlong, &OperandStack::popLong, true>),

        make_tuple(0x7c, "iushr", __shr<jint, &OperandStack::popInt, false>),
        make_tuple(0x7d, "lushr", __shr<jlong, &OperandStack::popLong, false>),

        make_tuple(0x7e, "iand", mathcal<jint, &OperandStack::popInt, __and>),
        make_tuple(0x7f, "land", mathcal<jlong, &OperandStack::popLong, __and>),

        make_tuple(0x80, "ior", mathcal<jint, &OperandStack::popInt, __or>),
        make_tuple(0x81, "lor", mathcal<jlong, &OperandStack::popLong, __or>),

        make_tuple(0x82, "ixor", mathcal<jint, &OperandStack::popInt, __xor>),
        make_tuple(0x83, "lxor", mathcal<jlong, &OperandStack::popLong, __xor>),

        make_tuple(0x84, "iinc", __iinc),

        // Conversions // todo 指令实现完全有问题！！！！！ 只改变数据类型不行，还需进行数据的转换！！！！！！！！！！！！！！！！
        make_tuple(0x85, "i2l", [](StackFrame *frame){ frame->operandStack.peek().x2x(PRIMITIVE_INT, PRIMITIVE_LONG); }),
        make_tuple(0x86, "i2f", [](StackFrame *frame){ frame->operandStack.peek().x2x(PRIMITIVE_INT, PRIMITIVE_FLOAT); }),
        make_tuple(0x87, "i2d", [](StackFrame *frame){ frame->operandStack.peek().x2x(PRIMITIVE_INT, PRIMITIVE_DOUBLE); }),

        make_tuple(0x88, "l2i", [](StackFrame *frame){ frame->operandStack.peek().x2x(PRIMITIVE_LONG, PRIMITIVE_INT); }),
        make_tuple(0x89, "l2f", [](StackFrame *frame){ frame->operandStack.peek().x2x(PRIMITIVE_LONG, PRIMITIVE_FLOAT); }),
        make_tuple(0x8a, "l2d", [](StackFrame *frame){ frame->operandStack.peek().x2x(PRIMITIVE_LONG, PRIMITIVE_DOUBLE); }), 

        make_tuple(0x8b, "f2i", [](StackFrame *frame){ frame->operandStack.peek().x2x(PRIMITIVE_FLOAT, PRIMITIVE_INT); }),
        make_tuple(0x8c, "f2l", [](StackFrame *frame){ frame->operandStack.peek().x2x(PRIMITIVE_FLOAT, PRIMITIVE_LONG); }),
        make_tuple(0x8d, "f2d", [](StackFrame *frame){ frame->operandStack.peek().x2x(PRIMITIVE_FLOAT, PRIMITIVE_DOUBLE); }),

        make_tuple(0x8e, "d2i", [](StackFrame *frame){ frame->operandStack.peek().x2x(PRIMITIVE_DOUBLE, PRIMITIVE_INT); }),
        make_tuple(0x8f, "d2l", [](StackFrame *frame){ frame->operandStack.peek().x2x(PRIMITIVE_DOUBLE, PRIMITIVE_LONG); }),
        make_tuple(0x90, "d2f", [](StackFrame *frame){ frame->operandStack.peek().x2x(PRIMITIVE_DOUBLE, PRIMITIVE_FLOAT); }),

        make_tuple(0x91, "i2b", nullptr),  // todo byte or boolean?????
        make_tuple(0x92, "i2c", [](StackFrame *frame){ frame->operandStack.peek().x2x(PRIMITIVE_INT, PRIMITIVE_CHAR); }),
        make_tuple(0x93, "i2s", [](StackFrame *frame){ frame->operandStack.peek().x2x(PRIMITIVE_INT, PRIMITIVE_SHORT); }),

        // Comparisons
        make_tuple(0x94, "lcmp", __cmp<jlong, &OperandStack::popLong>),
        make_tuple(0x95, "fcmpl", __cmp<jfloat, &OperandStack::popFloat, -1>),
        make_tuple(0x96, "fcmpg", __cmp<jfloat, &OperandStack::popFloat, 1>),
        make_tuple(0x97, "dcmpl", __cmp<jdouble, &OperandStack::popDouble, -1>),
        make_tuple(0x98, "dcmpg", __cmp<jdouble, &OperandStack::popDouble, 1>),

        make_tuple(0x99, "ifeq", ifCmpCond<jint, &OperandStack::popInt, __eq, true>),
        make_tuple(0x9a, "ifne", ifCmpCond<jint, &OperandStack::popInt, __ne, true>),
        make_tuple(0x9b, "iflt", ifCmpCond<jint, &OperandStack::popInt, __lt, true>),
        make_tuple(0x9c, "ifge", ifCmpCond<jint, &OperandStack::popInt, __ge, true>),
        make_tuple(0x9d, "ifgt", ifCmpCond<jint, &OperandStack::popInt, __gt, true>),
        make_tuple(0x9e, "ifle", ifCmpCond<jint, &OperandStack::popInt, __le, true>),

        make_tuple(0x9f, "if_icmpeq", ifCmpCond<jint, &OperandStack::popInt, __eq>),
        make_tuple(0xa0, "if_icmpne", ifCmpCond<jint, &OperandStack::popInt, __ne>),
        make_tuple(0xa1, "if_icmplt", ifCmpCond<jint, &OperandStack::popInt, __lt>),
        make_tuple(0xa2, "if_icmpge", ifCmpCond<jint, &OperandStack::popInt, __ge>),
        make_tuple(0xa3, "if_icmpgt", ifCmpCond<jint, &OperandStack::popInt, __gt>),
        make_tuple(0xa4, "if_icmple", ifCmpCond<jint, &OperandStack::popInt, __le>),

        make_tuple(0xa5, "if_acmpeq", ifCmpCond<jreference, &OperandStack::popReference, __eq>),
        make_tuple(0xa6, "if_acmpne", ifCmpCond<jreference, &OperandStack::popReference, __ne>),

        // Control
        make_tuple(0xa7, "goto", __goto),

        make_tuple(0xa8, "jsr", nullptr),
        make_tuple(0xa9, "ret", nullptr),
        make_tuple(0xaa, "tableswitch", nullptr),
        make_tuple(0xab, "lookupswitch", nullptr),

        make_tuple(0xac, "ireturn", __return<jint, &OperandStack::popInt>),
        make_tuple(0xad, "lreturn", __return<jlong, &OperandStack::popLong>),
        make_tuple(0xae, "freturn", __return<jfloat, &OperandStack::popFloat>),
        make_tuple(0xaf, "dreturn", __return<jdouble, &OperandStack::popDouble>),

        // todo areturn: objectref必须是一个reference类型的数据，
        // todo 并且必须指向一个类型与当前方法的方法描述符（§4.3.3）中的返回值相匹配（JLS §5.2）的对象。
        make_tuple(0xb0, "areturn", __return<jreference, &OperandStack::popReference>),

        make_tuple(0xb1, "return", __return),

        // References
        make_tuple(0xb2, "getstatic", getstatic),
        make_tuple(0xb3, "putstatic", putstatic),
        make_tuple(0xb4, "getfield", getfield),
        make_tuple(0xb5, "putfield", putfield),

        make_tuple(0xb6, "invokevirtual", invokevirtual),
        make_tuple(0xb7, "invokespecial", invokespecial),
        make_tuple(0xb8, "invokestatic", invokestatic),
        make_tuple(0xb9, "invokeinterface", invokeinterface),
        make_tuple(0xba, "invokedynamic", nullptr),

        make_tuple(0xbb, "new", __new),

        make_tuple(0xbc, "newarray", newarray),
        make_tuple(0xbd, "anewarray", anewarray),
        make_tuple(0xbe, "arraylength", arraylength),

        make_tuple(0xbf, "athrow", athrow),
        make_tuple(0xc0, "checkcast", checkcast),
        make_tuple(0xc1, "instanceof", instanceof),
        make_tuple(0xc2, "monitorenter", monitorenter),
        make_tuple(0xc3, "monitorexit", monitorexit),

        // Extended
        make_tuple(0xc4, "wide", nullptr),
        make_tuple(0xc5, "multianewarray", multianewarray),
        make_tuple(0xc6, "ifnull", ifnull),
        make_tuple(0xc7, "ifnonnull", ifnonnull),
        make_tuple(0xc8, "goto_w", nullptr),
        make_tuple(0xc9, "jsr_w", nullptr),

        // Reserved
        make_tuple(0xca, "breakpoint", debuggerUsed),  // debugger used instruction

        make_tuple(0xcb, "Not Used", notUsed),
        make_tuple(0xcc, "Not Used", notUsed),
        make_tuple(0xcd, "Not Used", notUsed),
        make_tuple(0xce, "Not Used", notUsed),
        make_tuple(0xcf, "Not Used", notUsed),
        make_tuple(0xd0, "Not Used", notUsed),
        make_tuple(0xd1, "Not Used", notUsed),
        make_tuple(0xd2, "Not Used", notUsed),
        make_tuple(0xd3, "Not Used", notUsed),
        make_tuple(0xd4, "Not Used", notUsed),
        make_tuple(0xd5, "Not Used", notUsed),
        make_tuple(0xd6, "Not Used", notUsed),
        make_tuple(0xd7, "Not Used", notUsed),
        make_tuple(0xd8, "Not Used", notUsed),
        make_tuple(0xd9, "Not Used", notUsed),
        make_tuple(0xda, "Not Used", notUsed),
        make_tuple(0xdb, "Not Used", notUsed),
        make_tuple(0xdc, "Not Used", notUsed),
        make_tuple(0xdd, "Not Used", notUsed),
        make_tuple(0xde, "Not Used", notUsed),
        make_tuple(0xdf, "Not Used", notUsed),
        make_tuple(0xe0, "Not Used", notUsed),
        make_tuple(0xe1, "Not Used", notUsed),
        make_tuple(0xe2, "Not Used", notUsed),
        make_tuple(0xe3, "Not Used", notUsed),
        make_tuple(0xe4, "Not Used", notUsed),
        make_tuple(0xe5, "Not Used", notUsed),
        make_tuple(0xe6, "Not Used", notUsed),
        make_tuple(0xe7, "Not Used", notUsed),
        make_tuple(0xe8, "Not Used", notUsed),
        make_tuple(0xe9, "Not Used", notUsed),
        make_tuple(0xea, "Not Used", notUsed),
        make_tuple(0xeb, "Not Used", notUsed),
        make_tuple(0xec, "Not Used", notUsed),
        make_tuple(0xed, "Not Used", notUsed),
        make_tuple(0xee, "Not Used", notUsed),
        make_tuple(0xef, "Not Used", notUsed),
        make_tuple(0xf0, "Not Used", notUsed),
        make_tuple(0xf1, "Not Used", notUsed),
        make_tuple(0xf2, "Not Used", notUsed),
        make_tuple(0xf3, "Not Used", notUsed),
        make_tuple(0xf4, "Not Used", notUsed),
        make_tuple(0xf5, "Not Used", notUsed),
        make_tuple(0xf6, "Not Used", notUsed),
        make_tuple(0xf7, "Not Used", notUsed),
        make_tuple(0xf8, "Not Used", notUsed),
        make_tuple(0xf9, "Not Used", notUsed),
        make_tuple(0xfa, "Not Used", notUsed),
        make_tuple(0xfb, "Not Used", notUsed),
        make_tuple(0xfc, "Not Used", notUsed),
        make_tuple(0xfd, "Not Used", notUsed),

        make_tuple(0xfe, "impdep1", [](StackFrame *frame) {
            auto nativeMethod = findNativeMethod(
                    frame->method->jclass->className, frame->method->name, frame->method->descriptor);

            if (nativeMethod == nullptr) {
                // todo
                jvmAbort("error\n");
            }
            nativeMethod(frame);
        }),  // jvm used instruction

        make_tuple(0xff, "impdep2", notUsed),  // jvm used instruction, not used in this jvm.
};

int instructionCount = sizeof(instructions) / sizeof(*instructions);
