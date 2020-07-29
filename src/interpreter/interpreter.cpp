/*
 * Author: Yo Ka
 */

#include <iostream>
#include <cmath>
#include <ffi.h>
#include "../jvmstd.h"
#include "interpreter.h"
#include "../objects/invoke.h"
#include "../objects/object.h"
#include "../metadata/class.h"
#include "../metadata/method.h"
#include "../debug.h"
#include "../runtime/vm_thread.h"
#include "../runtime/frame.h"
#include "../objects/array_object.h"

using namespace std;
using namespace utf8;
using namespace slot;
using namespace method_type;
using namespace method_handles;

#if TRACE_INTERPRETER
// the mapping of instructions's code and name
static const char *instruction_names[] = {
        "nop",

        // Constants [0x01 ... 0x14]
        "aconst_null",
        "iconst_m1", "iconst_0", "iconst_1", "iconst_2", "iconst_3", "iconst_4", "iconst_5",
        "lconst_0", "lconst_1",
        "fconst_0", "fconst_1", "fconst_2",
        "dconst_0", "dconst_1",
        "bipush", "sipush",
        "ldc", "ldc_w", "ldc2_w",

        // Loads [0x15 ... 0x35]
        "iload", "lload", "fload", "dload", "aload",
        "iload_0", "iload_1", "iload_2", "iload_3",
        "lload_0", "lload_1", "lload_2", "lload_3",
        "fload_0", "fload_1", "fload_2", "fload_3",
        "dload_0", "dload_1", "dload_2", "dload_3",
        "aload_0", "aload_1", "aload_2", "aload_3",
        "iaload", "laload", "faload", "daload", "aaload", "baload", "caload", "saload",

        // Stores [0x36 ... 0x56]
        "istore", "lstore", "fstore", "dstore", "astore",
        "istore_0", "istore_1", "istore_2", "istore_3",
        "lstore_0", "lstore_1", "lstore_2", "lstore_3",
        "fstore_0", "fstore_1", "fstore_2", "fstore_3",
        "dstore_0", "dstore_1", "dstore_2", "dstore_3",
        "astore_0", "astore_1", "astore_2", "astore_3",
        "iastore", "lastore", "fastore", "dastore", "aastore", "bastore", "castore", "sastore",

        // Stack [0x57 ... 0x5f]
        "pop", "pop2", "dup", "dup_x1", "dup_x2", "dup2", "dup2_x1", "dup2_x2", "swap",

        // Math [0x60 ... 0x84]
        "iadd", "ladd", "fadd", "dadd",
        "isub", "lsub", "fsub", "dsub",
        "imul", "lmul", "fmul", "dmul",
        "idiv", "ldiv", "fdiv", "ddiv",
        "irem", "lrem", "frem", "drem",
        "ineg", "lneg", "fneg", "dneg",
        "ishl", "lshl", "ishr", "lshr", "iushr", "lushr",
        "iand", "land", "ior", "lor", "ixor", "lxor", "iinc",

        // Conversions [0x85 ... 0x93]
        "i2l", "i2f", "i2d",
        "l2i", "l2f", "l2d",
        "f2i", "f2l", "f2d",
        "d2i", "d2l", "d2f",
        "i2b", "i2c", "i2s",

        // Comparisons [0x94 ... 0xa6]
        "lcmp", "fcmpl", "fcmpg", "dcmpl", "dcmpg",
        "ifeq", "ifne", "iflt", "ifge", "ifgt", "ifle",
        "if_icmpeq", "if_icmpne", "if_icmplt", "if_icmpge", "if_icmpgt", "if_icmple",
        "if_acmpeq", "if_acmpne",

        // Control [0xa7 ... 0xb1]
        "goto", "jsr", "ret", "tableswitch", "lookupswitch",
        "ireturn", "lreturn", "freturn", "dreturn","areturn", "return",

        // References [0xb2 ... 0xc3]
        "getstatic", "putstatic", "getfield", "putfield",
        "invokevirtual", "invokespecial", "invokestatic", "invokeinterface", "invokedynamic",
        "new", "newarray", "anewarray", "arraylength",
        "athrow", "checkcast", "instanceof", "monitorenter", "monitorexit",

        // Extended [0xc4 ... 0xc9]
        "wide", "multianewarray", "ifnull", "ifnonnull", "goto_w", "jsr_w",

        // Reserved [0xca ... 0xff]
#undef U
#define U "unused"
        "breakpoint",
        U, U, U, U, U, // [0xcb ... 0xcf]
        U, U, U, U, U, U, U, U, // [0xd0 ... 0xd7]
        U, U, U, U, U, U, U, U, // [0xd8 ... 0xdf]
        U, U, U, U, U, U, U, U, // [0xe0 ... 0xe7]
        U, U, U, U, U, U, U, U, // [0xe8 ... 0xef]
        U, U, U, U, U, U, U, U, // [0xf0 ... 0xf7]
        U, U, U, U, U, U, // [0xf8 ... 0xfd]
        "invokenative", "impdep2"
#undef U        
};

#define TRACE PRINT_TRACE
#define PRINT_OPCODE TRACE("%d(0x%x), %s, pc = %d\n", opcode, opcode, instruction_names[opcode], (int)frame->reader.pc);
#else
#define TRACE(...)
#define PRINT_OPCODE
#endif

static void callJNIMethod(Frame *frame);
static bool checkcast(Class *s, Class *t);

/*
 * 执行当前线程栈顶的frame
 */
static slot_t *exec()
{    
    static void *handlers[] = {
        &&opc_nop, 
        
        // Constants [0x01 ... 0x14]
        &&opc_aconst_null, 
        &&opc_iconst_m1, &&opc_iconst_0, &&opc_iconst_1, &&opc_iconst_2, 
        &&opc_iconst_3, &&opc_iconst_4, &&opc_iconst_5, 
        &&opc_lconst_0, &&opc_lconst_1, 
        &&opc_fconst_0, &&opc_fconst_1, &&opc_fconst_2, 
        &&opc_dconst_0, &&opc_dconst_1,
        &&opc_bipush, &&opc_sipush, 
        &&opc_ldc, &&opc_ldc_w, &&opc_ldc2_w, 
        
        // Loads [0x15 ... 0x35]
        &&opc_iload, &&opc_lload, &&opc_fload, &&opc_dload, &&opc_aload,         
        &&opc_iload_0, &&opc_iload_1, &&opc_iload_2, &&opc_iload_3, 
        &&opc_lload_0, &&opc_lload_1, &&opc_lload_2, &&opc_lload_3, 
        &&opc_fload_0, &&opc_fload_1, &&opc_fload_2, &&opc_fload_3, 
        &&opc_dload_0, &&opc_dload_1, &&opc_dload_2, &&opc_dload_3,
        &&opc_aload_0, &&opc_aload_1, &&opc_aload_2, &&opc_aload_3, 
        &&opc_iaload, &&opc_laload, &&opc_faload, &&opc_daload, &&opc_aaload,
        &&opc_baload, &&opc_caload, &&opc_saload,

        // Stores [0x36 ... 0x56]
        &&opc_istore, &&opc_lstore, &&opc_fstore, &&opc_dstore, &&opc_astore, 
        &&opc_istore_0, &&opc_istore_1, &&opc_istore_2, &&opc_istore_3, 
        &&opc_lstore_0, &&opc_lstore_1, &&opc_lstore_2, &&opc_lstore_3,
        &&opc_fstore_0, &&opc_fstore_1, &&opc_fstore_2, &&opc_fstore_3, 
        &&opc_dstore_0, &&opc_dstore_1, &&opc_dstore_2, &&opc_dstore_3, 
        &&opc_astore_0, &&opc_astore_1, &&opc_astore_2, &&opc_astore_3, 
        &&opc_iastore, &&opc_lastore, &&opc_fastore, &&opc_dastore, &&opc_aastore, 
        &&opc_bastore, &&opc_castore, &&opc_sastore, 
        
        // Stack [0x57 ... 0x5f]
        &&opc_pop, &&opc_pop2, &&opc_dup, &&opc_dup_x1, &&opc_dup_x2, &&opc_dup2, &&opc_dup2_x1, 
        &&opc_dup2_x2, &&opc_swap, 
        
        // Math [0x60 ... 0x84]
        &&opc_iadd, &&opc_ladd, &&opc_fadd, &&opc_dadd, 
        &&opc_isub, &&opc_lsub, &&opc_fsub, &&opc_dsub, 
        &&opc_imul, &&opc_lmul, &&opc_fmul, &&opc_dmul, 
        &&opc_idiv, &&opc_ldiv, &&opc_fdiv, &&opc_ddiv,
        &&opc_irem, &&opc_lrem, &&opc_frem, &&opc_drem, 
        &&opc_ineg, &&opc_lneg, &&opc_fneg, &&opc_dneg, 
        &&opc_ishl, &&opc_lshl, &&opc_ishr, &&opc_lshr, &&opc_iushr, &&opc_lushr, 
        &&opc_iand, &&opc_land, &&opc_ior, &&opc_lor, &&opc_ixor, &&opc_lxor, &&opc_iinc, 
        
        // Conversions [0x85 ... 0x93]
        &&opc_i2l, &&opc_i2f, &&opc_i2d, 
        &&opc_l2i, &&opc_l2f, &&opc_l2d, 
        &&opc_f2i, &&opc_f2l, &&opc_f2d, 
        &&opc_d2i, &&opc_d2l, &&opc_d2f, 
        &&opc_i2b, &&opc_i2c, &&opc_i2s, 
        
        // Comparisons [0x94 ... 0xa6]
        &&opc_lcmp, &&opc_fcmpl, &&opc_fcmpg, &&opc_dcmpl, &&opc_dcmpg, 
        &&opc_ifeq, &&opc_ifne, &&opc_iflt, &&opc_ifge, &&opc_ifgt, &&opc_ifle, 
        &&opc_if_icmpeq, &&opc_if_icmpne, &&opc_if_icmplt, &&opc_if_icmpge,
        &&opc_if_icmpgt, &&opc_if_icmple,     
        &&opc_if_acmpeq, &&opc_if_acmpne, 
        
        // Control [0xa7 ... 0xb1]
        &&opc_goto, &&opc_jsr, &&opc_ret, 
        &&opc_tableswitch, &&opc_lookupswitch, &&opc_ireturn, &&opc_lreturn, &&opc_freturn, &&opc_dreturn, 
        &&opc_areturn, &&opc_return, 
        
        // References [0xb2 ... 0xc3]
        &&opc_getstatic, &&opc_putstatic, &&opc_getfield, &&opc_putfield,             
        &&opc_invokevirtual, &&opc_invokespecial, &&opc_invokestatic, &&opc_invokeinterface,         
        &&opc_invokedynamic, 
        &&opc_new, &&opc_newarray, &&opc_anewarray, &&opc_arraylength, 
        &&opc_athrow, &&opc_checkcast, &&opc_instanceof, &&opc_monitorenter, &&opc_monitorexit, 
        
        // Extended [0xc4 ... 0xc9]
        &&opc_wide, &&opc_multianewarray,
        &&opc_ifnull, &&opc_ifnonnull, &&opc_goto_w, &&opc_jsr_w, 
     
        // Reserved [0xca ... 0xff]
#undef U
#define U &&opc_unused   
        &&opc_breakpoint, 
        U, U, U, U, U,          // [0xcb ... 0xcf]
        U, U, U, U, U, U, U, U, // [0xd0 ... 0xd7]
        U, U, U, U, U, U, U, U, // [0xd8 ... 0xdf]
        U, U, U, U, U, U, U, U, // [0xe0 ... 0xe7]
        U, U, U, U, U, U, U, U, // [0xe8 ... 0xef]
        U, U, U, U, U, U, U, U, // [0xf0 ... 0xf7]
        U, U, U, U, U, U,       // [0xf8 ... 0xfd]
        &&opc_invokenative, &&opc_impdep2
#undef U
    };

    Thread *thread = getCurrentThread();
    Method *resolved_method;

    Frame *frame = thread->getTopFrame();
    TRACE("executing frame: %s\n", frame->toString().c_str());
    
    int index;

    BytecodeReader *reader = &frame->reader;
    Class *clazz = frame->method->clazz;
    ConstantPool *cp = &frame->method->clazz->cp;
    // slot_t *ostack = frame->ostack;
    slot_t *lvars = frame->lvars;

    jref _this = frame->method->isStatic() ? (jref) clazz : RSLOT(lvars);

#define THROW_EXCEPTION(exception_name, message)  \
    {                                         \
        signalException(exception_name, message); \
        jref eo = exceptionOccured();         \
        clearException();                     \
        frame->pushr(eo);                     \
        goto opc_athrow;                      \
    }

#define NULL_POINTER_CHECK(ref)                                          \
    {                                                                    \
        if (ref == nullptr)                                              \
            THROW_EXCEPTION(S(java_lang_NullPointerException), nullptr); \
    }

#define CHANGE_FRAME(new_frame)                                          \
    {                                                                    \
        /*frame->ostack = ostack;  stack指针在变动，需要设置一下 todo */   \
        frame = new_frame;                                               \
        reader = &frame->reader;                                         \
        clazz = frame->method->clazz;                                    \
        cp = &frame->method->clazz->cp;                                  \
        /*ostack = frame->ostack; */                                     \
        lvars = frame->lvars;                                            \
        _this = frame->method->isStatic() ? (jref) clazz : RSLOT(lvars); \
        TRACE("executing frame: %s\n", frame->toString().c_str());       \
    }

    u1 opcode;
    
#define DISPATCH                   \
    {                              \
        opcode = reader->readu1(); \
        PRINT_OPCODE;              \
        goto *handlers[opcode];    \
    }

opc_nop:
    DISPATCH
opc_aconst_null:
    frame->pushr(jnull);
    DISPATCH
opc_iconst_m1:
    frame->pushi(-1);
    DISPATCH
opc_iconst_0:
    frame->pushi(0);
    DISPATCH
opc_iconst_1:
    frame->pushi(1);
    DISPATCH
opc_iconst_2:
    frame->pushi(2);
    DISPATCH
opc_iconst_3:
    frame->pushi(3);
    DISPATCH
opc_iconst_4:
    frame->pushi(4);
    DISPATCH
opc_iconst_5:
    frame->pushi(5);
    DISPATCH
opc_lconst_0:
    frame->pushl(0);
    DISPATCH
opc_lconst_1:
    frame->pushl(1);
    DISPATCH
opc_fconst_0:
    frame->pushf(0);
    DISPATCH
opc_fconst_1:
    frame->pushf(1);
    DISPATCH
opc_fconst_2:
    frame->pushf(2);
    DISPATCH
opc_dconst_0:
    frame->pushd(0);
    DISPATCH
opc_dconst_1:
    frame->pushd(1);
    DISPATCH
opc_bipush: // Byte Integer push
    frame->pushi(reader->reads1());
    DISPATCH
opc_sipush: // Short Integer push
    frame->pushi(reader->reads2());
    DISPATCH
opc_ldc:
    index = reader->readu1();
    goto _ldc;
opc_ldc_w:
    index = reader->readu2();
_ldc: {
    u1 type = cp->type(index);
    switch (type) {
        case JVM_CONSTANT_Integer:
            frame->pushi(cp->_int(index));
            break;
        case JVM_CONSTANT_Float:
            frame->pushf(cp->_float(index));
            break;
        case JVM_CONSTANT_String:
        case JVM_CONSTANT_ResolvedString:
            frame->pushr(cp->resolveString(index));
            break;
        case JVM_CONSTANT_Class:
        case JVM_CONSTANT_ResolvedClass:
            frame->pushr(cp->resolveClass(index)->java_mirror);
            break;
        default:
            THROW_EXCEPTION(S(java_lang_UnknownError), MSG("unknown type: %d", type));
            break;
    }
    DISPATCH
}
opc_ldc2_w: {
    index = reader->readu2();
    u1 type = cp->type(index);
    switch (type) {
        case JVM_CONSTANT_Long:
            frame->pushl(cp->_long(index));
            break;
        case JVM_CONSTANT_Double:
         //    printvm("=====    %f\n", cp->_double(index));
            frame->pushd(cp->_double(index));
            break;
        default:
            THROW_EXCEPTION(S(java_lang_UnknownError), MSG("unknown type: %d", type));
            break;
    }
    DISPATCH
}
opc_iload:
opc_fload:
opc_aload:
    index = reader->readu1();
_iload:
_fload:
_aload:
    *frame->ostack++ = lvars[index];
    DISPATCH
opc_lload:
opc_dload: 
    index = reader->readu1();
_lload:
_dload: 
    *frame->ostack++ = lvars[index];
    *frame->ostack++ = lvars[index + 1];
    DISPATCH
opc_iload_0:
opc_fload_0:
opc_aload_0:
    *frame->ostack++ = lvars[0];
    DISPATCH
opc_iload_1:
opc_fload_1:
opc_aload_1:
    *frame->ostack++ = lvars[1];
    DISPATCH
opc_iload_2:
opc_fload_2:
opc_aload_2:
    *frame->ostack++ = lvars[2];
    DISPATCH
opc_iload_3:
opc_fload_3:
opc_aload_3:
    *frame->ostack++ = lvars[3];
    DISPATCH
opc_lload_0:
opc_dload_0:
    *frame->ostack++ = lvars[0];
    *frame->ostack++ = lvars[1];
    DISPATCH
opc_lload_1:
opc_dload_1:
    *frame->ostack++ = lvars[1];
    *frame->ostack++ = lvars[2];
    DISPATCH
opc_lload_2:
opc_dload_2:
    *frame->ostack++ = lvars[2];
    *frame->ostack++ = lvars[3];
    DISPATCH
opc_lload_3:
opc_dload_3:
    *frame->ostack++ = lvars[3];
    *frame->ostack++ = lvars[4];
    DISPATCH
    
#define GET_AND_CHECK_ARRAY             \
    index = frame->popi();              \
    auto arr = (Array *) frame->popr(); \
    NULL_POINTER_CHECK(arr)             \
    if (!arr->checkBounds(index))       \
       THROW_EXCEPTION(S(java_lang_ArrayIndexOutOfBoundsException), nullptr);

opc_iaload: {
    GET_AND_CHECK_ARRAY
    auto value = arr->get<jint>(index);
    frame->pushi(value);
    DISPATCH
}
opc_faload: {
    GET_AND_CHECK_ARRAY
    auto value = arr->get<jfloat>(index);
    frame->pushf(value);
    DISPATCH
}
opc_aaload: {
    GET_AND_CHECK_ARRAY
    auto value = arr->get<jref>(index);
    frame->pushr(value);
    DISPATCH
}
opc_baload: {
    GET_AND_CHECK_ARRAY
    jint value = arr->get<jbyte>(index);
    frame->pushi(value);
    DISPATCH
}
opc_caload: {
    GET_AND_CHECK_ARRAY
    jint value = arr->get<jchar>(index);
    frame->pushi(value);
    DISPATCH
}
opc_saload: {
    GET_AND_CHECK_ARRAY
    jint value = arr->get<jshort>(index);
    frame->pushi(value);
    DISPATCH
}
opc_laload: {
    GET_AND_CHECK_ARRAY
    auto value = arr->get<jlong>(index);
    frame->pushl(value);
    DISPATCH
}
opc_daload: {
    GET_AND_CHECK_ARRAY
    auto value = arr->get<jdouble>(index);
    frame->pushd(value);
    DISPATCH
}

opc_istore:
opc_fstore:
opc_astore:
    index = reader->readu1();
_istore:
_fstore:
_astore:
    lvars[index] = *--frame->ostack;
    DISPATCH
opc_lstore:
opc_dstore: 
    index = reader->readu1();
_lstore:
_dstore: 
    lvars[index + 1] = *--frame->ostack;
    lvars[index] = *--frame->ostack;
    DISPATCH
opc_istore_0:
opc_fstore_0:
opc_astore_0:
    lvars[0] = *--frame->ostack;
    DISPATCH
opc_istore_1:
opc_fstore_1:
opc_astore_1:
    lvars[1] = *--frame->ostack;
    DISPATCH
opc_istore_2:
opc_fstore_2:
opc_astore_2:
    lvars[2] = *--frame->ostack;
    DISPATCH
opc_istore_3:
opc_fstore_3:
opc_astore_3:
    lvars[3] = *--frame->ostack;
    DISPATCH
opc_lstore_0:
opc_dstore_0:
    lvars[1] = *--frame->ostack;
    lvars[0] = *--frame->ostack;
    DISPATCH
opc_lstore_1:
opc_dstore_1:
    lvars[2] = *--frame->ostack;
    lvars[1] = *--frame->ostack;
    DISPATCH
opc_lstore_2:
opc_dstore_2:
    lvars[3] = *--frame->ostack;
    lvars[2] = *--frame->ostack;
    DISPATCH
opc_lstore_3:
opc_dstore_3:
    lvars[4] = *--frame->ostack;
    lvars[3] = *--frame->ostack;
    DISPATCH
opc_iastore: {
    auto value = frame->popi();
    GET_AND_CHECK_ARRAY
    arr->setInt(index, value);
    DISPATCH
}
opc_fastore: {
    auto value = frame->popf();
    GET_AND_CHECK_ARRAY
    arr->setFloat(index, value);
    DISPATCH
}
opc_aastore: {
    auto value = frame->popr();
    GET_AND_CHECK_ARRAY
    arr->setRef(index, value);
    DISPATCH
}
opc_bastore: {
    auto value = frame->popi();
    GET_AND_CHECK_ARRAY
    if (arr->clazz->isByteArrayClass()) {
        arr->setByte(index, (jbyte) value);
    } else if (arr->clazz->isBooleanArrayClass()) {  
        arr->setBoolean(index, value != 0 ? jtrue : jfalse);
    } else {
        jvm_abort("never go here"); // todo
    }
    DISPATCH
}
opc_castore: {
    auto value = frame->popi();
    GET_AND_CHECK_ARRAY
    arr->setChar(index, (jchar) value);
    DISPATCH
}
opc_sastore: {
    auto value = frame->popi();
    GET_AND_CHECK_ARRAY
    arr->setShort(index, (jshort) value);
    DISPATCH
}
opc_lastore: {
    auto value = frame->popl();
    GET_AND_CHECK_ARRAY
    arr->setLong(index, value);
    DISPATCH
}
opc_dastore: {
    auto value = frame->popd();
    GET_AND_CHECK_ARRAY
    arr->setDouble(index, value);
    DISPATCH
}
#undef GET_AND_CHECK_ARRAY

opc_pop:
    frame->ostack--;
    DISPATCH
opc_pop2:
    frame->ostack -= 2;
    DISPATCH
opc_dup:
    frame->ostack[0] = frame->ostack[-1];
    frame->ostack++;
    DISPATCH
opc_dup_x1:
    frame->ostack[0] = frame->ostack[-1];
    frame->ostack[-1] = frame->ostack[-2];
    frame->ostack[-2] = frame->ostack[0];
    frame->ostack++;
    DISPATCH
opc_dup_x2:
    frame->ostack[0] = frame->ostack[-1];
    frame->ostack[-1] = frame->ostack[-2];
    frame->ostack[-2] = frame->ostack[-3];
    frame->ostack[-3] = frame->ostack[0];
    frame->ostack++;
    DISPATCH
opc_dup2:
    frame->ostack[0] = frame->ostack[-2];
    frame->ostack[1] = frame->ostack[-1];
    frame->ostack += 2;
    DISPATCH
opc_dup2_x1:
    // ..., value3, value2, value1 →
    // ..., value2, value1, value3, value2, value1
    frame->ostack[1] = frame->ostack[-1];
    frame->ostack[0] = frame->ostack[-2];
    frame->ostack[-1] = frame->ostack[-3];
    frame->ostack[-2] = frame->ostack[1];
    frame->ostack[-3] = frame->ostack[0];
    frame->ostack += 2;
    DISPATCH
opc_dup2_x2:
    // ..., value4, value3, value2, value1 →
    // ..., value2, value1, value4, value3, value2, value1
    frame->ostack[1] = frame->ostack[-1];
    frame->ostack[0] = frame->ostack[-2];
    frame->ostack[-1] = frame->ostack[-3];
    frame->ostack[-2] = frame->ostack[-4];
    frame->ostack[-3] = frame->ostack[1];
    frame->ostack[-4] = frame->ostack[0];
    frame->ostack += 2;
    DISPATCH
opc_swap:
    swap(frame->ostack[-1], frame->ostack[-2]);
    DISPATCH

#define BINARY_OP(type, t, oper)     \
    {                                \
        type v2 = frame->pop##t();   \
        type v1 = frame->pop##t();   \
        frame->push##t(v1 oper v2);  \
        DISPATCH                     \
    }
opc_iadd:
    BINARY_OP(jint, i, +)
opc_ladd:
    BINARY_OP(jlong, l, +)
opc_fadd:
    BINARY_OP(jfloat, f, +)
opc_dadd:
    BINARY_OP(jdouble, d, +)
opc_isub:
    BINARY_OP(jint, i, -)
opc_lsub:
    BINARY_OP(jlong, l, -)
opc_fsub:
    BINARY_OP(jfloat, f, -)
opc_dsub:
    BINARY_OP(jdouble, d, -)
opc_imul:
    BINARY_OP(jint, i, *)
opc_lmul:
    BINARY_OP(jlong, l, *)
opc_fmul:
    BINARY_OP(jfloat, f, *)
opc_dmul:
    BINARY_OP(jdouble, d, *)
    
#define ZERO_DIVISOR_CHECK(value)                                                  \
    {                                                                              \
        if (value == 0)                                                            \
            THROW_EXCEPTION(S(java_lang_ArithmeticException), "division by zero"); \
    }

opc_idiv:
    ZERO_DIVISOR_CHECK(getInt(frame->ostack - 1))
    BINARY_OP(jint, i, /)
opc_ldiv:
    ZERO_DIVISOR_CHECK(getLong(frame->ostack - 1))
    BINARY_OP(jlong, l, /)
opc_fdiv:
    ZERO_DIVISOR_CHECK(getFloat(frame->ostack - 1))
    BINARY_OP(jfloat, f, /)
opc_ddiv:
    ZERO_DIVISOR_CHECK(getDouble(frame->ostack - 1))
    BINARY_OP(jdouble, d, /)
opc_irem: 
    ZERO_DIVISOR_CHECK(getInt(frame->ostack - 1))
    BINARY_OP(jint, i, %)
opc_lrem:
    ZERO_DIVISOR_CHECK(getLong(frame->ostack - 1))
    BINARY_OP(jlong, l, %)
#undef ZERO_DIVISOR_CHECK

opc_frem: {
    jfloat v2 = frame->popf();
    jfloat v1 = frame->popf();
    frame->pushf(fmod(v1, v2));
    DISPATCH
}
opc_drem: {
    jdouble v2 = frame->popd();
    jdouble v1 = frame->popd();
    frame->pushd(fmod(v1, v2));
    DISPATCH
}
opc_ineg:
    frame->pushi(-frame->popi());
    DISPATCH
opc_lneg:
    frame->pushl(-frame->popl());
    DISPATCH
opc_fneg:
    frame->pushf(-frame->popf());
    DISPATCH
opc_dneg:
    frame->pushd(-frame->popd());
    DISPATCH 
opc_ishl: {
    // 与0x1f是因为低5位表示位移距离，位移距离实际上被限制在0到31之间。
    jint shift = frame->popi() & 0x1f;
    jint ivalue = frame->popi();
    frame->pushi(ivalue << shift);
    DISPATCH
}
opc_lshl: {
    // 与0x3f是因为低6位表示位移距离，位移距离实际上被限制在0到63之间。
    jint shift = frame->popi() & 0x3f;
    jlong lvalue = frame->popl();
    frame->pushl(lvalue << shift);
    DISPATCH
}
opc_ishr: {
    // 逻辑右移 shift logical right
    jint shift = frame->popi() & 0x1f;
    jint ivalue = frame->popi();
    frame->pushi((~(((jint)1) >> shift)) & (ivalue >> shift));
    DISPATCH
}
opc_lshr: {
    jint shift = frame->popi() & 0x3f;
    jlong lvalue = frame->popl();
    frame->pushl((~(((jlong)1) >> shift)) & (lvalue >> shift));
    DISPATCH
}
opc_iushr: {
    // 算术右移 shift arithmetic right
    jint shift = frame->popi() & 0x1f;
    jint ivalue = frame->popi();
    frame->pushi(ivalue >> shift);
    DISPATCH
}
opc_lushr: {
    jint shift = frame->popi() & 0x3f;
    jlong lvalue = frame->popl();
    frame->pushl(lvalue >> shift);
    DISPATCH
}
opc_iand:
    BINARY_OP(jint, i, &)
opc_land:
    BINARY_OP(jlong, l, &)
opc_ior:
    BINARY_OP(jint, i, |)
opc_lor:
    BINARY_OP(jlong, l, |)
opc_ixor:
    BINARY_OP(jint, i, ^)
opc_lxor:
    BINARY_OP(jlong, l, ^)
#undef BINARY_OP
opc_iinc: 
    index = reader->readu1();
    ISLOT(lvars + index) = ISLOT(lvars + index) + reader->reads1();
    DISPATCH
_wide_iinc:  
    ISLOT(lvars + index) = ISLOT(lvars + index) + reader->readu2();  
    DISPATCH
opc_i2l:
    frame->pushl(frame->popi());
    DISPATCH
opc_i2f:
    frame->pushf(frame->popi());
    DISPATCH
opc_i2d:
    frame->pushd(frame->popi());
    DISPATCH
opc_l2i:
    frame->pushi((jint) frame->popl());
    DISPATCH
opc_l2f:
    frame->pushf((jfloat) frame->popl());
    DISPATCH
opc_l2d:
    frame->pushd(frame->popl());
    DISPATCH
opc_f2i:
   frame->pushi((jint) frame->popf());
    DISPATCH
opc_f2l:
    frame->pushl((jlong) frame->popf());
    DISPATCH
opc_f2d:
    frame->pushd(frame->popf());
    DISPATCH
opc_d2i:
    frame->pushi((jint) frame->popd());
    DISPATCH
opc_d2l:
    frame->pushl((jlong) frame->popd());
    DISPATCH
opc_d2f:
    frame->pushf((jfloat) frame->popd());
    DISPATCH
opc_i2b:
    frame->pushi(jint2jbyte(frame->popi()));
    DISPATCH
opc_i2c:
    frame->pushi(jint2jchar(frame->popi()));
    DISPATCH
opc_i2s:
    frame->pushi(jint2jshort(frame->popi()));
    DISPATCH
/*
 * NAN 与正常的的浮点数无法比较，即 即不大于 也不小于 也不等于。
 * 两个 NAN 之间也无法比较，即 即不大于 也不小于 也不等于。
 */
#define DO_CMP(v1, v2, default_value) \
            (jint)((v1) > (v2) ? 1 : ((v1) == (v2) ? 0 : ((v1) < (v2) ? -1 : (default_value))))

#define CMP(type, t, cmp_result) \
{                                \
    type v2 = frame->pop##t();   \
    type v1 = frame->pop##t();   \
    frame->pushi(cmp_result);    \
    DISPATCH                     \
}
opc_lcmp: 
    CMP(jlong, l, DO_CMP(v1, v2, -1))
opc_fcmpl: 
    CMP(jfloat, f, DO_CMP(v1, v2, -1))
opc_fcmpg: 
    CMP(jfloat, f, DO_CMP(v1, v2, 1))
opc_dcmpl:
    CMP(jdouble, d, DO_CMP(v1, v2, -1))
opc_dcmpg:
    CMP(jdouble, d, DO_CMP(v1, v2, 1))

#define IF_COND(cond)                                             \
{                                                                 \
    jint v = frame->popi();                                       \
    jint offset = reader->reads2();                               \
    if (v cond 0)                                                 \
        reader->skip(offset - 3);  /* minus instruction length */ \
    DISPATCH                                                      \
}
opc_ifeq:
    IF_COND(==)
opc_ifne:
    IF_COND(!=)
opc_iflt:
    IF_COND(<)
opc_ifge:
    IF_COND(>=)
opc_ifgt:
    IF_COND(>)
opc_ifle:
    IF_COND(<=)
#undef IF_COND

#define IF_CMP_COND(t, cond)                                     \
{                                                                \
    s2 offset = reader->reads2();                                \
    auto v2 = frame->pop##t();                                   \
    auto v1 = frame->pop##t();                                   \
    if (v1 cond v2)                                              \
        reader->skip(offset - 3); /* minus instruction length */ \
    DISPATCH                                                     \
}
opc_if_icmpeq:
    IF_CMP_COND(i, ==)
opc_if_acmpeq:
    IF_CMP_COND(r, ==)
opc_if_icmpne:
    IF_CMP_COND(i, !=)
opc_if_acmpne:
    IF_CMP_COND(r, !=)
opc_if_icmplt:
    IF_CMP_COND(i, <)
opc_if_icmpge:
    IF_CMP_COND(i, >=)
opc_if_icmpgt:
    IF_CMP_COND(i, >)
opc_if_icmple:
    IF_CMP_COND(i, <=)
#undef IF_CMP_COND

opc_goto: {
    s2 offset = reader->reads2();
    reader->skip(offset - 3);  // minus instruction length
    DISPATCH
}

// 在Java 6之前，Oracle的Java编译器使用 jsr, jsr_w 和 ret 指令来实现 finally 子句。
// 从Java 6开始，已经不再使用这些指令
opc_jsr:
    THROW_EXCEPTION(S(java_lang_InternalError), "jsr doesn't support after jdk 6.");
opc_ret:
    THROW_EXCEPTION(S(java_lang_InternalError), "ret doesn't support after jdk 6.");

opc_tableswitch: {
    // todo 指令说明  好像是实现 switch 语句
    size_t saved_pc = reader->pc - 1; // save the pc before 'tableswitch' instruction
    reader->align4();

    // 默认情况下执行跳转所需字节码的偏移量
    // 对应于 switch 中的 default 分支。
    s4 default_offset = reader->reads4();

    // low 和 height 标识了 case 的取值范围。
    s4 low = reader->reads4();
    s4 height = reader->reads4();

    // 跳转偏移量表，对应于各个 case 的情况
    s4 jump_offset_count = height - low + 1;
    s4 jump_offsets[jump_offset_count];
    reader->reads4s(jump_offset_count, jump_offsets);

    // 弹出要判断的值
    index = frame->popi();
    s4 offset;
    if (index < low || index > height) {
        offset = default_offset; // 没在 case 标识的范围内，跳转到 default 分支。
    } else {
        offset = jump_offsets[index - low]; // 找到对应的case了
    }

    // The target address that can be calculated from each jump table
    // offset, as well as the one that can be calculated from default,
    // must be the address of an opcode of an instruction within the method
    // that contains this tableswitch instruction.
    reader->pc = saved_pc + offset;
    DISPATCH
}
opc_lookupswitch: {
    // todo 指令说明  好像是实现 switch 语句
    size_t saved_pc = reader->pc - 1; // save the pc before 'lookupswitch' instruction
    reader->align4();

    // 默认情况下执行跳转所需字节码的偏移量
    // 对应于 switch 中的 default 分支。
    s4 default_offset = reader->reads4();

    // case的个数
    s4 npairs = reader->reads4();
    assert(npairs >= 0); // The npairs must be greater than or equal to 0.

    // match_offsets 有点像 Map，它的 key 是 case 值，value 是跳转偏移量。
    s4 match_offsets[npairs * 2];
    reader->reads4s(npairs * 2, match_offsets);

    // 弹出要判断的值
    jint key = frame->popi();
    s4 offset = default_offset;
    for (int i = 0; i < npairs * 2; i += 2) {
        if (match_offsets[i] == key) { // 找到 case
            offset = match_offsets[i + 1];
            break;
        }
    }

    // The target address is calculated by adding the corresponding offset
    // to the address of the opcode of this lookupswitch instruction.
    reader->pc = saved_pc + offset;
    DISPATCH
}                

    int ret_value_slot_count;
opc_ireturn:
opc_freturn:
opc_areturn:
    ret_value_slot_count = 1;
    goto _method_return;
opc_lreturn:
opc_dreturn:
    ret_value_slot_count = 2;
    goto _method_return;
opc_return:
    ret_value_slot_count = 0;
_method_return: {
    TRACE("will return: %s\n", frame->toString().c_str());
    thread->popFrame();
    Frame *invoke_frame = thread->getTopFrame();
    TRACE("invoke frame: %s\n", invoke_frame == nullptr ? "NULL" : invoke_frame->toString().c_str());
    frame->ostack -= ret_value_slot_count;
    slot_t *ret_value = frame->ostack;
    if (frame->vm_invoke || invoke_frame == nullptr) {
        if (frame->method->isSynchronized()) {
//                        _this->unlock();
        }
        return ret_value;
    }

    for (int i = 0; i < ret_value_slot_count; i++) {
        *invoke_frame->ostack++ = *ret_value++;
    }
    if (frame->method->isSynchronized()) {
//                    _this->unlock();
    }
    CHANGE_FRAME(invoke_frame)
    DISPATCH  
}
opc_getstatic: {
    index = reader->readu2();
    Field *field = cp->resolveField(index);
    if (!field->isStatic()) {
        THROW_EXCEPTION(S(java_lang_IncompatibleClassChangeError), nullptr);
    }

    initClass(field->clazz);

    *frame->ostack++ = field->static_value.data[0];
    if (field->category_two) {
        *frame->ostack++ = field->static_value.data[1];
    }
    DISPATCH
}
opc_putstatic: {
    index = reader->readu2();
    Field *field = cp->resolveField(index);
    if (!field->isStatic()) {
        THROW_EXCEPTION(S(java_lang_IncompatibleClassChangeError), nullptr);
    }

    initClass(field->clazz);

    if (field->category_two) {
        frame->ostack -= 2;
        field->static_value.data[0] = frame->ostack[0];
        field->static_value.data[1] = frame->ostack[1];
    } else {
        field->static_value.data[0] = *--frame->ostack;
    }

    DISPATCH
}                
opc_getfield: {
    index = reader->readu2();
    Field *field = cp->resolveField(index);
    if (field->isStatic()) {
        THROW_EXCEPTION(S(java_lang_IncompatibleClassChangeError), nullptr);
    }

    jref obj = frame->popr();
    NULL_POINTER_CHECK(obj)

    *frame->ostack++ = obj->data[field->id];
    if (field->category_two) {
        *frame->ostack++ = obj->data[field->id + 1];
    }
    DISPATCH
}
opc_putfield: {
    index = reader->readu2();
    Field *field = cp->resolveField(index);
    if (field->isStatic()) {
        THROW_EXCEPTION(S(java_lang_IncompatibleClassChangeError), nullptr);
    }

    // 如果是final字段，则只能在构造函数中初始化，否则抛出java.lang.IllegalAccessError。
    if (field->isFinal()) {
        if (!clazz->equals(field->clazz) || !equals(frame->method->name, S(object_init))) {
            THROW_EXCEPTION(S(java_lang_IllegalAccessError), nullptr);
        }
    }

    if (field->category_two) {
        frame->ostack -= 2;
    } else {
        frame->ostack--;
    }
    slot_t *value = frame->ostack;

    jref obj = frame->popr();
    NULL_POINTER_CHECK(obj)

    obj->setFieldValue(field, value);
    DISPATCH
}                   
opc_invokevirtual: {
    // invokevirtual指令用于调用对象的实例方法，根据对象的实际类型进行分派（虚方法分派）。
    index = reader->readu2();
    Method *m = cp->resolveMethod(index);
    if (m == nullptr) {
        // todo ....
    }
    if (m->isStatic()) {
        THROW_EXCEPTION(S(java_lang_IncompatibleClassChangeError), nullptr);
    }

    frame->ostack -= m->arg_slot_count;
    jref obj = RSLOT(frame->ostack);
    NULL_POINTER_CHECK(obj)

    if (m->isPrivate()) {
        resolved_method = m;
    } else {
        // assert(m->vtable_index >= 0);
        // assert(m->vtable_index < (int) obj->clazz->vtable.size());
        // resolved_method = obj->clazz->vtable[m->vtable_index];
        resolved_method = obj->clazz->lookupMethod(m->name, m->descriptor);
    }

    // assert(resolved_method == obj->clazz->lookupMethod(m->name, m->descriptor));
    goto _invoke_method;
}
opc_invokespecial: {
    // invokespecial指令用于调用一些需要特殊处理的实例方法， 包括：
    // 1. 构造函数
    // 2. 私有方法
    // 3. 通过super关键字调用的超类方法，或者超接口中的默认方法。
    index = reader->readu2();
    Method *m = cp->resolveMethodOrInterfaceMethod(index);

    /*
     * 如果调用的中超类中的函数，但不是构造函数，不是private 函数，且当前类的ACC_SUPER标志被设置，
     * 需要一个额外的过程查找最终要调用的方法；否则前面从方法符号引用中解析出来的方法就是要调用的方法。
     * todo 详细说明
     */
    if (m->clazz->isSuper()
        && !m->isPrivate()
        && clazz->isSubclassOf(m->clazz) // todo
        && !utf8::equals(m->name, S(object_init))) {
        m = clazz->super_class->lookupMethod(m->name, m->descriptor);
    }

    if (m->isAbstract()) {
        THROW_EXCEPTION(S(java_lang_AbstractMethodError), nullptr);
    }
    if (m->isStatic()) {
        THROW_EXCEPTION(S(java_lang_IncompatibleClassChangeError), nullptr);
    }

    frame->ostack -= m->arg_slot_count;
    jref obj = RSLOT(frame->ostack);
    NULL_POINTER_CHECK(obj)

    resolved_method = m;
    goto _invoke_method;
}
opc_invokestatic: {
    // invokestatic指令用来调用静态方法。
    // 如果类还没有被初始化，会触发类的初始化。
    index = reader->readu2();
    Method *m = cp->resolveMethodOrInterfaceMethod(index);
    if (m->isAbstract()) {
        THROW_EXCEPTION(S(java_lang_AbstractMethodError), nullptr);
    }
    if (!m->isStatic()) {
        THROW_EXCEPTION(S(java_lang_IncompatibleClassChangeError), nullptr);
    }

    initClass(m->clazz);

    frame->ostack -= m->arg_slot_count;
    resolved_method = m;
    goto _invoke_method;
}            
opc_invokeinterface: {
    index = reader->readu2();

    /*
     * 此字节的值是给方法传递参数需要的slot数，
     * 其含义和给method结构体定义的arg_slot_count字段相同。
     * 这个数是可以根据方法描述符计算出来的，它的存在仅仅是因为历史原因。
     */
    reader->readu1();
    /*
     * 此字节是留给Oracle的某些Java虚拟机实现用的，它的值必须是0。
     * 该字节的存在是为了保证Java虚拟机可以向后兼容。
     */
    reader->readu1();

    Method *m = cp->resolveInterfaceMethod(index);
    assert(m->clazz->isInterface());

    /* todo 本地方法 */

    frame->ostack -= m->arg_slot_count;
    jref obj = RSLOT(frame->ostack);
    NULL_POINTER_CHECK(obj)

    // itable的实现还不对 todo
    // resolved_method = obj->clazz->findFromITable(m->clazz, m->itable_index);
    // assert(resolved_method != nullptr);
    // assert(resolved_method == obj->clazz->lookupMethod(m->name, m->descriptor));
    resolved_method = obj->clazz->lookupMethod(m->name, m->descriptor);
    if (resolved_method->isAbstract()) {
        THROW_EXCEPTION(S(java_lang_AbstractMethodError), nullptr);
    }

    if (!resolved_method->isPublic()) {
        THROW_EXCEPTION(S(java_lang_IllegalAccessError), nullptr);
    }

    goto _invoke_method;
}           
opc_invokedynamic: {
    printvm("invokedynamic\n"); /////////////////////////////////////////////////////////////////////////////////

    ConstantPool &cp = clazz->cp;

    u2 index = reader->readu2(); // point to JVM_CONSTANT_InvokeDynamic_info
    reader->readu1(); // this byte must always be zero.
    reader->readu1(); // this byte must always be zero.

    const utf8_t *invoked_name = cp.invokeDynamicMethodName(index); // "run"
    const utf8_t *invoked_descriptor = cp.invokeDynamicMethodType(index); // "()Ljava/lang/Runnable;"

    jref invoked_type = fromMethodDescriptor(invoked_descriptor, clazz->loader); // "java/lang/invoke/MethodType"
    jref caller = getCaller(); // "java/lang/invoke/MethodHandles$Lookup"

    BootstrapMethod &bm = clazz->bootstrap_methods.at(cp.invokeDynamicBootstrapMethodIndex(index));
    u2 ref_kind = cp.methodHandleReferenceKind(bm.bootstrap_method_ref); // 6
    u2 ref_index = cp.methodHandleReferenceIndex(bm.bootstrap_method_ref); // 40

    switch (ref_kind) {
        case JVM_REF_invokeStatic: {
            const utf8_t *class_name = cp.methodClassName(ref_index); // "java/lang/invoke/LambdaMetafactory"
            Class *bootstrap_class = loadClass(clazz->loader, class_name);

            // bootstrap method is static,  todo 对不对
            // 前三个参数固定为 MethodHandles.Lookup caller, String invokedName, MethodType invokedType todo 对不对
            // 后续的参数由 ref->argc and ref->args 决定
            Method *bootstrap_method = bootstrap_class->getDeclaredStaticMethod(
                                    cp.methodName(ref_index), cp.methodType(ref_index));
            // name: "metafactory"
            // type: "(Ljava/lang/invoke/MethodHandles$Lookup;Ljava/lang/String;Ljava/lang/invoke/MethodType;Ljava/lang/invoke/MethodType;Ljava/lang/invoke/MethodHandle;Ljava/lang/invoke/MethodType;)Ljava/lang/invoke/CallS"...
            
            // args's length is big enough,多余的长度无所谓，bootstrap_method 会按需读取的。
            slot_t args[3 + bm.bootstrap_arguments.size() * 2];
            RSLOT(args) = caller;
            RSLOT(args + 1) = newString(invoked_name);
            RSLOT(args + 2) = invoked_type;
            bm.resolveArgs(&cp, args + 3);
            auto call_set = RSLOT(execJavaFunc(bootstrap_method, args));

            // public abstract MethodHandle dynamicInvoker()
            auto dyn_invoker = call_set->clazz->lookupInstMethod("dynamicInvoker", "()Ljava/lang/invoke/MethodHandle;");
            auto exact_method_handle = RSLOT(execJavaFunc(dyn_invoker, {call_set}));

            // public final Object invokeExact(Object... args) throws Throwable
            Method *invokeExact = exact_method_handle->clazz->lookupInstMethod(
                                        "invokeExact", "([Ljava/lang/Object;)Ljava/lang/Object;");
            assert(invokeExact->isVarargs());
            u2 slot_count = Method::calArgsSlotsCount(invoked_descriptor, true);
            slot_t _args[slot_count];
            RSLOT(_args) = exact_method_handle;
            slot_count--; // 减去"this"
            frame->ostack -= slot_count; // pop all args
            memcpy(_args + 1, frame->ostack, slot_count * sizeof(slot_t));
            // invoke exact method, invokedynamic completely execute over.
            execJavaFunc(invokeExact, _args);

            break;
        }
        case JVM_REF_newInvokeSpecial:
            jvm_abort("JVM_REF_newInvokeSpecial"); // todo
            break;
        default:
            jvm_abort("never goes here"); // todo
            break;
    }
    jvm_abort("never goes here"); // todo

    DISPATCH
}
_invoke_method: {
    assert(resolved_method);
    Frame *new_frame = thread->allocFrame(resolved_method, false);
    TRACE("Alloc new frame: %s\n", new_frame->toString().c_str());

    new_frame->lvars = frame->ostack; // todo 什么意思？？？？？？？？
    CHANGE_FRAME(new_frame)    
    if (resolved_method->isSynchronized()) {
//        _this->unlock(); // todo why unlock 而不是 lock ................................................
    }
    DISPATCH
}
opc_new: {
    // new指令专门用来创建类实例。数组由专门的指令创建
    // 如果类还没有被初始化，会触发类的初始化。
    Class *c = cp->resolveClass(reader->readu2());
    initClass(c);

    if (c->isInterface() || c->isAbstract()) {
        THROW_EXCEPTION(S(java_lang_InstantiationException), nullptr);
    }

    // jref o = newObject(c);
    // if (strcmp(o->clazz->className, "java/lang/invoke/MemberName") == 0)
    //     printvm("%s\n", o->toString().c_str()); /////////////////////////////////////////////////////////////
    // frame->pushr(o);
    frame->pushr(newObject(c));
    DISPATCH
}
opc_newarray: {
    // 创建一维基本类型数组。
    // 包括 boolean[], byte[], char[], short[], int[], long[], float[] 和 double[] 8种。
    jint arr_len = frame->popi();
    if (arr_len < 0) {
        THROW_EXCEPTION(S(java_lang_NegativeArraySizeException), nullptr);
    }

    auto arr_type = reader->readu1();
    frame->pushr(newTypeArray(ArrayType(arr_type), arr_len));
    DISPATCH
}
opc_anewarray: {
    // 创建一维引用类型数组
    jint arr_len = frame->popi();
    if (arr_len < 0) {
        THROW_EXCEPTION(S(java_lang_NegativeArraySizeException), nullptr);
    }

    index = reader->readu2();
    Class *ac = cp->resolveClass(index)->arrayClass();
    frame->pushr(newArray(ac, arr_len));
    DISPATCH
}
opc_multianewarray: {
    // 创建多维数组
    index = reader->readu2();
    Class *ac = cp->resolveClass(index);

    u1 dim = reader->readu1(); // 多维数组的维度
    if (dim < 1) { // 必须大于或等于1
        THROW_EXCEPTION(S(java_lang_UnknownError),
                        "The dimensions must be greater than or equal to 1.");
    }

    jint lens[dim];
    for (int i = dim - 1; i >= 0; i--) {
        lens[i] = frame->popi();
        if (lens[i] < 0) {
            THROW_EXCEPTION(S(java_lang_NegativeArraySizeException), nullptr);
        }
    }
    frame->pushr(newMultiArray(ac, dim, lens));
    DISPATCH
}           
opc_arraylength: {
    Object *o = frame->popr();
    NULL_POINTER_CHECK(o);
    if (!o->isArrayObject()) {
        THROW_EXCEPTION(S(java_lang_UnknownError), "not a array");
    }
    
    frame->pushi(((Array *) o)->len);
    DISPATCH
}
opc_athrow: {
    jref eo = frame->popr(); // exception object
    if (eo == jnull) {
        signalException(S(java_lang_NullPointerException), nullptr);
        eo = exceptionOccured();
        assert(eo != nullptr);
        clearException();
    }

    // 遍历虚拟机栈找到可以处理此异常的方法
    while (true) {
        int handler_pc = frame->method->findExceptionHandler(eo->clazz, reader->pc - 1); // instruction length todo 好像是错的
        if (handler_pc >= 0) {  // todo 可以等于0吗
            /*
             * 找到可以处理的代码块了
             * 操作数栈清空 // todo 为啥要清空操作数栈
             * 把异常对象引用推入栈顶
             * 跳转到异常处理代码之前
             */
            frame->clearStack();
            frame->pushr(eo);
            reader->pc = (size_t) handler_pc;

            TRACE("athrow: find exception handler: %s\n", frame->toString().c_str());
            break;
        }

        if (frame->vm_invoke) {
            // frame 由虚拟机调用，将异常交由虚拟机处理
            setException(eo);
            return nullptr;
        }

        // frame 无法处理异常，弹出
        thread->popFrame();

        if (frame->prev == nullptr) {
            // 虚拟机栈已空，还是无法处理异常，交由虚拟机处理
            setException(eo);
            return nullptr;
        }

        TRACE("athrow: pop frame: %s\n", frame->toString().c_str());
        CHANGE_FRAME(frame->prev)
    }
    DISPATCH
}
    
opc_checkcast: {
    jref obj = RSLOT(frame->ostack - 1); // 不改变操作数栈
    index = reader->readu2();

    // 如果引用是null，则指令执行结束。也就是说，null 引用可以转换成任何类型
    if (obj != jnull) {
        Class *c = cp->resolveClass(index);
        if (!checkcast(obj->clazz, c)) {
            auto err_msg = MSG("class %s cannot be cast to %s", obj->clazz->class_name, c->class_name);
            THROW_EXCEPTION(S(java_lang_ClassCastException), err_msg);
        }
    }
    DISPATCH
}

opc_instanceof: {
    index =  reader->readu2();
    Class *c = cp->resolveClass(index);

    jref obj = frame->popr();
    if (obj == jnull) {
        frame->pushi(0);
    } else {
        frame->pushi(checkcast(obj->clazz, c) ? 1 : 0);
    }
    DISPATCH
}
opc_monitorenter: {
    jref o = frame->popr();
    NULL_POINTER_CHECK(o);
//                o->lock();
    DISPATCH
}
opc_monitorexit: {
    jref o = frame->popr();
    NULL_POINTER_CHECK(o);
//                o->unlock();
    DISPATCH
}
opc_wide:
    opcode = reader->readu1();
    PRINT_OPCODE
    index = reader->readu2();
    switch (opcode) {
        case JVM_OPC_iload:  goto _iload;
        case JVM_OPC_fload:  goto _fload;
        case JVM_OPC_aload:  goto _aload;
        case JVM_OPC_lload:  goto _lload;
        case JVM_OPC_dload:  goto _dload;
        case JVM_OPC_istore: goto _istore;
        case JVM_OPC_fstore: goto _fstore;
        case JVM_OPC_astore: goto _astore;
        case JVM_OPC_lstore: goto _lstore;
        case JVM_OPC_dstore: goto _dstore;
        case JVM_OPC_ret:    goto opc_ret;
        case JVM_OPC_iinc:   goto _wide_iinc;
        default:
            THROW_EXCEPTION(S(java_lang_UnknownError), "never goes here.");
    }  
opc_ifnull: {
    s2 offset = reader->reads2();
    if (frame->popr() == jnull) {
        reader->skip(offset - 3); // minus instruction length
    }
    DISPATCH
}
opc_ifnonnull: {
    s2 offset = reader->reads2();
    if (frame->popr() != jnull) {
        reader->skip(offset - 3); // minus instruction length
    }
    DISPATCH
}
opc_goto_w:
    THROW_EXCEPTION(S(java_lang_InternalError), "goto_w doesn't support");
    DISPATCH
opc_jsr_w: 
    THROW_EXCEPTION(S(java_lang_InternalError), "jsr_w doesn't support after jdk 6.");
    DISPATCH
opc_breakpoint:  
    THROW_EXCEPTION(S(java_lang_InternalError), "breakpoint doesn't support in this jvm.");
    DISPATCH
opc_invokenative: {
    TRACE("%s\n", frame->toString().c_str());
    if (frame->method->native_method == nullptr){ // todo
        jvm_abort("not find native method: %s\n", frame->method->toString().c_str());
    }

    // todo 不需要在这里做任何同步的操作

    assert(frame->method->native_method != nullptr);

    // if (strcmp(frame->method->clazz->className, "java/lang/invoke/MethodHandle") == 0) {
    //     ((void (*)(Frame *)) frame->method->native_method)(frame);
    // } else {
    //     callJNIMethod(frame);
    // }
    callJNIMethod(frame);
    jref eo = exceptionOccured();
    if (eo != jnull) {
        TRACE("native method throw a exception\n");
        clearException();                     
        frame->pushr(eo);                     
        goto opc_athrow;                      
    }

//    if (frame->method->isSynchronized()) {
//        _this->unlock();
//    }
    DISPATCH
}
opc_impdep2:
    jvm_abort("This instruction isn't used.\n"); // todo
    DISPATCH            
opc_unused:
    jvm_abort("This instruction isn't used. %d(0x%x)\n", opcode, opcode); // todo    
    DISPATCH    
}

// check can s cast to t?
static bool checkcast(Class *s, Class *t)
{
    assert(s != nullptr && t != nullptr);
    if (!s->isArrayClass()) {
        if (t->isArrayClass()) 
            return false;
        return s->isSubclassOf(t);
    } else { // s is array type
        if (t->isInterface()) {
            // 数组实现了两个接口，看看t是不是其中之一。
            return s->isSubclassOf(t);
        } else if (t->isArrayClass()) { // s and t are both array type
            Class *sc = s->componentClass();
            Class *tc = t->componentClass();
            if (sc->isPrimClass() || tc->isPrimClass()) {
                // s and t are same prim type array.
                return sc == tc;
            }
            return checkcast(sc, tc);
        } else { // t is not interface and array type,
            return equals(t->class_name, S(java_lang_Object));
        }
    }
}

slot_t *execJavaFunc(Method *method, const slot_t *args)
{
    assert(method != nullptr);
    assert(method->arg_slot_count > 0 ? args != nullptr : true);

    Frame *frame = getCurrentThread()->allocFrame(method, true);

    // 准备参数
    for (int i = 0; i < method->arg_slot_count; i++) {
        // 传递参数到被调用的函数。
        frame->lvars[i] = args[i];
    }

    slot_t *result = exec();
    if (exceptionOccured()) {
        printStackTrace();
        JVM_EXIT
    }

    return result;
}

slot_t *execJavaFunc(Method *method, initializer_list<slot_t> args)
{
    assert(method != nullptr);
    assert(method->arg_slot_count == args.size());

    slot_t slots[args.size()];
    int i = 0;
    for (slot_t arg : args) {
        slots[i++] = arg;
    }

    return execJavaFunc(method, slots);
}

slot_t *execJavaFunc(Method *method, std::initializer_list<jref> args)
{
    assert(method != nullptr);
    assert(method->arg_slot_count == args.size());

    slot_t slots[args.size()];
    int i = 0;
    for (jref arg : args) {
        slots[i++] = slot::rslot(arg);
    }

    return execJavaFunc(method, slots);
}

slot_t *execJavaFunc(Method *m, jref _this, Array *args)
{
    assert(m != nullptr);
    assert(_this != nullptr);

    if (args == nullptr) {
        return execJavaFunc(m, {_this});
    }

    // Class[]
    Array *types = m->getParameterTypes();
    assert(types != nullptr);
    assert(types->len == args->len);

    // 因为有 category two 的存在，result 的长度最大为 types_len * 2 + this_obj
    auto real_args = new slot_t[2 * types->len + 1];
    RSLOT(real_args) = _this;
    int k = 1; // 第0位已经存放_this了，下面从第一位开始。
    for (int i = 0; i < types->len; i++) {
        auto c = types->get<ClassObject *>(i)->jvm_mirror;
        auto o = args->get<jref>(i);

        if (c->isPrimClass()) {
            const slot_t *unbox = o->unbox();
            real_args[k++] = *unbox;
            if (strcmp(o->clazz->class_name, "long") == 0
                || strcmp(o->clazz->class_name, "double") == 0) // category_two
                real_args[k++] = *++unbox;
        } else {
            RSLOT(real_args + k) = o;
            k++;
        }
    }

    return execJavaFunc(m, real_args);
}

static inline void apply(void *func, int argc,
                         ffi_type *rtype, ffi_type **arg_types,
                         void *rvalue, void **arg_values)
{
    ffi_cif cif;
    ffi_prep_cif(&cif, FFI_DEFAULT_ABI, argc, rtype, arg_types);
    ffi_call(&cif, (void (*)())func, rvalue, arg_values);
}

static void callJNIMethod(Frame *frame)
{
    assert(frame != nullptr && frame->method != nullptr);
    assert(frame->method->isNative() && frame->method->native_method != nullptr);

    Method *m = frame->method;
    int arg_count_max = m->arg_slot_count + (m->isStatic() ? 1 /* jclsref */ : 0);

    ffi_type *arg_types[arg_count_max];
    void *arg_values[arg_count_max];

    // 准备参数
    int argc = 0;
    const slot_t *lvars = frame->lvars;
    if (!m->isStatic()) {
        arg_types[argc] = &ffi_type_pointer;
        arg_values[argc] = (void *) lvars; // this
        lvars++;
        argc++;
    }

    const char *p = m->descriptor;
    assert(*p == JVM_SIGNATURE_FUNC);
    p++; // skip start (

    for (; *p != JVM_SIGNATURE_ENDFUNC; lvars++, p++, argc++) {
        switch (*p) {
            case JVM_SIGNATURE_BOOLEAN:
            case JVM_SIGNATURE_BYTE: {
                jbyte b = jint2jbyte(ISLOT(lvars));
                *(jbyte *)lvars = b;
                arg_types[argc] = &ffi_type_sint8;
                arg_values[argc] = (void *) lvars;
                break;
            }
            case JVM_SIGNATURE_CHAR: {
                jchar c = jint2jchar(ISLOT(lvars));
                *(jchar *)lvars = c;
                arg_types[argc] = &ffi_type_uint16;
                arg_values[argc] = (void *) lvars;
                break;
            }
            case JVM_SIGNATURE_SHORT: {
                jshort s = jint2jshort(ISLOT(lvars));
                *(jshort *)lvars = s;
                arg_types[argc] = &ffi_type_sint16;
                arg_values[argc] = (void *) lvars;
                break;
            }
            case JVM_SIGNATURE_INT:
                arg_types[argc] = &ffi_type_sint32;
                arg_values[argc] = (void *) lvars;
                break;
            case JVM_SIGNATURE_FLOAT:
                arg_types[argc] = &ffi_type_float;
                arg_values[argc] = (void *) lvars;
                break;
            case JVM_SIGNATURE_LONG:
                arg_types[argc] = &ffi_type_sint64;
                arg_values[argc] = (void *) lvars;
                lvars++;
                break;
            case JVM_SIGNATURE_DOUBLE:
                arg_types[argc] = &ffi_type_double;
                arg_values[argc] = (void *) lvars;
                lvars++;
                break;
            case JVM_SIGNATURE_ARRAY:
                while (*++p == JVM_SIGNATURE_ARRAY);
                if (*p != JVM_SIGNATURE_CLASS) { // 基本类型的数组
                    goto __ref;
                }
            case JVM_SIGNATURE_CLASS:
                while(*++p != JVM_SIGNATURE_ENDCLASS);
            __ref:
                arg_types[argc] = &ffi_type_pointer;
                arg_values[argc] = (void *) lvars;
                break;
            default:
                // todo error
                break;
        }
    }

    switch (m->ret_type) {
        case Method::RET_VOID:
            apply(m->native_method, argc, &ffi_type_void, arg_types, nullptr, arg_values);
            break;
        case Method::RET_BYTE:
        case Method::RET_BOOL: {
            jbyte ret_value;
            apply(m->native_method, argc, &ffi_type_sint8, arg_types, &ret_value, arg_values);
            frame->pushi(ret_value);
            break;
        }
        case Method::RET_CHAR: {
            jchar ret_value;
            apply(m->native_method, argc, &ffi_type_uint16, arg_types, &ret_value, arg_values);
            frame->pushi(ret_value);
            break;
        }
        case Method::RET_SHORT: {
            jshort ret_value;
            apply(m->native_method, argc, &ffi_type_sint16, arg_types, &ret_value, arg_values);
            frame->pushi(ret_value);
            break;
        }
        case Method::RET_INT: {
            jint ret_value;
            apply(m->native_method, argc, &ffi_type_sint32, arg_types, &ret_value, arg_values);
            frame->pushi(ret_value);
            break;
        }
        case Method::RET_FLOAT: {
            jfloat ret_value;
            apply(m->native_method, argc, &ffi_type_float, arg_types, &ret_value, arg_values);
            frame->pushf(ret_value);
            break;
        }
        case Method::RET_LONG: {
            jlong ret_value;
            apply(m->native_method, argc, &ffi_type_sint64, arg_types, &ret_value, arg_values);
            frame->pushl(ret_value);
            break;
        }
        case Method::RET_DOUBLE: {
            jdouble ret_value;
            apply(m->native_method, argc, &ffi_type_double, arg_types, &ret_value, arg_values);
            frame->pushd(ret_value);
            break;
        }
        case Method::RET_REFERENCE: {
            jref ret_value;
            apply(m->native_method, argc, &ffi_type_pointer, arg_types, &ret_value, arg_values);
            // printvm("ret_value: %p\n", ret_value);
            frame->pushr(ret_value);
            break;
        }
        default:
            // todo error
            jvm_abort("never go here");
            break;
    }
}
