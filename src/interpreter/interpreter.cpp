/*
 * Author: kayo
 */

#include <iostream>
#include <cmath>
#include "interpreter.h"
#include "../kayo.h"
#include "../debug.h"
#include "../runtime/Thread.h"
#include "../runtime/Frame.h"
#include "../classfile/constant.h"
#include "../objects/Array.h"
#include "../objects/Class.h"
#include "../objects/Field.h"
#include "../objects/invoke.h"

using namespace std;
using namespace utf8;
using namespace method_type;
using namespace method_handles;

#if TRACE_INTERPRETER
#define TRACE PRINT_TRACE
#else
#define TRACE(...)
#endif

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
        "breakpoint",
        "ldc_quick", "ldc_w_quick", "getfield_quick", "getfield2_quick", "invokestatic_quick", // [0xcb ... 0xcf]
        "notused", "notused", "notused", "notused", "notused", "notused", "notused", "notused", // [0xd0 ... 0xd7]
        "notused", "notused", "notused", "notused", "notused", "notused", "notused", "notused", // [0xd8 ... 0xdf]
        "notused", "notused", "notused", "notused", "notused", "notused", "notused", "notused", // [0xe0 ... 0xe7]
        "notused", "notused", "notused", "notused", "notused", "notused", "notused", "notused", // [0xe8 ... 0xef]
        "notused", "notused", "notused", "notused", "notused", "notused", "notused", "notused", // [0xf0 ... 0xf7]
        "notused", "notused", "notused", "notused", "notused", "notused", // [0xf8 ... 0xfd]
        "invokenative", "impdep2"
};
#endif


/*
 * 执行当前线程栈顶的frame
 */
static slot_t *exec()
{
    Thread *thread = getCurrentThread();

    Method *resolved_method;

    Frame *frame = thread->getTopFrame();
    TRACE("executing frame: %s\n", frame->toString().c_str());

    BytecodeReader *reader = &frame->reader;
    Class *clazz = frame->method->clazz;
    ConstantPool *cp = &frame->method->clazz->cp;
    slot_t *ostack = frame->ostack;
    slot_t *lvars = frame->lvars;

    jref _this = frame->method->isStatic() ? (jref) clazz : (jref) lvars[0];

#define CHANGE_FRAME(newFrame) \
    do { \
        /*frame->ostack = ostack;  stack指针在变动，需要设置一下 todo */ \
        frame = newFrame; \
        reader = &frame->reader; \
        clazz = frame->method->clazz; \
        cp = &frame->method->clazz->cp; \
        ostack = frame->ostack; \
        lvars = frame->lvars; \
        _this = frame->method->isStatic() ? (jref) clazz : (jref) lvars[0]; \
        TRACE("executing frame: %s\n", frame->toString().c_str()); \
    } while (false)

    static void *labels[] = {
        &&nop,

        // Constants [0x01 ... 0x14]
        &&opc_aconst_null,
        &&opc_iconst_m1, &&opc_iconst_0, &&opc_iconst_1, &&opc_iconst_2, &&opc_iconst_3, &&opc_iconst_4, &&opc_iconst_5,
        &&opc_lconst_0,  &&opc_lconst_1,
        &&opc_fconst_0,  &&opc_fconst_1, &&opc_fconst_2,
        &&opc_dconst_0,  &&opc_dconst_1,
        &&opc_bipush,    &&opc_sipush,
        &&opc_ldc, &&opc_ldc_w, &&opc_ldc2_w,

        // Loads [0x15 ... 0x35]
        &&opc_iload,   &&opc_lload,   &&opc_fload,   &&opc_dload, &&opc_aload,
        &&opc_iload_0, &&opc_iload_1, &&opc_iload_2, &&opc_iload_3,
        &&opc_lload_0, &&opc_lload_1, &&opc_lload_2, &&opc_lload_3,
        &&opc_fload_0, &&opc_fload_1, &&opc_fload_2, &&opc_fload_3,
        &&opc_dload_0, &&opc_dload_1, &&opc_dload_2, &&opc_dload_3,
        &&opc_aload_0, &&opc_aload_1, &&opc_aload_2, &&opc_aload_3,
        &&opc_iaload,  &&opc_laload,  &&opc_faload,  &&opc_daload, &&opc_aaload,
        &&opc_baload,  &&opc_caload,  &&opc_saload,

        // Stores [0x36 ... 0x56]
        &&opc_istore,   &&opc_lstore,   &&opc_fstore,   &&opc_dstore, &&opc_astore,
        &&opc_istore_0, &&opc_istore_1, &&opc_istore_2, &&opc_istore_3,
        &&opc_lstore_0, &&opc_lstore_1, &&opc_lstore_2, &&opc_lstore_3,
        &&opc_fstore_0, &&opc_fstore_1, &&opc_fstore_2, &&opc_fstore_3,
        &&opc_dstore_0, &&opc_dstore_1, &&opc_dstore_2, &&opc_dstore_3,
        &&opc_astore_0, &&opc_astore_1, &&opc_astore_2, &&opc_astore_3,
        &&opc_iastore,  &&opc_lastore,  &&opc_fastore,  &&opc_dastore, &&opc_aastore,
        &&opc_bastore,  &&opc_castore,  &&opc_sastore,

        // Stack [0x57 ... 0x5f]
        &&opc_pop, &&opc_pop2,
        &&opc_dup, &&opc_dup_x1, &&opc_dup_x2, &&opc_dup2, &&opc_dup2_x1, &&opc_dup2_x2,
        &&opc_swap,

        // Math [0x60 ... 0x84]
        &&opc_iadd, &&opc_ladd, &&opc_fadd, &&opc_dadd,
        &&opc_isub, &&opc_lsub, &&opc_fsub, &&opc_dsub,
        &&opc_imul, &&opc_lmul, &&opc_fmul, &&opc_dmul,
        &&opc_idiv, &&opc_ldiv, &&opc_fdiv, &&opc_ddiv,
        &&opc_irem, &&opc_lrem, &&opc_frem, &&opc_drem,
        &&opc_ineg, &&opc_lneg, &&opc_fneg, &&opc_dneg,
        &&opc_ishl, &&opc_lshl, &&opc_ishr, &&opc_lshr, &&opc_iushr, &&opc_lushr,
        &&opc_iand, &&opc_land, &&opc_ior,  &&opc_lor,  &&opc_ixor,  &&opc_lxor, &&opc_iinc,

        // Conversions [0x85 ... 0x93]
        &&opc_i2l, &&opc_i2f, &&opc_i2d,
        &&opc_l2i, &&opc_l2f, &&opc_l2d,
        &&opc_f2i, &&opc_f2l, &&opc_f2d,
        &&opc_d2i, &&opc_d2l, &&opc_d2f,
        &&opc_i2b, &&opc_i2c, &&opc_i2s,

        // Comparisons [0x94 ... 0xa6]
        &&opc_lcmp, &&opc_fcmpl, &&opc_fcmpg, &&opc_dcmpl, &&opc_dcmpg,
        &&opc_ifeq, &&opc_ifne,  &&opc_iflt,  &&opc_ifge,  &&opc_ifgt, &&opc_ifle,
        &&opc_if_icmpeq, &&opc_if_icmpne, &&opc_if_icmplt, &&opc_if_icmpge, &&opc_if_icmpgt, &&opc_if_icmple,
        &&opc_if_acmpeq, &&opc_if_acmpne,

        // Control [0xa7 ... 0xb1]
        &&opc_goto, &&opc_jsr, &&opc_ret, &&opc_tableswitch, &&opc_lookupswitch,
        &&opc_ireturn, &&opc_lreturn, &&opc_freturn, &&opc_dreturn, &&opc_areturn, &&opc_return,

        // References [0xb2 ... 0xc3]
        &&opc_getstatic, &&opc_putstatic, &&opc_getfield, &&opc_putfield,
        &&opc_invokevirtual, &&opc_invokespecial, &&opc_invokestatic,&&opc_invokeinterface, &&opc_invokedynamic,
        &&opc_new,&&opc_newarray, &&opc_anewarray, &&opc_arraylength,
        &&opc_athrow, &&opc_checkcast, &&opc_instanceof, &&opc_monitorenter, &&opc_monitorexit,

        // Extended [0xc4 ... 0xc9]
        &&opc_wide, &&opc_multianewarray, &&opc_ifnull, &&opc_ifnonnull, &&opc_goto_w, &&opc_jsr_w,

        // Reserved [0xca ... 0xff]
        &&opc_breakpoint,
        &&opc_ldc_quick, &&opc_ldc_w_quick, &&opc_getfield_quick, &&opc_getfield2_quick, &&opc_invokestatic_quick, &&opc_invokesuper_quick, &&opc_invokenonvirtual_quick,
        &&opc_notused, &&opc_notused, &&opc_notused, &&opc_notused, &&opc_notused, &&opc_notused, &&opc_notused,
        &&opc_notused, &&opc_notused, &&opc_notused, &&opc_notused, &&opc_notused, &&opc_notused, &&opc_notused,
        &&opc_notused, &&opc_notused, &&opc_notused, &&opc_notused, &&opc_notused, &&opc_notused, &&opc_notused,
        &&opc_notused, &&opc_notused, &&opc_notused, &&opc_notused, &&opc_notused, &&opc_notused, &&opc_notused,
        &&opc_notused, &&opc_notused, &&opc_notused, &&opc_notused, &&opc_notused, &&opc_notused, &&opc_notused,
        &&opc_notused, &&opc_notused, &&opc_notused, &&opc_notused, &&opc_notused, &&opc_notused, &&opc_notused,
        &&opc_notused, &&opc_notused, &&opc_invokenative, &&opc_impdep2
    };

#if TRACE_INTERPRETER
#define DISPATCH \
{ \
    u1 opcode = reader->readu1(); \
    TRACE("%d(0x%x), %s, pc = %lu\n", opcode, opcode, instruction_names[opcode], frame->reader.pc); \
    goto *labels[opcode]; \
}
#else
#define DISPATCH goto *labels[reader->readu1()];
#endif

nop:
    DISPATCH
opc_aconst_null:
    *frame->ostack++ = (slot_t) jnull;
    DISPATCH
opc_iconst_m1:
    *frame->ostack++ = -1;
    DISPATCH
opc_iconst_0:
    *frame->ostack++ = 0;
    DISPATCH
opc_iconst_1:
    *frame->ostack++ = 1;
    DISPATCH
opc_iconst_2:
    *frame->ostack++ = 2;
    DISPATCH
opc_iconst_3:
    *frame->ostack++ = 3;
    DISPATCH
opc_iconst_4:
    *frame->ostack++ = 4;
    DISPATCH
opc_iconst_5:
    *frame->ostack++ = 5;
    DISPATCH

opc_lconst_0:
    frame->pushl(0);
    DISPATCH
opc_lconst_1:
    frame->pushl(1);
    DISPATCH

opc_fconst_0:
    *frame->ostack++ = 0;
    DISPATCH
opc_fconst_1:
    *((jfloat*) frame->ostack) = (jfloat) 1.0;
    frame->ostack++;
    DISPATCH
opc_fconst_2:
    *((jfloat*) frame->ostack) = (jfloat) 2.0;
    frame->ostack++;
    DISPATCH

opc_dconst_0:
    frame->pushd(0);
    DISPATCH
opc_dconst_1:
    frame->pushd(1);
    DISPATCH

opc_bipush: // Byte Integer push
    frame->pushi(reader->readu1());
    DISPATCH
opc_sipush: // Short Integer push
    frame->pushi(reader->readu2());
    DISPATCH

{
    u2 index;
opc_ldc:
    index = reader->readu1();
#if USE_QUICK_INSTRUCTIONS
    reader->setu1(-2, OPC_LDC_QUICK);
#endif
    goto __ldc;
opc_ldc_w:
    index = reader->readu2();
#if USE_QUICK_INSTRUCTIONS
    reader->setu1(-3, OPC_LDC_W_QUICK);
#endif
__ldc:
    u1 type = cp->type(index);

    switch (type) {
        case CONSTANT_Integer:
            frame->pushi(cp->_int(index));
            break;
        case CONSTANT_Float:
            frame->pushf(cp->_float(index));
            break;
        case CONSTANT_String:
        case CONSTANT_ResolvedString:
            frame->pushr(cp->resolveString(index));
            break;
        case CONSTANT_Class:
        case CONSTANT_ResolvedClass:
            frame->pushr(cp->resolveClass(index));
            break;
        default:
            thread_throw(new UnknownError(NEW_MSG("unknown type: %d", type)));
            break;
    }
    DISPATCH
    
opc_ldc_quick:
    index = reader->readu1();
    *frame->ostack++ = cp->info(index);
    DISPATCH
opc_ldc_w_quick:
    index = reader->readu2();
    *frame->ostack++ = cp->info(index);
    DISPATCH
}
opc_ldc2_w: {
    u2 index = reader->readu2();
    u1 type = cp->type(index);
    switch (type) {
        case CONSTANT_Long:
            frame->pushl(cp->_long(index));
            break;
        case CONSTANT_Double:
            frame->pushd(cp->_double(index));
            break;
        default:
            thread_throw(new UnknownError(NEW_MSG("unknown type: %d", type)));
            break;
    }
    DISPATCH
}
opc_iload:
opc_fload:
opc_aload: {
    u1 index = reader->readu1();
    *frame->ostack++ = lvars[index];
    DISPATCH
}
opc_lload:
opc_dload: {
    u1 index = reader->readu1();
    *frame->ostack++ = lvars[index];
    *frame->ostack++ = lvars[index + 1];
    DISPATCH
}
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

#define GET_AND_CHECK_ARRAY \
    jint index = frame->popi(); \
    auto arr = (Array *) frame->popr(); \
    if ((arr) == jnull) \
        thread_throw(new NullPointerException); \
    if (!arr->checkBounds(index)) \
        thread_throw(new ArrayIndexOutOfBoundsException);

#define ARRAY_LOAD_CATEGORY_ONE(type) \
{ \
    GET_AND_CHECK_ARRAY \
    *frame->ostack++ = (slot_t) arr->get<type>(index); \
    DISPATCH \
}
opc_iaload:
    ARRAY_LOAD_CATEGORY_ONE(jint);
opc_faload:
    ARRAY_LOAD_CATEGORY_ONE(jfloat);
opc_aaload:
    ARRAY_LOAD_CATEGORY_ONE(jref);
opc_baload:
    ARRAY_LOAD_CATEGORY_ONE(jbyte);
opc_caload:
    ARRAY_LOAD_CATEGORY_ONE(jchar);
opc_saload:
    ARRAY_LOAD_CATEGORY_ONE(jshort);

#undef ARRAY_LOAD_CATEGORY_ONE

opc_laload:
opc_daload: {
    GET_AND_CHECK_ARRAY
    auto value = (slot_t *) arr->index(index);
    *frame->ostack++ = value[0];
    *frame->ostack++ = value[1];
    DISPATCH
}
opc_istore:
opc_fstore:
opc_astore: {
    u1 index = reader->readu1();
    lvars[index] = *--frame->ostack;
    DISPATCH
}
opc_lstore:
opc_dstore: {
    u1 index = reader->readu1();
    lvars[index + 1] = *--frame->ostack;
    lvars[index] = *--frame->ostack;
    DISPATCH
}
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

#define ARRAY_STORE_CATEGORY_ONE(type) \
{ \
    auto value = (type) *--frame->ostack; \
    GET_AND_CHECK_ARRAY \
    arr->set(index, value); \
    DISPATCH \
}
opc_iastore:
    ARRAY_STORE_CATEGORY_ONE(jint);
opc_fastore:
    ARRAY_STORE_CATEGORY_ONE(jfloat);
opc_aastore:
    ARRAY_STORE_CATEGORY_ONE(jref);
opc_bastore:
    ARRAY_STORE_CATEGORY_ONE(jbyte);
opc_castore:
    ARRAY_STORE_CATEGORY_ONE(jchar);
opc_sastore:
    ARRAY_STORE_CATEGORY_ONE(jshort);

#undef ARRAY_STORE_CATEGORY_ONE

opc_lastore:
opc_dastore: {
    frame->ostack -= 2;
    slot_t *value = frame->ostack;
    GET_AND_CHECK_ARRAY
    memcpy(arr->index(index), value, sizeof(slot_t) * 2);
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

#define BINARY_OP(type, n, oper) \
{ \
    frame->ostack -= (n);\
    ((type *) frame->ostack)[-1] = ((type *) frame->ostack)[-1] oper ((type *) frame->ostack)[0]; \
    DISPATCH \
}

opc_iadd:
    BINARY_OP(jint, 1, +);
opc_ladd:
    BINARY_OP(jlong, 2, +);
opc_fadd:
    BINARY_OP(jfloat, 1, +);
opc_dadd:
    BINARY_OP(jdouble, 2, +);

opc_isub:
    BINARY_OP(jint, 1, -);
opc_lsub:
    BINARY_OP(jlong, 2, -);
opc_fsub:
    BINARY_OP(jfloat, 1, -);
opc_dsub:
    BINARY_OP(jdouble, 2, -);

opc_imul:
    BINARY_OP(jint, 1, *);
opc_lmul:
    BINARY_OP(jlong, 2, *);
opc_fmul:
    BINARY_OP(jfloat, 1, *);
opc_dmul:
    BINARY_OP(jdouble, 2, *);

opc_idiv:
    BINARY_OP(jint, 1, /);
opc_ldiv:
    BINARY_OP(jlong, 2, /);
opc_fdiv:
    BINARY_OP(jfloat, 1, /);
opc_ddiv:
    BINARY_OP(jdouble, 2, /);

opc_irem:
    BINARY_OP(jint, 1, %);
opc_lrem:
    BINARY_OP(jlong, 2, %);

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
    BINARY_OP(jint, 1, &);
opc_land:
    BINARY_OP(jlong, 2, &);
opc_ior:
    BINARY_OP(jint, 1, |);
opc_lor:
    BINARY_OP(jlong, 2, |);
opc_ixor:
    BINARY_OP(jint, 1, ^);
opc_lxor:
    BINARY_OP(jlong, 2, ^);

#undef BINARY_OP

opc_iinc: {
    u1 index = reader->readu1();
    ISLOT(lvars + index) = ISLOT(lvars + index) + reader->reads1();
    DISPATCH
}
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
{ \
    type v2 = frame->pop##t(); \
    type v1 = frame->pop##t(); \
    frame->pushi(cmp_result); \
    DISPATCH \
}

opc_lcmp:
    CMP(jlong, l, DO_CMP(v1, v2, -1));
opc_fcmpl:
    CMP(jfloat, f, DO_CMP(v1, v2, -1));
opc_fcmpg:
    CMP(jfloat, f, DO_CMP(v1, v2, 1));
opc_dcmpl:
    CMP(jdouble, d, DO_CMP(v1, v2, -1));
opc_dcmpg:
    CMP(jdouble, d, DO_CMP(v1, v2, 1));

#define IF_COND(cond) \
{ \
    jint v = frame->popi(); \
    jint offset = reader->reads2(); \
    if (v cond 0) \
        reader->skip(offset - 3);  /* minus instruction length */ \
    DISPATCH \
}
opc_ifeq:
    IF_COND(==);
opc_ifne:
    IF_COND(!=);
opc_iflt:
    IF_COND(<);
opc_ifge:
    IF_COND(>=);
opc_ifgt:
    IF_COND(>);
opc_ifle:
    IF_COND(<=);

#undef IF_COND

#define IF_CMP_COND(cond) \
{ \
    frame->ostack -= 2;\
    jint offset = reader->reads2(); \
    if (frame->ostack[0] cond frame->ostack[1]) \
        reader->skip(offset - 3); /* minus instruction length */ \
    DISPATCH \
}

opc_if_icmpeq:
opc_if_acmpeq:
    IF_CMP_COND(==);
opc_if_icmpne:
opc_if_acmpne:
    IF_CMP_COND(!=);
opc_if_icmplt:
    IF_CMP_COND(<);
opc_if_icmpge:
    IF_CMP_COND(>=);
opc_if_icmpgt:
    IF_CMP_COND(>);
opc_if_icmple:
    IF_CMP_COND(<=);

#undef IF_CMP_COND

opc_goto: {
    s2 offset = reader->reads2();
    reader->skip(offset - 3);  // minus instruction length
    DISPATCH
}

// 在Java 6之前，Oracle的Java编译器使用 jsr, jsr_w 和 ret 指令来实现 finally 子句。
// 从Java 6开始，已经不再使用这些指令
opc_jsr:
    thread_throw(new InternalError("jsr doesn't support after jdk 6."));
opc_ret:
    thread_throw(new InternalError("ret doesn't support after jdk 6."));

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
    jint index = frame->popi();
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

{
    int ret_value_slots_count;
opc_ireturn:
opc_freturn:
opc_areturn:
    ret_value_slots_count = 1;
    goto __method_return;
opc_lreturn:
opc_dreturn:
    ret_value_slots_count = 2;
    goto __method_return;
opc_return:
    ret_value_slots_count = 0;
__method_return:
    TRACE("will return: %s\n", frame->toString().c_str());
    thread->popFrame();
    Frame *invokeFrame = thread->getTopFrame();
    TRACE("invoke frame: %s\n", invokeFrame == nullptr ? "NULL" : invokeFrame->toString().c_str());
    frame->ostack -= ret_value_slots_count;
    slot_t *ret_value = frame->ostack;
    if (frame->vm_invoke || invokeFrame == nullptr) {
        if (frame->method->isSynchronized()) {
            _this->unlock();
        }
        return ret_value;
    }

    for (int i = 0; i < ret_value_slots_count; i++) {
        *invokeFrame->ostack++ = *ret_value++;
    }
    if (frame->method->isSynchronized()) {
        _this->unlock();
    }
    CHANGE_FRAME(invokeFrame);
    DISPATCH
}
opc_getstatic: {
    u2 index = reader->readu2();
    Field *field = cp->resolveField(index);
    assert(field->isStatic()); // todo

    initClass(field->clazz);

    *frame->ostack++ = field->staticValue.data[0];
    if (field->categoryTwo) {
        *frame->ostack++ = field->staticValue.data[1];
    }
    DISPATCH
}
opc_putstatic: {
    u2 index = reader->readu2();
    Field *field = cp->resolveField(index);
    assert(field->isStatic()); // todo

    initClass(field->clazz);

    if (field->categoryTwo) {
        frame->ostack -= 2;
        field->staticValue.data[0] = frame->ostack[0];
        field->staticValue.data[1] = frame->ostack[1];
    } else {
        field->staticValue.data[0] = *--frame->ostack;
    }

    DISPATCH
}
opc_getfield: {
    u2 index = reader->readu2();
    Field *field = cp->resolveField(index);
    assert(!field->isStatic()); // todo

    jref obj = frame->popr();
    if (obj == jnull) {
        thread_throw(new NullPointerException);
    }

    *frame->ostack++ = obj->data[field->id];
    if (field->categoryTwo) {
        *frame->ostack++ = obj->data[field->id + 1];
    }
#if USE_QUICK_INSTRUCTIONS
    if (field->id < U2_MAX) {
        if (!field->categoryTwo)
            reader->setu1(-3, OPC_GETFIELD_QUICK);
        else
            reader->setu1(-3, OPC_GETFIELD2_QUICK);
        // 用 field->id 替换掉 index
        reader->setu2(-2, (u2)(field->id));
    }
#endif
    DISPATCH
}
opc_getfield_quick: {
    u2 filed_id = reader->readu2();
    jref obj = frame->popr();
    if (obj == jnull) {
        thread_throw(new NullPointerException);
    }

    *frame->ostack++ = obj->data[filed_id];
    DISPATCH
}
opc_getfield2_quick: {
    u2 filed_id = reader->readu2();
    jref obj = frame->popr();
    if (obj == jnull) {
        thread_throw(new NullPointerException);
    }

    *frame->ostack++ = obj->data[filed_id];
    *frame->ostack++ = obj->data[filed_id + 1];
    DISPATCH
}
opc_putfield: {
    u2 index = reader->readu2();
    Field *field = cp->resolveField(index);
    assert(!field->isStatic()); // todo

    // 如果是final字段，则只能在构造函数中初始化，否则抛出java.lang.IllegalAccessError。
    if (field->isFinal()) {
        // todo
        if (!clazz->equals(field->clazz) || !equals(frame->method->name, S(object_init))) {
            thread_throw(new IllegalAccessError());
        }
    }

    if (field->categoryTwo) {
        frame->ostack -= 2;
    } else {
        frame->ostack--;
    }
    slot_t *value = frame->ostack;

    jref obj = frame->popr();
    if (obj == jnull) {
        thread_throw(new NullPointerException);
    }

    obj->setFieldValue(field, value);
    DISPATCH
}
opc_invokevirtual: {
    // invokevirtual指令用于调用对象的实例方法，根据对象的实际类型进行分派（虚方法分派）。
    u2 index = reader->readu2();
    Method *m = cp->resolveMethod(index);

    frame->ostack -= m->arg_slot_count;
    auto obj = (jref) frame->ostack[0];
    if (obj == jnull) {
        thread_throw(new NullPointerException);
    }

    assert(m->vtableIndex >= 0);
    assert(m->vtableIndex < obj->clazz->vtable.size());
    resolved_method = obj->clazz->vtable[m->vtableIndex];
    assert(resolved_method == obj->clazz->lookupMethod(m->name, m->descriptor));

    TRACE("obj: %p, %s\n", obj, resolved_method->toString().c_str());
    goto __invoke_method;
}
opc_invokespecial: {
    // invokespecial指令用于调用一些需要特殊处理的实例方法，
    // 包括构造函数、私有方法和通过super关键字调用的超类方法。
    u2 index = reader->readu2();
    Method *m = cp->resolveMethod(index);

    /*
     * 如果调用的中超类中的函数，但不是构造函数，不是private 函数，且当前类的ACC_SUPER标志被设置，
     * 需要一个额外的过程查找最终要调用的方法；否则前面从方法符号引用中解析出来的方法就是要调用的方法。
     * todo 详细说明
     */
    if (m->clazz->isSuper()
        && !m->isPrivate()
        && clazz->isSubclassOf(m->clazz) // todo
        && !utf8::equals(m->name, S(object_init))) {
        m = clazz->superClass->lookupMethod(m->name, m->descriptor);
#if USE_QUICK_INSTRUCTIONS
        reader->setu1(-3, OPC_INVOKESUPER_QUICK);
#endif
    } else {
#if USE_QUICK_INSTRUCTIONS
        reader->setu1(-3, OPC_INVOKENONVIRTUAL_QUICK);
#endif
    }

    if (m->isAbstract()) {
        thread_throw(new AbstractMethodError());
    }
    if (m->isStatic()) {
        thread_throw(new IncompatibleClassChangeError());
    }

    frame->ostack -= m->arg_slot_count;
    auto obj = (jref) frame->ostack[0];
    if (obj == jnull) {
        thread_throw(new NullPointerException);
    }

    resolved_method = m;
    goto __invoke_method;
}
opc_invokesuper_quick: {
    u2 index = reader->readu2();
    auto m = (Method *) cp->info(index);
    resolved_method = clazz->superClass->lookupMethod(m->name, m->descriptor);
    frame->ostack -= resolved_method->arg_slot_count;
    auto obj = (jref) frame->ostack[0];
    if (obj == jnull) {
        thread_throw(new NullPointerException);
    }
    goto __invoke_method;
}
opc_invokenonvirtual_quick: {
    u2 index = reader->readu2();
    resolved_method = (Method *) cp->info(index);
    frame->ostack -= resolved_method->arg_slot_count;
    auto obj = (jref) frame->ostack[0];
    if (obj == jnull) {
        thread_throw(new NullPointerException);
    }
    goto __invoke_method;
}
opc_invokestatic: {
    // invokestatic指令用来调用静态方法。
    // 如果类还没有被初始化，会触发类的初始化。
    u2 index = reader->readu2();
    Method *m = cp->resolveMethod(index);
    if (m->isAbstract()) {
        thread_throw(new AbstractMethodError());
    }
    if (!m->isStatic()) {
        thread_throw(new IncompatibleClassChangeError());
    }

    initClass(m->clazz);

    frame->ostack -= m->arg_slot_count;
    resolved_method = m;
#if USE_QUICK_INSTRUCTIONS
    reader->setu1(-3, OPC_INVOKESTATIC_QUICK);
#endif
    goto __invoke_method;
}
opc_invokestatic_quick: {
    u2 index = reader->readu2();
    resolved_method = (Method *) cp->info(index);
    frame->ostack -= resolved_method->arg_slot_count;
    goto __invoke_method;
}
opc_invokeinterface: {
    u2 index = reader->readu2();

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
    auto obj = (jref) frame->ostack[0];
    if (obj == jnull) {
        thread_throw(new NullPointerException);
    }

    Method *method = obj->clazz->lookupMethod(m->name, m->descriptor);
    if (method->isAbstract()) {
        thread_throw(new AbstractMethodError());
    }

    if (!method->isPublic()) {
        thread_throw(new IllegalAccessError());
    }

    resolved_method = method;
    goto __invoke_method;
}
opc_invokedynamic: {
    u2 index = reader->readu2(); // point to CONSTANT_InvokeDynamic_info
    reader->readu1(); // this byte must always be zero.
    reader->readu1(); // this byte must always be zero.

    const utf8_t *invokedName = cp->invokeDynamicMethodName(index);
    const utf8_t *invokedDescriptor = cp->invokeDynamicMethodType(index);

    auto invokedType = fromMethodDescriptor(invokedDescriptor, clazz->loader);
    auto caller = getCaller();

    Class::BootstrapMethod &bm = clazz->bootstrapMethods.at(cp->invokeDynamicBootstrapMethodIndex(index));
    u2 refKind = cp->methodHandleReferenceKind(bm.bootstrapMethodRef);
    u2 refIndex = cp->methodHandleReferenceIndex(bm.bootstrapMethodRef);

    switch (refKind) {
        case REF_invokeStatic: {
            const utf8_t *className = cp->methodClassName(refIndex);
            Class *bootstrapClass = loadClass(clazz->loader, className);

            // bootstrap method is static,  todo 对不对
            // 前三个参数固定为 MethodHandles.Lookup caller, String invokedName, MethodType invokedType todo 对不对
            // 后续的参数由 ref->argc and ref->args 决定
            Method *bootstrapMethod
                    = bootstrapClass->getDeclaredStaticMethod(cp->methodName(refIndex), cp->methodType(refIndex));
            // args's length is big enough,多余的长度无所谓，bootstrapMethod 会按需读取的。
            slot_t args[3 + bm.bootstrapArguments.size() * 2] = {
                    (slot_t) caller, (slot_t) newString(invokedName), (slot_t) invokedType };
            bm.resolveArgs(*cp, args + 3);
            auto callSet = (jref) *execJavaFunc(bootstrapMethod, args);

            // public abstract MethodHandle dynamicInvoker()
            auto dynInvoker = callSet->clazz->lookupInstMethod("dynamicInvoker", "()Ljava/lang/invoke/MethodHandle;");
            auto exactMethodHandle = (jref) *execJavaFunc(dynInvoker, callSet);

            // public final Object invokeExact(Object... args) throws Throwable
            Method *invokeExact = exactMethodHandle->clazz->lookupInstMethod(
                    "invokeExact", "([Ljava/lang/Object;)Ljava/lang/Object;");
            assert(invokeExact->isVarargs());
            u2 slotsCount = Method::calArgsSlotsCount(invokedDescriptor, true);
            slot_t __args[slotsCount];
            __args[0] = (slot_t) exactMethodHandle;
            slotsCount--; // 减去"this"
            frame->ostack -= slotsCount; // pop all args
            memcpy(__args + 1, frame->ostack, slotsCount * sizeof(slot_t));
            // invoke exact method, invokedynamic completely execute over.
            execJavaFunc(invokeExact, __args);

            DISPATCH
        }
        case REF_newInvokeSpecial:
            jvm_abort(" "); // todo
            DISPATCH
        default:
            jvm_abort("never goes here"); // todo never goes here
            break;
    }
    jvm_abort("never goes here"); // todo
}

__invoke_method: {
    assert(resolved_method);
    Frame *newFrame = thread->allocFrame(resolved_method, false);
    TRACE("Alloc new frame: %s\n", newFrame->toString().c_str());

    newFrame->lvars = frame->ostack;
    CHANGE_FRAME(newFrame);    
    if (resolved_method->isSynchronized()) {
        _this->unlock();
    }
    DISPATCH
}
opc_new: {
    // new指令专门用来创建类实例。数组由专门的指令创建
    // 如果类还没有被初始化，会触发类的初始化。
    Class *c = cp->resolveClass(reader->readu2());
    initClass(c);

    if (c->isInterface() || c->isAbstract()) {
        thread_throw(new InstantiationException());
    }

    frame->pushr(newObject(c));
    DISPATCH
}
opc_newarray: {
    // 创建一维基本类型数组。包括 boolean[], byte[], char[], short[], int[], long[], float[] 和 double[] 8种。
    jint arrLen = frame->popi();
    if (arrLen < 0) {
        thread_throw(new NegativeArraySizeException);
    }

    int arrType = reader->readu1();
    const char *arrClassName;
    switch (arrType) {
        case AT_BOOLEAN: arrClassName = "[Z"; break;
        case AT_CHAR:    arrClassName = "[C"; break;
        case AT_FLOAT:   arrClassName = "[F"; break;
        case AT_DOUBLE:  arrClassName = "[D"; break;
        case AT_BYTE:    arrClassName = "[B"; break;
        case AT_SHORT:   arrClassName = "[S"; break;
        case AT_INT:     arrClassName = "[I"; break;
        case AT_LONG:    arrClassName = "[J"; break;
        default:
            thread_throw(new UnknownError(NEW_MSG("error. Invalid array type: %d\n", arrType)));
    }

    auto c = loadArrayClass(arrClassName);
    frame->pushr(newArray(c, arrLen));

    DISPATCH
}
opc_anewarray: {
    // 创建一维引用类型数组
    jint arrLen = frame->popi();
    if (arrLen < 0) {
        thread_throw(new ArrayIndexOutOfBoundsException);
    }

    u2 index = reader->readu2();
    auto ac = cp->resolveClass(index)->arrayClass();
    frame->pushr(newArray(ac, arrLen));

    DISPATCH
}
opc_multianewarray: {
    /*
     * 创建多维数组
     * todo 注意这种情况，基本类型的多维数组 int[][][]
     */
    auto index = reader->readu2();
    Class *ac = cp->resolveClass(index);

    auto dim = reader->readu1(); // 多维数组的维度
    if (dim < 1) { // 必须大于或等于1
        // todo error
        jvm_abort("dim < 1");
    }

    jint lens[dim];
    for (int i = 0; i < dim; i++) {
        lens[i] = frame->popi();
    }
    frame->pushr(newMultiArray(ac, dim, lens));

    DISPATCH
}
opc_arraylength: {
    Object *o = frame->popr();
    if (o == jnull) {
        thread_throw(new NullPointerException);
    }
    if (!o->isArrayObject()) {
        thread_throw(new UnknownError("not a array")); // todo
    }
    frame->pushi(((Array *) o)->len);
    DISPATCH
}
opc_athrow: {
    jref eo = frame->popr(); // exception object
    if (eo == jnull) {
        thread_throw(new NullPointerException);
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
            DISPATCH
        }

        if (frame->vm_invoke) {
            // frame 由虚拟机调用，则将异常抛给虚拟机
            throw Throwable(eo);
        }

        // frame 无法处理异常，弹出
        thread->popFrame();

        if (frame->prev == nullptr) {
            // 虚拟机栈已空，还是无法处理异常
            TRACE("uncaught exception: %s\n", eo->toString().c_str());
            thread_uncaught_exception(eo);
        }

        TRACE("athrow: pop frame: %s\n", frame->toString().c_str());
        CHANGE_FRAME(frame->prev);
    }
}
opc_checkcast: {
    jref obj = RSLOT(frame->ostack - 1); // 不改变操作数栈
    u2 index = reader->readu2();

    // 如果引用是null，则指令执行结束。也就是说，null 引用可以转换成任何类型
    if (obj != jnull) {
        Class *c = cp->resolveClass(index);
        if (!obj->isInstanceOf(c)) {
//            thread_throw(new ClassCastException(obj->clazz->className, c->className));
            thread_throw(new ClassCastException());
        }
    }
    DISPATCH
}
opc_instanceof: {
    u2 index = reader->readu2();
    Class *c = cp->resolveClass(index);

    jref obj = frame->popr();
    if (obj == jnull)
        frame->pushi(0);
    else
        frame->pushi(obj->isInstanceOf(c) ? 1 : 0);
    DISPATCH
}
opc_monitorenter: {
    jref o = frame->popr();
    if (o == jnull) {
        thread_throw(new NullPointerException);
    }
    o->lock();
    DISPATCH
}
opc_monitorexit: {
    jref o = frame->popr();
    if (o == jnull) {
        thread_throw(new NullPointerException);
    }
    o->unlock();
    DISPATCH
}
opc_wide: {
    int __opcode = reader->readu1();
    TRACE("%d(0x%x), %s, pc = %lu\n", __opcode, __opcode, instruction_names[__opcode], frame->reader.pc);
    u2 index = reader->readu2();
    switch (__opcode) {
        case OPC_ILOAD:
        case OPC_FLOAD:
        case OPC_ALOAD:
            *frame->ostack++ = lvars[index];
            break;
        case OPC_LLOAD:
        case OPC_DLOAD:
            *frame->ostack++ = lvars[index];
            *frame->ostack++ = lvars[index + 1];
            break;
        case OPC_ISTORE:
        case OPC_FSTORE:
        case OPC_ASTORE:
            lvars[index] = *--frame->ostack;
            break;
        case OPC_LSTORE:
        case OPC_DSTORE:
            lvars[index + 1] = *--frame->ostack;
            lvars[index] = *--frame->ostack;
            break;
        case OPC_RET:
            thread_throw(new InternalError("ret doesn't support after jdk 6."));
            break;
        case OPC_IINC:
            ISLOT(lvars + index) = ISLOT(lvars + index) + reader->readu2();
            break;
        default:
            thread_throw(new UnknownError("never goes here."));
            break;
    }
    DISPATCH
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
opc_goto_w: // todo
    thread_throw(new InternalError("goto_w doesn't support"));
    DISPATCH
opc_jsr_w: // todo
    thread_throw(new InternalError("jsr_w doesn't support after jdk 6."));
    DISPATCH
opc_breakpoint: // todo
    thread_throw(new InternalError("breakpoint doesn't support in this jvm."));
    DISPATCH
opc_notused:
    jvm_abort("This instruction isn't used.\n"); // todo
    DISPATCH
opc_invokenative: {
    TRACE("%s\n", frame->toString().c_str());
    if (frame->method->nativeMethod == nullptr){ // todo 
        jvm_abort("not find native method: %s\n", frame->method->toString().c_str());
    }

    // todo 不需要则这里做任何同步的操作

    assert(frame->method->nativeMethod != nullptr);
    try {
        frame->method->nativeMethod(frame);
    } catch (Throwable &t) {
        TRACE("native method throw a exception\n");
        assert(t.getJavaThrowable() != nullptr);
        frame->pushr(t.getJavaThrowable());
//        if (frame->method->isSynchronized()) {
//            _this->unlock();
//        }
        goto opc_athrow;
    } catch (...) {
        TRACE("error, native method 不应该抛出除Java Exception以外的其他异常");
//        if (frame->method->isSynchronized()) {
//            _this->unlock();
//        }
        throw;
    }

//    if (frame->method->isSynchronized()) {
//        _this->unlock();
//    }
    DISPATCH
}
opc_impdep2:
    jvm_abort("This instruction isn't used.\n"); // todo
    DISPATCH
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

    try {
        return exec();
    } catch(Throwable &e) {
        // todo
        thread_throw(&e);
    }
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

slot_t *execConstructor(Method *constructor, jref _this, Array *args)
{
    assert(constructor != nullptr);
    assert(_this != nullptr);

    if (args == nullptr) {
        return execJavaFunc(constructor, _this);
    }

    // Class[]
    Array *types = constructor->getParameterTypes();
    assert(types != nullptr);
    assert(types->len == args->len);

    // 因为有 category two 的存在，result 的长度最大为 types_len * 2 + this_obj
    auto realArgs = new slot_t[2 * types->len + 1];
    int k = 0;
    realArgs[k++] = (slot_t) _this;

    for (int i = 0; i < types->len; i++) {
        auto c = types->get<Class *>(i);
        auto o = args->get<jref>(i);

        if (c->isPrimClass()) {
            const slot_t *unbox = o->unbox();
            realArgs[k++] = *unbox;
            if (strcmp(o->clazz->className, "long") == 0 || strcmp(o->clazz->className, "double") == 0) // category_two
                realArgs[k++] = *++unbox;
        } else {
            RSLOT(realArgs + k) = o;
            k++;
        }
    }

    return execJavaFunc(constructor, realArgs);
}
