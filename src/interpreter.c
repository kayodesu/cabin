#include <math.h>
#include <ffi.h>
#include "cabin.h"
#include "constants.h"
#include "interpreter.h"
#include "attributes.h"
#include "jni.h"
#include "thread.h"
#include "mh.h"
#include "meta.h"
#include "object.h"
#include "exception.h"
#include "bytecode_reader.h"


// the mapping of instructions' code and name
static const char *instruction_names[] = JVM_OPCODE_NAME_INITIALIZER;

#define PRINT_OPCODE VERBOSE("%d(0x%x), %s, pc = %d\n", \
                        opcode, opcode, instruction_names[opcode], (int)frame->reader.pc);

static unsigned char opcode_len[JVM_OPC_MAX+1] = JVM_OPCODE_LENGTH_INITIALIZER;

static void call_jni_method(Frame *frame);
static bool checkcast(Class *s, Class *t);

/*
 * 执行当前线程栈顶的frame
 */
static slot_t *exec(jref *excep)
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

    Thread *thread = get_current_thread();
    Frame *frame = thread->top_frame;
    TRACE("executing frame: %s", get_frame_info(frame));

    Method *resolved_method = NULL;
    int index;

    BytecodeReader *reader = &frame->reader;
    Class *clazz = frame->method->clazz;
    ConstantPool *cp = &frame->method->clazz->cp;
    // slot_t *ostack = frame->ostack;
    slot_t *lvars = frame->lvars;

    jref _this = IS_STATIC(frame->method) ? (jref) clazz : slot_get_ref(lvars);

#define HANDLE_EXCEPTION0(_excep) \
do { \
    assert(_excep != NULL); \
    ostack_pushr(frame, _excep); \
    goto opc_athrow; \
} while(false)  

#define CHECK_EXCEPTION_OCCURRED \
{ \
    jref _excep = exception_occurred(); \
    if (_excep != NULL) { \
        clear_exception(); \
        HANDLE_EXCEPTION0(_excep); \
    } \
}

#define HANDLE_EXCEPTION(_excep_name, _msg) \
do { \
    raise_exception(_excep_name, _msg); \
    jref _excep = exception_occurred(); \
    clear_exception(); \
    HANDLE_EXCEPTION0(_excep); \
} while(false)

#define NULL_POINTER_CHECK(ref) \
do { \
    if ((ref) == NULL) \
        HANDLE_EXCEPTION(S(java_lang_NullPointerException), NULL); \
} while(false) 

#define CHANGE_FRAME(new_frame) \
do { \
    /*frame->ostack = ostack;  stack指针在变动，需要设置一下 todo */ \
    frame = new_frame; \
    reader = &frame->reader; \
    clazz = frame->method->clazz; \
    cp = &frame->method->clazz->cp; \
    /*ostack = frame->ostack; */                                     \
    lvars = frame->lvars; \
    _this = IS_STATIC(frame->method) ? (jref) clazz : slot_get_ref(lvars); \
    TRACE("executing frame: %s", get_frame_info(frame)); \
} while(false)

    u1 opcode;
    
#define DISPATCH \
{ \
    opcode = bcr_readu1(reader); \
    PRINT_OPCODE; \
    goto *handlers[opcode]; \
}

opc_nop:
    DISPATCH
opc_aconst_null:
    ostack_pushr(frame, NULL);
    DISPATCH
opc_iconst_m1:
    ostack_pushi(frame, -1);
    DISPATCH
opc_iconst_0:
    ostack_pushi(frame, 0);
    DISPATCH
opc_iconst_1:
    ostack_pushi(frame, 1);
    DISPATCH
opc_iconst_2:
    ostack_pushi(frame, 2);
    DISPATCH
opc_iconst_3:
    ostack_pushi(frame, 3);
    DISPATCH
opc_iconst_4:
    ostack_pushi(frame, 4);
    DISPATCH
opc_iconst_5:
    ostack_pushi(frame, 5);
    DISPATCH
opc_lconst_0:
    ostack_pushl(frame, 0);
    DISPATCH
opc_lconst_1:
    ostack_pushl(frame, 1);
    DISPATCH
opc_fconst_0:
    ostack_pushf(frame, 0);
    DISPATCH
opc_fconst_1:
    ostack_pushf(frame, 1);
    DISPATCH
opc_fconst_2:
    ostack_pushf(frame, 2);
    DISPATCH
opc_dconst_0:
    ostack_pushd(frame, 0);
    DISPATCH
opc_dconst_1:
    ostack_pushd(frame, 1);
    DISPATCH
opc_bipush: // Byte Integer push
    ostack_pushi(frame, bcr_reads1(reader));
    DISPATCH
opc_sipush: // Short Integer push
    ostack_pushi(frame, bcr_reads2(reader));
    DISPATCH
opc_ldc:
    index = bcr_readu1(reader);
    goto _ldc;
opc_ldc_w:
    index = bcr_readu2(reader);
_ldc: {
    u1 type = cp_get_type(cp, index);
    switch (type) {
        case JVM_CONSTANT_Integer:
            ostack_pushi(frame, cp_get_int(cp, index));
            break;
        case JVM_CONSTANT_Float:
            ostack_pushf(frame, cp_get_float(cp, index));
            break;
        case JVM_CONSTANT_String:
        case JVM_CONSTANT_ResolvedString:
            ostack_pushr(frame, resolve_string(cp, index));
            break;
        case JVM_CONSTANT_Class:
        case JVM_CONSTANT_ResolvedClass:
            ostack_pushr(frame, resolve_class(cp, index)->java_mirror);
            break;
        default:
            HANDLE_EXCEPTION(S(java_lang_UnknownError), NULL); // todo msg
            // throw java_lang_UnknownError("unknown type: " + to_string(type));
            break;
    }
    DISPATCH
}
opc_ldc2_w: {
    index = bcr_readu2(reader);
    u1 type = cp_get_type(cp, index);
    switch (type) {
        case JVM_CONSTANT_Long:
            ostack_pushl(frame, cp_get_long(cp, index));
            break;
        case JVM_CONSTANT_Double:
//             printvm("=====    %f\n", cp->getDouble(index));
            ostack_pushd(frame, cp_get_double(cp, index));
            break;
        default:
            HANDLE_EXCEPTION(S(java_lang_UnknownError), NULL); // todo msg
            // throw java_lang_UnknownError("unknown type: " + to_string(type));
            break;
    }
    DISPATCH
}
opc_iload:
opc_fload:
opc_aload:
    index = bcr_readu1(reader);
_iload:
_fload:
_aload:
    *frame->ostack++ = lvars[index];
    DISPATCH
opc_lload:
opc_dload: 
    index = bcr_readu1(reader);
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
    
#define GET_AND_CHECK_ARRAY \
    index = ostack_popi(frame); \
    jarrRef arr = ostack_popr(frame); \
    NULL_POINTER_CHECK(arr); \
    if (!array_check_bounds(arr, index)) \
        HANDLE_EXCEPTION(S(java_lang_ArrayIndexOutOfBoundsException), NULL); /* todo msg */       
       // throw java_lang_ArrayIndexOutOfBoundsException("index is " + to_string(index));

opc_iaload: {
    GET_AND_CHECK_ARRAY
    jint value = array_get(jint, arr, index);
    ostack_pushi(frame, value);
    DISPATCH
}
opc_faload: {
    GET_AND_CHECK_ARRAY
    jfloat value = array_get(jfloat, arr, index);
    ostack_pushf(frame, value);
    DISPATCH
}
opc_aaload: {
    GET_AND_CHECK_ARRAY
    jref value = array_get(jref, arr, index);
    ostack_pushr(frame, value);
    DISPATCH
}
opc_baload: {
    GET_AND_CHECK_ARRAY
    jint value = array_get(jbyte, arr, index);
    ostack_pushi(frame, value);
    DISPATCH
}
opc_caload: {
    GET_AND_CHECK_ARRAY
    jint value = array_get(jchar, arr, index);
    ostack_pushi(frame, value);
    DISPATCH
}
opc_saload: {
    GET_AND_CHECK_ARRAY
    jint value = array_get(jshort, arr, index);
    ostack_pushi(frame, value);
    DISPATCH
}
opc_laload: {
    GET_AND_CHECK_ARRAY
    jlong value = array_get(jlong, arr, index);
    ostack_pushl(frame, value);
    DISPATCH
}
opc_daload: {
    GET_AND_CHECK_ARRAY
    jdouble value = array_get(jdouble, arr, index);
    ostack_pushd(frame, value);
    DISPATCH
}

opc_istore:
opc_fstore:
opc_astore:
    index = bcr_readu1(reader);
_istore:
_fstore:
_astore:
    lvars[index] = *--frame->ostack;
    DISPATCH
opc_lstore:
opc_dstore: 
    index = bcr_readu1(reader);
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
    jint value = ostack_popi(frame);
    GET_AND_CHECK_ARRAY
    // arr->setInt(index, value);
    array_set_int(arr, index, value);
    DISPATCH
}
opc_fastore: {
    jfloat value = ostack_popf(frame);
    GET_AND_CHECK_ARRAY
    // arr->setFloat(index, value);
    array_set_float(arr, index, value);
    DISPATCH
}
opc_aastore: {
    jref value = ostack_popr(frame);
    GET_AND_CHECK_ARRAY
    // arr->setRef(index, value);
    array_set_ref(arr, index, value);
    DISPATCH
}
opc_bastore: {
    jint value = ostack_popi(frame);
    GET_AND_CHECK_ARRAY
    if (is_byte_array_class(arr->clazz)) {
        array_set_byte(arr, index, JINT_TO_JBYTE(value));
    } else if (is_boolean_array_class(arr->clazz)) {  
        array_set_boolean(arr, index, JINT_TO_JBOOL(value));
    } else {
        SHOULD_NEVER_REACH_HERE(arr->clazz->class_name);
    }
    DISPATCH
}
opc_castore: {
    jint value = ostack_popi(frame);
    GET_AND_CHECK_ARRAY
    array_set_char(arr, index, JINT_TO_JCHAR(value));
    DISPATCH
}
opc_sastore: {
    jint value = ostack_popi(frame);
    GET_AND_CHECK_ARRAY
    array_set_short(arr, index, JINT_TO_JSHORT(value));
    DISPATCH
}
opc_lastore: {
    jlong value = ostack_popl(frame);
    GET_AND_CHECK_ARRAY
    array_set_long(arr, index, value);
    DISPATCH
}
opc_dastore: {
    jdouble value = ostack_popd(frame);
    GET_AND_CHECK_ARRAY
    array_set_double(arr, index, value);
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
opc_swap: {
    slot_t tmp = frame->ostack[-1];
    frame->ostack[-1] = frame->ostack[-2];
    frame->ostack[-2] = tmp;
    // swap(frame->ostack[-1], frame->ostack[-2]);
    DISPATCH
}
#define BINARY_OP(type, t, oper) \
do { \
    type v2 = ostack_pop##t(frame); \
    type v1 = ostack_pop##t(frame); \
    ostack_push##t(frame, v1 oper v2); \
    DISPATCH \
} while(false)

opc_iadd:
    BINARY_OP(jint, i, +);
opc_ladd:
    BINARY_OP(jlong, l, +);
opc_fadd:
    BINARY_OP(jfloat, f, +);
opc_dadd:
    BINARY_OP(jdouble, d, +);
opc_isub:
    BINARY_OP(jint, i, -);
opc_lsub:
    BINARY_OP(jlong, l, -);
opc_fsub:
    BINARY_OP(jfloat, f, -);
opc_dsub:
    BINARY_OP(jdouble, d, -);
opc_imul:
    BINARY_OP(jint, i, *);
opc_lmul:
    BINARY_OP(jlong, l, *);
opc_fmul:
    BINARY_OP(jfloat, f, *);
opc_dmul:
    BINARY_OP(jdouble, d, *);
    
#define ZERO_DIVISOR_CHECK(value) \
do { \
    if (value == 0) \
        HANDLE_EXCEPTION(S(java_lang_ArithmeticException), "division by zero"); \
} while(false)

opc_idiv:
    ZERO_DIVISOR_CHECK(slot_get_int(frame->ostack - 1));
    BINARY_OP(jint, i, /);
opc_ldiv:
    ZERO_DIVISOR_CHECK(slot_get_long(frame->ostack - 2));
    BINARY_OP(jlong, l, /);
opc_fdiv:
    ZERO_DIVISOR_CHECK(slot_get_float(frame->ostack - 1));
    BINARY_OP(jfloat, f, /);
opc_ddiv:
    ZERO_DIVISOR_CHECK(slot_get_double(frame->ostack - 2));
    BINARY_OP(jdouble, d, /);
opc_irem: 
    ZERO_DIVISOR_CHECK(slot_get_int(frame->ostack - 1));
    BINARY_OP(jint, i, %);
opc_lrem:
    ZERO_DIVISOR_CHECK(slot_get_long(frame->ostack - 2));
    BINARY_OP(jlong, l, %);
#undef ZERO_DIVISOR_CHECK

opc_frem: {
    jfloat v2 = ostack_popf(frame);
    jfloat v1 = ostack_popf(frame);
    ostack_pushf(frame, fmod(v1, v2));
    DISPATCH
}
opc_drem: {
    jdouble v2 = ostack_popd(frame);
    jdouble v1 = ostack_popd(frame);
    ostack_pushd(frame, fmod(v1, v2));
    DISPATCH
}
opc_ineg:
    ostack_pushi(frame, -ostack_popi(frame));
    DISPATCH
opc_lneg:
    ostack_pushl(frame, -ostack_popl(frame));
    DISPATCH
opc_fneg:
    ostack_pushf(frame, -ostack_popf(frame));
    DISPATCH
opc_dneg:
    ostack_pushd(frame, -ostack_popd(frame));
    DISPATCH 
opc_ishl: {
    // 与0x1f是因为低5位表示位移距离，位移距离实际上被限制在0到31之间。
    jint shift = ostack_popi(frame) & 0x1f;
    assert(0 <= shift && shift <= 31);
    jint x = ostack_popi(frame);
    ostack_pushi(frame, x << shift);
    DISPATCH
}
opc_lshl: {
    // 与0x3f是因为低6位表示位移距离，位移距离实际上被限制在0到63之间。
    jint shift = ostack_popi(frame) & 0x3f;
    assert(0 <= shift && shift <= 63);
    jlong x = ostack_popl(frame);
    ostack_pushl(frame, x << shift);
    DISPATCH
}
opc_ishr: {
    // 算术右移 shift arithmetic right
    // 带符号右移。正数右移高位补0，负数右移高位补1。
    // 对应于Java中的 >>
    jint shift = ostack_popi(frame) & 0x1f;
    assert(0 <= shift && shift <= 31);
    jint x = ostack_popi(frame);
    ostack_pushi(frame, x >> shift);
    DISPATCH
}
opc_lshr: {
    jint shift = ostack_popi(frame) & 0x3f;
    assert(0 <= shift && shift <= 63);
    jlong x = ostack_popl(frame);
    ostack_pushl(frame, x >> shift);
    DISPATCH
}
opc_iushr: {
    // 逻辑右移 shift logical right
    // 无符号右移。无论是正数还是负数，高位通通补0。
    // 对应于Java中的 >>>
    // https://stackoverflow.com/questions/5253194/implementing-logical-right-shift-in-c/
    jint shift = ostack_popi(frame) & 0x1f;
    assert(0 <= shift && shift <= 31);
    jint x = ostack_popi(frame);
    int size = sizeof(jint) * 8 - 1; // bits count
    ostack_pushi(frame, (x >> shift) & ~(((((jint)1) << size) >> shift) << 1));
    DISPATCH
}
opc_lushr: {
    jint shift = ostack_popi(frame) & 0x3f;
    assert(0 <= shift && shift <= 63);
    jlong x = ostack_popl(frame);
    int size = sizeof(jlong) * 8 - 1; // bits count
    ostack_pushl(frame, (x >> shift) & ~(((((jlong)1) << size) >> shift) << 1));
    DISPATCH
}
opc_iand:
    BINARY_OP(jint, i, &);
opc_land:
    BINARY_OP(jlong, l, &);
opc_ior:
    BINARY_OP(jint, i, |);
opc_lor:
    BINARY_OP(jlong, l, |);
opc_ixor:
    BINARY_OP(jint, i, ^);
opc_lxor:
    BINARY_OP(jlong, l, ^);
#undef BINARY_OP

opc_iinc: 
    index = bcr_readu1(reader);
    slot_set_int(lvars + index, slot_get_int(lvars + index) + bcr_reads1(reader)); 
    DISPATCH
_wide_iinc:  
    slot_set_int(lvars + index, slot_get_int(lvars + index) + bcr_reads2(reader)); 
    DISPATCH
opc_i2l:
    ostack_pushl(frame, ostack_popi(frame));
    DISPATCH
opc_i2f:
    ostack_pushf(frame, ostack_popi(frame));
    DISPATCH
opc_i2d:
    ostack_pushd(frame, ostack_popi(frame));
    DISPATCH
opc_l2i:
    ostack_pushi(frame, (jint) ostack_popl(frame));
    DISPATCH
opc_l2f:
    ostack_pushf(frame, (jfloat) ostack_popl(frame));
    DISPATCH
opc_l2d:
    ostack_pushd(frame, ostack_popl(frame));
    DISPATCH
opc_f2i:
   ostack_pushi(frame, (jint) ostack_popf(frame));
    DISPATCH
opc_f2l:
    ostack_pushl(frame, (jlong) ostack_popf(frame));
    DISPATCH
opc_f2d:
    ostack_pushd(frame, ostack_popf(frame));
    DISPATCH
opc_d2i:
    ostack_pushi(frame, (jint) ostack_popd(frame));
    DISPATCH
opc_d2l:
    ostack_pushl(frame, (jlong) ostack_popd(frame));
    DISPATCH
opc_d2f:
    ostack_pushf(frame, (jfloat) ostack_popd(frame));
    DISPATCH
opc_i2b:
    ostack_pushi(frame, JINT_TO_JBYTE(ostack_popi(frame)));
    DISPATCH
opc_i2c:
    ostack_pushi(frame, JINT_TO_JCHAR(ostack_popi(frame)));
    DISPATCH
opc_i2s:
    ostack_pushi(frame, JINT_TO_JSHORT(ostack_popi(frame)));
    DISPATCH
/*
 * NAN 与正常的的浮点数无法比较，即 即不大于 也不小于 也不等于。
 * 两个 NAN 之间也无法比较，即 即不大于 也不小于 也不等于。
 */
#define DO_CMP(v1, v2, default_value) \
            (jint)((v1) > (v2) ? 1 : ((v1) == (v2) ? 0 : ((v1) < (v2) ? -1 : (default_value))))

#define CMP(type, t, cmp_result) \
do { \
    type v2 = ostack_pop##t(frame); \
    type v1 = ostack_pop##t(frame); \
    ostack_pushi(frame, cmp_result); \
    DISPATCH \
} while(false)

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

#undef CMP

#define IF_COND(cond, opc_len) \
do { \
    jint v = ostack_popi(frame); \
    jint offset = bcr_reads2(reader); \
    if (v cond 0) \
        bcr_skip(reader, offset - opc_len); \
    DISPATCH \
} while(false)

opc_ifeq:
    IF_COND(==, opcode_len[JVM_OPC_ifeq]);
opc_ifne:
    IF_COND(!=, opcode_len[JVM_OPC_ifne]);
opc_iflt:
    IF_COND(<, opcode_len[JVM_OPC_iflt]);
opc_ifge:
    IF_COND(>=, opcode_len[JVM_OPC_ifge]);
opc_ifgt:
    IF_COND(>, opcode_len[JVM_OPC_ifgt]);
opc_ifle:
    IF_COND(<=, opcode_len[JVM_OPC_ifle]);

#undef IF_COND

#define IF_CMP_COND(type, t, cond, opc_len) \
do { \
    s2 offset = bcr_reads2(reader); \
    type v2 = ostack_pop##t(frame); \
    type v1 = ostack_pop##t(frame); \
    if (v1 cond v2) \
        bcr_skip(reader, offset - opc_len); \
    DISPATCH \
} while(false)

opc_if_icmpeq:
    IF_CMP_COND(jint, i, ==, opcode_len[JVM_OPC_if_icmpeq]);
opc_if_acmpeq:
    IF_CMP_COND(jref, r, ==, opcode_len[JVM_OPC_if_acmpeq]);
opc_if_icmpne:
    IF_CMP_COND(jint, i, !=, opcode_len[JVM_OPC_if_icmpne]);
opc_if_acmpne:
    IF_CMP_COND(jref, r, !=, opcode_len[JVM_OPC_if_acmpne]);
opc_if_icmplt:
    IF_CMP_COND(jint, i, <, opcode_len[JVM_OPC_if_icmplt]);
opc_if_icmpge:
    IF_CMP_COND(jint, i, >=, opcode_len[JVM_OPC_if_icmpge]);
opc_if_icmpgt:
    IF_CMP_COND(jint, i, >, opcode_len[JVM_OPC_if_icmpgt]);
opc_if_icmple:
    IF_CMP_COND(jint, i, <=, opcode_len[JVM_OPC_if_icmple]);

#undef IF_CMP_COND

opc_goto: {
    s2 offset = bcr_reads2(reader);
    bcr_skip(reader, offset - opcode_len[JVM_OPC_goto]);
    DISPATCH
}

// 在Java 6之前，Oracle的Java编译器使用 jsr, jsr_w 和 ret 指令来实现 finally 子句。
// 从Java 6开始，已经不再使用这些指令
opc_jsr:
    HANDLE_EXCEPTION(S(java_lang_InternalError), "jsr doesn't support after jdk 6.");  
opc_ret:
    HANDLE_EXCEPTION(S(java_lang_InternalError), "ret doesn't support after jdk 6."); 

opc_tableswitch: {
    // 实现当各个case值跨度比较小时的 switch 语句
    size_t saved_pc = reader->pc - 1; // save the pc before 'tableswitch' instruction
    bcr_align4(reader);

    // 默认情况下执行跳转所需字节码的偏移量
    // 对应于 switch 中的 default 分支。
    s4 default_offset = bcr_reads4(reader);

    // low 和 height 标识了 case 的取值范围。
    s4 low = bcr_reads4(reader);
    s4 height = bcr_reads4(reader);

    // 跳转偏移量表，对应于各个 case 的情况
    s4 jump_offset_count = height - low + 1;
    s4 jump_offsets[jump_offset_count];
    bcr_reads4s(reader, jump_offset_count, jump_offsets);

    // 弹出要判断的值
    index = ostack_popi(frame);
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
    // 实现当各个case值跨度比较大时的 switch 语句
    size_t saved_pc = reader->pc - 1; // save the pc before 'lookupswitch' instruction
    bcr_align4(reader);

    // 默认情况下执行跳转所需字节码的偏移量
    // 对应于 switch 中的 default 分支。
    s4 default_offset = bcr_reads4(reader);

    // case的个数
    s4 npairs = bcr_reads4(reader);
    assert(npairs >= 0); // The npairs must be greater than or equal to 0.

    // match_offsets 有点像 Map，它的 key 是 case 值，value 是跳转偏移量。
    s4 match_offsets[npairs * 2];
    bcr_reads4s(reader, npairs * 2, match_offsets);

    // 弹出要判断的值
    jint key = ostack_popi(frame);
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
    TRACE("will return: %s", get_frame_info(frame));
    pop_frame(thread);
    Frame *invoke_frame = thread->top_frame;
    TRACE("invoke frame: %s", invoke_frame == NULL ? "NULL" : get_frame_info(invoke_frame));
    frame->ostack -= ret_value_slot_count;
    slot_t *ret_value = frame->ostack;
    if (frame->vm_invoke || invoke_frame == NULL) {
        if (IS_SYNCHRONIZED(frame->method)) {
//                        _this->unlock();
        }
        return ret_value;
    }

    for (int i = 0; i < ret_value_slot_count; i++) {
        *invoke_frame->ostack++ = *ret_value++;
    }
    if (IS_SYNCHRONIZED(frame->method)) {
//                    _this->unlock();
    }
    CHANGE_FRAME(invoke_frame);
    DISPATCH  
}
opc_getstatic: {
    index = bcr_readu2(reader);
    Field *field = resolve_field(cp, index);
    if (!IS_STATIC(field)) {
        HANDLE_EXCEPTION(S(java_lang_IncompatibleClassChangeError), get_field_info(field));  
    }

    init_class(field->clazz);

    *frame->ostack++ = field->static_value.data[0];
    if (field->category_two) {
        *frame->ostack++ = field->static_value.data[1];
    }
    DISPATCH
}
opc_putstatic: {
    index = bcr_readu2(reader);
    Field *field = resolve_field(cp, index);
    if (!IS_STATIC(field)) {
        HANDLE_EXCEPTION(S(java_lang_IncompatibleClassChangeError), get_field_info(field));  
    }

    init_class(field->clazz);

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
    index = bcr_readu2(reader);
    Field *field = resolve_field(cp, index);
    CHECK_EXCEPTION_OCCURRED
    if (IS_STATIC(field)) {
        HANDLE_EXCEPTION(S(java_lang_IncompatibleClassChangeError), get_field_info(field));  
    }

    jref obj = ostack_popr(frame);
    NULL_POINTER_CHECK(obj);

    *frame->ostack++ = obj->data[field->id];
    if (field->category_two) {
        *frame->ostack++ = obj->data[field->id + 1];
    }
    DISPATCH
}
opc_putfield: {
    index = bcr_readu2(reader);
    Field *field = resolve_field(cp, index);
    CHECK_EXCEPTION_OCCURRED
    if (IS_STATIC(field)) {
        HANDLE_EXCEPTION(S(java_lang_IncompatibleClassChangeError), get_field_info(field));  
    }

    // 如果是final字段，则只能在构造函数中初始化，否则抛出java.lang.IllegalAccessError。
    if (IS_FINAL(field)) {
        if (!class_equals(clazz, field->clazz) || !utf8_equals(frame->method->name, S(object_init))) {
            HANDLE_EXCEPTION(S(java_lang_IllegalAccessError), get_field_info(field));   
        }
    }

    if (field->category_two) {
        frame->ostack -= 2;
    } else {
        frame->ostack--;
    }
    slot_t *value = frame->ostack;

    jref obj = ostack_popr(frame);
    NULL_POINTER_CHECK(obj);

    set_field_value0(obj, field, value);
    DISPATCH
}                   
opc_invokevirtual: {
    // invokevirtual指令用于调用对象的实例方法，根据对象的实际类型进行分派（虚方法分派）。
    index = bcr_readu2(reader);
    Method *m = resolve_method(cp, index);
    CHECK_EXCEPTION_OCCURRED

    if (is_signature_polymorphic(m)) {
        assert(IS_NATIVE(m));
//        frame->ostack -= m->arg_slot_count;
        u2 arg_slots_count = cal_method_args_slots_count(m->descriptor, true);
        frame->ostack -= arg_slots_count;
        resolved_method = m;
        assert(resolved_method);
        goto _invoke_method;
    }

    if (IS_STATIC(m)) {
        HANDLE_EXCEPTION(S(java_lang_IncompatibleClassChangeError), get_method_info(m));  
    }

    frame->ostack -= m->arg_slot_count;
    jref obj = slot_get_ref(frame->ostack);
    NULL_POINTER_CHECK(obj);

    if (IS_PRIVATE(m)) {
        resolved_method = m;
    } else {
        // assert(m->vtable_index >= 0);
        // assert(m->vtable_index < (int) obj->clazz->vtable.size());
        // resolved_method = obj->clazz->vtable[m->vtable_index];
        resolved_method = lookup_method(obj->clazz, m->name, m->descriptor);
        assert(resolved_method);
    }
    
    // assert(resolved_method == obj->clazz->lookupMethod(m->name, m->descriptor));
    assert(resolved_method);
    goto _invoke_method;
}
opc_invokespecial: {
    // invokespecial指令用于调用一些需要特殊处理的实例方法， 包括：
    // 1. 构造函数
    // 2. 私有方法
    // 3. 通过super关键字调用的超类方法，或者超接口中的默认方法。
    index = bcr_readu2(reader);
    Method *m = resolve_method_or_interface_method(cp, index);
    CHECK_EXCEPTION_OCCURRED

    /*
     * 如果调用的中超类中的函数，但不是构造函数，不是private 函数，且当前类的ACC_SUPER标志被设置，
     * 需要一个额外的过程查找最终要调用的方法；否则前面从方法符号引用中解析出来的方法就是要调用的方法。
     * todo 详细说明
     */
    if (IS_SUPER(m->clazz)
        && !IS_PRIVATE(m)
        && is_subclass_of(clazz, m->clazz) // todo
        && !utf8_equals(m->name, S(object_init))) {
        m = lookup_method(clazz->super_class, m->name, m->descriptor);
    }

    if (IS_ABSTRACT(m)) {
        HANDLE_EXCEPTION(S(java_lang_AbstractMethodError), get_method_info(m));  
    }
    if (IS_STATIC(m)) {
        HANDLE_EXCEPTION(S(java_lang_IncompatibleClassChangeError), get_method_info(m));  
    }

    frame->ostack -= m->arg_slot_count;
    jref obj = slot_get_ref(frame->ostack);
    NULL_POINTER_CHECK(obj);

    resolved_method = m;
    assert(resolved_method);
    goto _invoke_method;
}
opc_invokestatic: {
    // invokestatic指令用来调用静态方法。
    // 如果类还没有被初始化，会触发类的初始化。
    index = bcr_readu2(reader);
    Method *m = resolve_method_or_interface_method(cp, index);
    CHECK_EXCEPTION_OCCURRED
    if (IS_ABSTRACT(m)) {
        HANDLE_EXCEPTION(S(java_lang_AbstractMethodError), get_method_info(m));  
    }
    if (!IS_STATIC(m)) {
        HANDLE_EXCEPTION(S(java_lang_IncompatibleClassChangeError), get_method_info(m));  
    }

    // printf("%s\n", m->name);
    // if (strcmp(m->name, "setJavaLangReflectAccess") == 0) {
    //     int iii = 0;
    // }

    init_class(m->clazz);

    frame->ostack -= m->arg_slot_count;
    resolved_method = m;
    assert(resolved_method);
    goto _invoke_method;
}            
opc_invokeinterface: {
    index = bcr_readu2(reader);

    /*
     * 此字节的值是给方法传递参数需要的slot数，
     * 其含义和给method结构体定义的arg_slot_count字段相同。
     * 这个数是可以根据方法描述符计算出来的，它的存在仅仅是因为历史原因。
     */
    bcr_readu1(reader);
    /*
     * 此字节是留给Oracle的某些Java虚拟机实现用的，它的值必须是0。
     * 该字节的存在是为了保证Java虚拟机可以向后兼容。
     */
    bcr_readu1(reader);

    Method *m = resolve_interface_method(cp, index);
    CHECK_EXCEPTION_OCCURRED
    assert(IS_INTERFACE(m->clazz));

    /* todo 本地方法 */

    frame->ostack -= m->arg_slot_count;
    jref obj = slot_get_ref(frame->ostack);
    NULL_POINTER_CHECK(obj);

    // itable的实现还不对 todo
    // resolved_method = obj->clazz->findFromITable(m->clazz, m->itable_index);
    // assert(resolved_method != NULL);
    // assert(resolved_method == obj->clazz->lookupMethod(m->name, m->descriptor));
    resolved_method = lookup_method(obj->clazz, m->name, m->descriptor);
    if (IS_ABSTRACT(resolved_method)) {
        HANDLE_EXCEPTION(S(java_lang_AbstractMethodError), get_method_info(resolved_method));  
    }

    if (!IS_PUBLIC(resolved_method)) {
        HANDLE_EXCEPTION(S(java_lang_IllegalAccessError), get_method_info(resolved_method));   
    }

    assert(resolved_method);
    goto _invoke_method;
}           
opc_invokedynamic: {
    // JVM_PANIC("Don't support invokedynamic.\n"); ///////////////////////////////////////////

    u2 i = bcr_readu2(reader); // point to JVM_CONSTANT_InvokeDynamic_info
    bcr_readu1(reader); // this byte must always be zero.
    bcr_readu1(reader); // this byte must always be zero.

    const utf8_t *invoked_name = cp_invoke_dynamic_method_name(cp, i);
    const utf8_t *invoked_descriptor = cp_invoke_dynamic_method_type(cp, i);

    jref invoked_type = findMethodType(invoked_descriptor, clazz->loader); // "java/lang/invoke/MethodType"
    jref caller = getCaller(); // "java/lang/invoke/MethodHandles$Lookup"

    BootstrapMethod *bm = clazz->bootstrap_methods + cp_invoke_dynamic_bootstrap_method_index(cp, i);
    u2 ref_kind = cp_method_handle_reference_kind(cp, bm->bootstrap_method_ref);
    u2 ref_index = cp_method_handle_reference_index(cp, bm->bootstrap_method_ref);

    switch (ref_kind) {
        case JVM_REF_invokeStatic: {
            const utf8_t *class_name = cp_method_class_name(cp, ref_index);
            Class *bootstrap_class = load_class(clazz->loader, class_name);

            // bootstrap method is static,  todo 对不对
            // 前三个参数固定为 MethodHandles.Lookup caller, String invokedName, MethodType invokedType todo 对不对
            // 后续的参数由 ref->argc and ref->args 决定
            Method *bootstrap_method = get_declared_static_method(bootstrap_class,
                                    cp_method_name(cp, ref_index), cp_method_type(cp, ref_index));
            // name: "metafactory"
            // type: "(Ljava/lang/invoke/MethodHandles$Lookup;Ljava/lang/String;Ljava/lang/invoke/MethodType;Ljava/lang/invoke/MethodType;Ljava/lang/invoke/MethodHandle;Ljava/lang/invoke/MethodType;)Ljava/lang/invoke/CallS"...
            
            // args's length is big enough,多余的长度无所谓，bootstrap_method 会按需读取的。
            slot_t args[3 + bm->args_count * 2];
            slot_set_ref(args, caller);
            slot_set_ref(args + 1, alloc_string(invoked_name));
            slot_set_ref(args + 2, invoked_type);
            resolve_bootstrap_method_args(bm, cp, args + 3);
            jref call_set = exec_java_r(bootstrap_method, args);

            // public abstract MethodHandle dynamicInvoker()
            Method *dyn_invoker = lookup_inst_method(call_set->clazz, "dynamicInvoker", "()Ljava/lang/invoke/MethodHandle;");
            jref exact_method_handle = exec_java_r(dyn_invoker, (slot_t[]) { rslot(call_set) });

            // public final Object invokeExact(Object... args) throws Throwable
            Method *invokeExact = lookup_inst_method(exact_method_handle->clazz,
                                        S(invokeExact), "([Ljava/lang/Object;)Ljava/lang/Object;");
            assert(IS_VARARGS(invokeExact));
            int slots_count = cal_method_args_slots_count(invoked_descriptor, false);
            slot_t _args[slots_count];
            slot_set_ref(_args, exact_method_handle);
            slots_count--; // 减去"this"
            frame->ostack -= slots_count; // pop all args
            memcpy(_args + 1, frame->ostack, slots_count * sizeof(slot_t));
            // invoke exact method, invokedynamic completely execute over.
            slot_t *ret = exec_java(invokeExact, _args);
            ostack_pushr(frame, slot_get_ref(ret));
            break;
        }
        case JVM_REF_newInvokeSpecial:
            JVM_PANIC("JVM_REF_newInvokeSpecial"); // todo
            break;
        default:
            JVM_PANIC("xxx"); // todo
            break;
    }
    DISPATCH
}
opc_invokenative: {
    TRACE("%s", get_frame_info(frame));

    // todo 不需要在这里做任何同步的操作

    call_jni_method(frame);

    // JNI 函数执行完毕，清空其局部引用表。
    for (int i = 0; i < frame->jni_local_ref_count; i++) {
        frame->jni_local_ref_table[i] = NULL;
    }
    frame->jni_local_ref_count = 0;

    CHECK_EXCEPTION_OCCURRED

    //    if (frame->method->isSynchronized()) {
    //        _this->unlock();
    //    }
    DISPATCH
}
//opc_invokehandle: {
//    assert(resolved_method);
//    Frame *new_frame = thread->allocFrame(resolved_method, false);
//    TRACE("Alloc new frame: %s", new_frame->toString().c_str());
//
//    new_frame->lvars = frame->ostack; // todo 什么意思？？？？？？？？
//    CHANGE_FRAME(new_frame)
//    if (IS_SYNCHRONIZED(resolved_method)) {
////        _this->unlock(); // todo why unlock 而不是 lock ................................................
//    }
//    goto opc_invokenative;
//}
_invoke_method: {
    assert(resolved_method);
    Frame *new_frame = alloc_frame(thread, resolved_method, false);
    TRACE("Alloc new frame: %s", get_frame_info(new_frame));

    new_frame->lvars = frame->ostack; // todo 什么意思？？？？？？？？
    CHANGE_FRAME(new_frame);
    if (IS_SYNCHRONIZED(resolved_method)) {
//        _this->unlock(); // todo why unlock 而不是 lock ................................................
    }
    DISPATCH
}
opc_new: {
    // new指令专门用来创建类实例。数组由专门的指令创建
    // 如果类还没有被初始化，会触发类的初始化。
    Class *c = resolve_class(cp, bcr_readu2(reader));
    init_class(c);

    if (IS_INTERFACE(c) || IS_ABSTRACT(c)) {
        HANDLE_EXCEPTION(S(java_lang_InstantiationException), c->class_name);  
    }

    // jref o = newObject(c);
    // if (strcmp(o->clazz->className, "java/lang/invoke/MemberName") == 0)
    //     printvm("%s\n", o->toString().c_str()); /////////////////////////////////////////////////////////////
    // ostack_pushr(frame, o);
    ostack_pushr(frame, alloc_object(c));
    DISPATCH
}
opc_newarray: {
    // 创建一维基本类型数组。
    // 包括 boolean[], byte[], char[], short[], int[], long[], float[] 和 double[] 8种。
    jint arr_len = ostack_popi(frame);
    if (arr_len < 0) {
        // throw java_lang_NegativeArraySizeException("len is " + to_string(arr_len));
        HANDLE_EXCEPTION(S(java_lang_NegativeArraySizeException), NULL);  // todo msg
    }

    u1 arr_type = bcr_readu1(reader);
    Class *c = load_type_array_class((ArrayType) arr_type);
    ostack_pushr(frame, alloc_array(c, arr_len));
    DISPATCH
}
opc_anewarray: {
    // 创建一维引用类型数组
    jint arr_len = ostack_popi(frame);
    if (arr_len < 0) {
        // throw java_lang_NegativeArraySizeException("len is " + to_string(arr_len));
        HANDLE_EXCEPTION(S(java_lang_NegativeArraySizeException), NULL);  // todo msg
    }

    index = bcr_readu2(reader);
    Class *ac = array_class(resolve_class(cp, index));
    ostack_pushr(frame, alloc_array(ac, arr_len));
    DISPATCH
}
opc_multianewarray: {
    // 创建多维数组
    index = bcr_readu2(reader);
    Class *ac = resolve_class(cp, index);

    u1 dim = bcr_readu1(reader); // 多维数组的维度
    if (dim < 1) { // 必须大于或等于1
        HANDLE_EXCEPTION(S(java_lang_UnknownError), "The dimensions must be greater than or equal to 1.");
    }

    jint lens[dim];
    for (int i = dim - 1; i >= 0; i--) {
        lens[i] = ostack_popi(frame);
        if (lens[i] < 0) {
            // throw java_lang_NegativeArraySizeException("len is %d" + to_string(lens[i]));
            HANDLE_EXCEPTION(S(java_lang_NegativeArraySizeException), NULL);  // todo msg
        }
    }
    ostack_pushr(frame, alloc_multi_array(ac, dim, lens));
    DISPATCH
}
opc_arraylength: {
    Object *o = ostack_popr(frame);
    NULL_POINTER_CHECK(o);
    if (!is_array_object(o)) {
        HANDLE_EXCEPTION(S(java_lang_UnknownError), "not a array");
    }
    
    ostack_pushi(frame, o->arr_len);
    DISPATCH
}
opc_athrow: {
    jref eo = ostack_popr(frame); // exception object
    if (eo == NULL) {
        // 异常对象有可能为空
        // 比如下面的Java代码:
        // try {
        //     Exception x = null;
        //     throw x;
        // } catch (NullPointerException e) {
        //     e.print_stack_trace();
        // }

        HANDLE_EXCEPTION(S(java_lang_NullPointerException), NULL);
    }

    // 遍历虚拟机栈找到可以处理此异常的方法
    while (true) {
        int handler_pc = find_exception_handler(frame->method,
                eo->clazz, reader->pc - opcode_len[JVM_OPC_athrow]); // instruction length todo 好像是错的
        if (handler_pc >= 0) {  // todo 可以等于0吗
            /*
             * 找到可以处理的代码块了
             * 操作数栈清空 // todo 为啥要清空操作数栈
             * 把异常对象引用推入栈顶
             * 跳转到异常处理代码之前
             */
            clear_frame_stack(frame);
            ostack_pushr(frame, eo);
            reader->pc = (size_t) handler_pc;

            TRACE("athrow: find exception handler: %s", get_frame_info(frame));
            break;
        }

        if (frame->vm_invoke) {
            // frame 由虚拟机调用，将异常交由虚拟机处理
            *excep = eo;
            return NULL;
            // throw UncaughtException(eo);
        }

        // frame 无法处理异常，弹出
        pop_frame(thread);

        if (frame->prev == NULL) {
            // 虚拟机栈已空，还是无法处理异常，交由虚拟机处理
            *excep = eo;
            return NULL;
            // throw UncaughtException(eo);
        }

        TRACE("athrow: pop frame: %s", get_frame_info(frame));
        CHANGE_FRAME(frame->prev);
    }
    DISPATCH
}

opc_checkcast: {
    jref obj = slot_get_ref(frame->ostack - 1); // 不改变操作数栈
    index = bcr_readu2(reader);

    // 如果引用是null，则指令执行结束。也就是说，null 引用可以转换成任何类型
    if (obj != NULL) {
        Class *c = resolve_class(cp, index);
        if (!checkcast(obj->clazz, c)) {
            // throw java_lang_ClassCastException(
            //         string(obj->clazz->class_name) + " cannot be cast to " + c->class_name);
            HANDLE_EXCEPTION(S(java_lang_ClassCastException), NULL);  // todo msg
        }
    }
    DISPATCH
}

opc_instanceof: {
    index =  bcr_readu2(reader);
    Class *c = resolve_class(cp, index);

    jref obj = ostack_popr(frame);
    if (obj == NULL) {
        ostack_pushi(frame, 0);
    } else {
        ostack_pushi(frame, checkcast(obj->clazz, c) ? 1 : 0);
    }
    DISPATCH
}
opc_monitorenter: {
    jref o = ostack_popr(frame);
    NULL_POINTER_CHECK(o);
//                o->lock();
    DISPATCH
}
opc_monitorexit: {
    jref o = ostack_popr(frame);
    NULL_POINTER_CHECK(o);
//                o->unlock();
    DISPATCH
}
opc_wide:
    opcode = bcr_readu1(reader);
    PRINT_OPCODE
    index = bcr_readu2(reader);
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
            HANDLE_EXCEPTION(S(java_lang_UnknownError), NULL); // todo msg
            // throw java_lang_UnknownError("never goes here.");
    }  
opc_ifnull: {
    s2 offset = bcr_reads2(reader);
    if (ostack_popr(frame) == NULL) {
        bcr_skip(reader, offset - opcode_len[JVM_OPC_ifnull]);
    }
    DISPATCH
}
opc_ifnonnull: {
    s2 offset = bcr_reads2(reader);
    if (ostack_popr(frame) != NULL) {
        bcr_skip(reader, offset - opcode_len[JVM_OPC_ifnonnull]);
    }
    DISPATCH
}
opc_goto_w:
    HANDLE_EXCEPTION(S(java_lang_InternalError), "goto_w doesn't support");  
    DISPATCH
opc_jsr_w:
    HANDLE_EXCEPTION(S(java_lang_InternalError), "jsr_w doesn't support after jdk 6.");  
    DISPATCH
opc_breakpoint:
    HANDLE_EXCEPTION(S(java_lang_InternalError), "breakpoint doesn't support in this jvm.");  
    DISPATCH
opc_impdep2:
    HANDLE_EXCEPTION(S(java_lang_InternalError), "opc_impdep2 isn't used.");  
    DISPATCH            
opc_unused:
    JVM_PANIC("xxxxxxxxxxxxxxxx"); // todo
    HANDLE_EXCEPTION(S(java_lang_InternalError), NULL);  // todo msg
    // throw java_lang_InternalError("This instruction isn't used. " + to_string(opcode));
    DISPATCH    
}

// check can s cast to t?
static bool checkcast(Class *s, Class *t)
{
    assert(s != NULL && t != NULL);
    if (!is_array_class(s)) {
        if (is_array_class(t)) 
            return false;
        return is_subclass_of(s, t);
    } else { // s is array type
        if (IS_INTERFACE(t)) {
            // 数组实现了两个接口，看看t是不是其中之一。
            return is_subclass_of(s, t);
        } else if (is_array_class(t)) { // s and t are both array type
            Class *sc = component_class(s);
            Class *tc = component_class(t);
            if (is_prim_class(sc) || is_prim_class(tc)) {
                // s and t are same prim type array.
                return sc == tc;
            }
            return checkcast(sc, tc);
        } else { // t is not interface and array type,
            return utf8_equals(t->class_name, S(java_lang_Object));
        }
    }
}

slot_t *exec_java(Method *method, const slot_t *args)
{
    assert(method != NULL);
    assert(method->arg_slot_count > 0 ? args != NULL : true);

    Frame *frame = alloc_frame(get_current_thread(), method, true);

    // 准备参数
    for (int i = 0; i < method->arg_slot_count; i++) {
        // 传递参数到被调用的函数。
        frame->lvars[i] = args[i];
    }

    jref excep = NULL;
    slot_t *result = exec(&excep);
    if (result == NULL) { // 发生了Java代码无法处理的异常，交由虚拟机处理
        assert(excep != NULL);
        print_stack_trace(excep);
        JVM_EXIT // todo
    }
    return result;
}

slot_t *exec_java0(Method *m, jref this, jarrRef args)
{
    assert(m != NULL);

    // If m is static, this is NULL.
    if (args == NULL) {
        if (this != NULL) {
            assert(!IS_STATIC(m));
            return exec_java(m, (slot_t[]) { rslot(this) });
        } else {
            assert(IS_STATIC(m));
            return exec_java(m, NULL);
        } 
    }

    // Class[]
    jarrRef types = get_parameter_types(m);
    assert(types != NULL);
    assert(types->arr_len == args->arr_len);

    // 因为有 category two 的存在，result 的长度最大为 types_len * 2 + this_obj
    slot_t *real_args = vm_malloc(sizeof(slot_t) * (2 * types->arr_len + 1));
    int k = 0;
    if (this != NULL) {
        assert(!IS_STATIC(m));
        slot_set_ref(real_args, this);
        k++;
    }
    for (int i = 0; i < types->arr_len; i++) {
        Class *c = array_get(jclsRef, types, i)->jvm_mirror;
        jref o = array_get(jref, args, i);

        if (is_prim_class(c)) {
            const slot_t *unbox = prim_wrapper_obj_unbox(o);
            real_args[k++] = *unbox;
            if (strcmp(o->clazz->class_name, "long") == 0
                || strcmp(o->clazz->class_name, "double") == 0) // category_two
                real_args[k++] = *++unbox;
        } else {
            slot_set_ref(real_args + k, o);
            k++;
        }
    }

    return exec_java(m, real_args);
}

JNIEnv *get_jni_env();
void *find_from_java_dll(Method *m);

static void call_jni_method(Frame *frame)
{
    assert(frame != NULL && frame->method != NULL);
    Method *m = frame->method;
    assert(IS_NATIVE(m));
    
    const slot_t *args = frame->lvars;    

    if (utf8_equals(m->clazz->class_name, "java/lang/invoke/MethodHandle")) {
        assert(IS_VARARGS(m));
        assert(m->native_method != NULL);
        jref r = ((jref(*)(const slot_t *)) m->native_method)(args);
        // todo
        JVM_PANIC("xxxxx");
        return;
    }

    if (m->native_method == NULL) {
        m->native_method = find_from_java_dll(m);
        if (m->native_method == NULL) {
            JVM_PANIC("Don't find native method: %s, %s, %s",
                            m->clazz->class_name, m->name, m->descriptor); // todo
        }
    }

    int args_count_max = m->arg_slot_count + 2; // plus 2: env and (clsRef or this)

    ffi_type *arg_types[args_count_max];
    void *arg_values[args_count_max];

    /* 准备参数 */
    JNIEnv *env = get_jni_env(); 
    arg_types[0] = &ffi_type_pointer;
    arg_values[0] = &env; 

    arg_types[1] = &ffi_type_pointer;
    if (IS_STATIC(m)) {
        arg_values[1] = &(m->clazz->java_mirror);
    } else {        
        arg_values[1] = (void *) args; // this
        args++;
    }

    int argc = 2;

    // 应对 java/lang/invoke/MethodHandle.java 中的 invoke* native methods.
    // 比如：
    // public final native @PolymorphicSignature Object invoke(Object... args) throws Throwable;
    // if (is_signature_polymorphic(frame->method)
    //         && !IS_STATIC(frame->method)
    //         && type == typeid(jref(*)(const slot_t *))) {
    //     jref ret = ((jref(*)(const slot_t *)) func)(lvars);
    //     ostack_pushr(frame, ret);
    //     return;
    // }

    // // 应对 java/lang/invoke/MethodHandle.java 中的 linkTo* native methods.
    // // 比如：
    // // static native @PolymorphicSignature Object linkToStatic(Object... args) throws Throwable;
    // if (is_signature_polymorphic(frame->method)
    //     && IS_STATIC(frame->method)
    //     && type == typeid(jref(*)(u2, const slot_t *))) {
    //     jref ret = ((jref(*)(u2, const slot_t *)) func)(frame->method->arg_slot_count, lvars);
    //     ostack_pushr(frame, ret);
    //     return;
    // }

    const char *p = m->descriptor;
    assert(*p == JVM_SIGNATURE_FUNC);
    p++; // skip start (

    for (; *p != JVM_SIGNATURE_ENDFUNC; args++, p++, argc++) {
        switch (*p) {
            case JVM_SIGNATURE_BOOLEAN:
            case JVM_SIGNATURE_BYTE: {
                jbyte b = slot_get_byte(args);
                *(jbyte *) args = b;
                arg_types[argc] = &ffi_type_sint8;
                arg_values[argc] = (void *) args;
                break;
            }
            case JVM_SIGNATURE_CHAR: {
                jchar c = slot_get_char(args);
                *(jchar *) args = c;
                arg_types[argc] = &ffi_type_uint16;
                arg_values[argc] = (void *) args;
                break;
            }
            case JVM_SIGNATURE_SHORT: {
                jshort s = slot_get_short(args);
                *(jshort *) args = s;
                arg_types[argc] = &ffi_type_sint16;
                arg_values[argc] = (void *) args;
                break;
            }
            case JVM_SIGNATURE_INT:
                arg_types[argc] = &ffi_type_sint32;
                arg_values[argc] = (void *) args;
                break;
            case JVM_SIGNATURE_FLOAT:
                arg_types[argc] = &ffi_type_float;
                arg_values[argc] = (void *) args;
                break;
            case JVM_SIGNATURE_LONG:
                arg_types[argc] = &ffi_type_sint64;
                arg_values[argc] = (void *) args;
                args++;
                break;
            case JVM_SIGNATURE_DOUBLE:
                arg_types[argc] = &ffi_type_double;
                arg_values[argc] = (void *) args;
                args++;
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
                arg_values[argc] = (void *) args;
                break;
            default:
                SHOULD_NEVER_REACH_HERE("%c", *p);
                break;
        }
    }

#define ffi_apply(func, argc, rtype, arg_types, rvalue, arg_values) \
do { \
    ffi_cif cif; \
    ffi_prep_cif(&cif, FFI_DEFAULT_ABI, argc, rtype, arg_types); \
    ffi_call(&cif, func, rvalue, arg_values); \
} while(0)


    switch (m->ret_type) {
        case RET_VOID:
            ffi_apply(m->native_method, argc, &ffi_type_void, arg_types, NULL, arg_values);
            break;
        case RET_BYTE:
        case RET_BOOL: {
            jbyte ret_value;
            ffi_apply(m->native_method, argc, &ffi_type_sint8, arg_types, &ret_value, arg_values);
            ostack_pushi(frame, ret_value);
            break;
        }
        case RET_CHAR: {
            jchar ret_value;
            ffi_apply(m->native_method, argc, &ffi_type_uint16, arg_types, &ret_value, arg_values);
            ostack_pushi(frame, ret_value);
            break;
        }
        case RET_SHORT: {
            jshort ret_value;
            ffi_apply(m->native_method, argc, &ffi_type_sint16, arg_types, &ret_value, arg_values);
            ostack_pushi(frame, ret_value);
            break;
        }
        case RET_INT: {
            jint ret_value;
            ffi_apply(m->native_method, argc, &ffi_type_sint32, arg_types, &ret_value, arg_values);
            ostack_pushi(frame, ret_value);
            break;
        }
        case RET_FLOAT: {
            jfloat ret_value;
            ffi_apply(m->native_method, argc, &ffi_type_float, arg_types, &ret_value, arg_values);
            ostack_pushf(frame, ret_value);
            break;
        }
        case RET_LONG: {
            jlong ret_value;
            ffi_apply(m->native_method, argc, &ffi_type_sint64, arg_types, &ret_value, arg_values);
            ostack_pushl(frame, ret_value);
            break;
        }
        case RET_DOUBLE: {
            jdouble ret_value;
            ffi_apply(m->native_method, argc, &ffi_type_double, arg_types, &ret_value, arg_values);
            ostack_pushd(frame, ret_value);
            break;
        }
        case RET_REFERENCE: {
            jref ret_value;
            ffi_apply(m->native_method, argc, &ffi_type_pointer, arg_types, &ret_value, arg_values);
            ostack_pushr(frame, ret_value);
            break;
        }
        default:
            SHOULD_NEVER_REACH_HERE("%d", m->ret_type);
            break;
    }
}
