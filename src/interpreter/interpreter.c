// #include <iostream>
#include <math.h>
#include <setjmp.h>
#include "../cabin.h"
#include "interpreter.h"
#include "../metadata/class.h"
#include "../objects/mh.h"
#include "../objects/object.h"
#include "../metadata/method.h"
#include "../debug.h"
#include "../runtime/vm_thread.h"
#include "../runtime/frame.h"
#include "../exception.h"

// using namespace std;
// using namespace method_handles;

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

static unsigned char opcode_len[JVM_OPC_MAX+1] = JVM_OPCODE_LENGTH_INITIALIZER;

static void callJNIMethod(Frame *frame);
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

    Thread *thread = getCurrentThread();

    Frame *frame = thread->top_frame;
    TRACE("executing frame: %s\n", get_frame_info(frame));

    Method *resolved_method;
    
    int index;

    BytecodeReader *reader = &frame->reader;
    Class *clazz = frame->method->clazz;
    ConstantPool *cp = &frame->method->clazz->cp;
    // slot_t *ostack = frame->ostack;
    slot_t *lvars = frame->lvars;

    jref _this = ACC_IS_STATIC(frame->method->access_flags) ? (jref) clazz : slot_get_ref(lvars);

    
    int result = setjmp(thread->jmpbuf);
    if (result != 0) {
        jobject _excep = thread->exception;
        assert(_excep != NULL);
        ostack_pushr(frame, _excep);
        
        thread->exception = NULL;
        goto opc_athrow;
    }

    // if (excep != NULL) {
    //     ostack_pushr(frame, excep);
    //     excep = NULL;
    //     goto opc_athrow;
    // }

#define NULL_POINTER_CHECK(ref) \
do { \
    if (ref == NULL) \
        raise_exception(S(java_lang_NullPointerException), NULL); \
} while(false) // throw java_lang_NullPointerException();

#define CHANGE_FRAME(new_frame) \
do { \
    /*frame->ostack = ostack;  stack指针在变动，需要设置一下 todo */ \
    frame = new_frame; \
    reader = &frame->reader; \
    clazz = frame->method->clazz; \
    cp = &frame->method->clazz->cp; \
    /*ostack = frame->ostack; */                                     \
    lvars = frame->lvars; \
    _this = ACC_IS_STATIC(frame->method->access_flags) ? (jref) clazz : slot_get_ref(lvars); \
    TRACE("executing frame: %s\n", get_frame_info(frame)); \
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
    ostack_pushr(frame, jnull);
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
            raise_exception(S(java_lang_UnknownError), NULL); // todo msg
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
            raise_exception(S(java_lang_UnknownError), NULL); // todo msg
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
    jarray arr = ostack_popr(frame); \
    NULL_POINTER_CHECK(arr); \
    if (!jarray_check_bounds(arr, index)) \
        raise_exception(S(java_lang_ArrayIndexOutOfBoundsException), NULL); /* todo msg */       
       // throw java_lang_ArrayIndexOutOfBoundsException("index is " + to_string(index));

opc_iaload: {
    GET_AND_CHECK_ARRAY
    jint value = jarray_get(jint, arr, index);
    ostack_pushi(frame, value);
    DISPATCH
}
opc_faload: {
    GET_AND_CHECK_ARRAY
    jfloat value = jarray_get(jfloat, arr, index);
    ostack_pushf(frame, value);
    DISPATCH
}
opc_aaload: {
    GET_AND_CHECK_ARRAY
    jref value = jarray_get(jref, arr, index);
    ostack_pushr(frame, value);
    DISPATCH
}
opc_baload: {
    GET_AND_CHECK_ARRAY
    jint value = jarray_get(jbyte, arr, index);
    ostack_pushi(frame, value);
    DISPATCH
}
opc_caload: {
    GET_AND_CHECK_ARRAY
    jint value = jarray_get(jchar, arr, index);
    ostack_pushi(frame, value);
    DISPATCH
}
opc_saload: {
    GET_AND_CHECK_ARRAY
    jint value = jarray_get(jshort, arr, index);
    ostack_pushi(frame, value);
    DISPATCH
}
opc_laload: {
    GET_AND_CHECK_ARRAY
    jlong value = jarray_get(jlong, arr, index);
    ostack_pushl(frame, value);
    DISPATCH
}
opc_daload: {
    GET_AND_CHECK_ARRAY
    jdouble value = jarray_get(jdouble, arr, index);
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
    jarray_set_int(arr, index, value);
    DISPATCH
}
opc_fastore: {
    jfloat value = ostack_popf(frame);
    GET_AND_CHECK_ARRAY
    // arr->setFloat(index, value);
    jarray_set_float(arr, index, value);
    DISPATCH
}
opc_aastore: {
    jobject value = ostack_popr(frame);
    GET_AND_CHECK_ARRAY
    // arr->setRef(index, value);
    jarray_set_ref(arr, index, value);
    DISPATCH
}
opc_bastore: {
    jint value = ostack_popi(frame);
    GET_AND_CHECK_ARRAY
    if (is_byte_array_class(arr->clazz)) {
        // arr->setByte(index, (jbyte) value);
        jarray_set_byte(arr, index, value);
    } else if (is_boolean_array_class(arr->clazz)) {  
        // arr->setBoolean(index, value != 0 ? jtrue : jfalse);
        jarray_set_boolean(arr, index, value != 0 ? jtrue : jfalse);
    } else {
        JVM_PANIC("never go here"); // todo
    }
    DISPATCH
}
opc_castore: {
    jint value = ostack_popi(frame);
    GET_AND_CHECK_ARRAY
    // arr->setChar(index, (jchar) value);
    jarray_set_char(arr, index, (jchar) value);
    DISPATCH
}
opc_sastore: {
    jint value = ostack_popi(frame);
    GET_AND_CHECK_ARRAY
    // arr->setShort(index, (jshort) value);
    jarray_set_short(arr, index, (jshort) value);
    DISPATCH
}
opc_lastore: {
    jlong value = ostack_popl(frame);
    GET_AND_CHECK_ARRAY
    // arr->setLong(index, value);
    jarray_set_long(arr, index, value);
    DISPATCH
}
opc_dastore: {
    jdouble value = ostack_popd(frame);
    GET_AND_CHECK_ARRAY
    // arr->setDouble(index, value);
    jarray_set_double(arr, index, value);
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
        raise_exception(S(java_lang_ArithmeticException), "division by zero"); \ 
} while(false) //  throw java_lang_ArithmeticException("division by zero"); 

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
    ostack_pushi(frame, jint2jbyte(ostack_popi(frame)));
    DISPATCH
opc_i2c:
    ostack_pushi(frame, jint2jchar(ostack_popi(frame)));
    DISPATCH
opc_i2s:
    ostack_pushi(frame, jint2jshort(ostack_popi(frame)));
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
    // throw java_lang_InternalError("jsr doesn't support after jdk 6.");
    raise_exception(S(java_lang_InternalError), "jsr doesn't support after jdk 6.");  
opc_ret:
    // throw java_lang_InternalError("ret doesn't support after jdk 6.");
    raise_exception(S(java_lang_InternalError), "ret doesn't support after jdk 6."); 

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
    TRACE("will return: %s\n", get_frame_info(frame));
    pop_frame(thread);
    Frame *invoke_frame = thread->top_frame;
    TRACE("invoke frame: %s\n", invoke_frame == NULL ? "NULL" : get_frame_info(invoke_frame));
    frame->ostack -= ret_value_slot_count;
    slot_t *ret_value = frame->ostack;
    if (frame->vm_invoke || invoke_frame == NULL) {
        if (ACC_IS_SYNCHRONIZED(frame->method->access_flags)) {
//                        _this->unlock();
        }
        return ret_value;
    }

    for (int i = 0; i < ret_value_slot_count; i++) {
        *invoke_frame->ostack++ = *ret_value++;
    }
    if (ACC_IS_SYNCHRONIZED(frame->method->access_flags)) {
//                    _this->unlock();
    }
    CHANGE_FRAME(invoke_frame);
    DISPATCH  
}
opc_getstatic: {
    index = bcr_readu2(reader);
    Field *field = resolve_field(cp, index);
    if (!ACC_IS_STATIC(field->access_flags)) {
        // throw java_lang_IncompatibleClassChangeError(get_field_info(field));
        raise_exception(S(java_lang_IncompatibleClassChangeError), get_field_info(field));  
    }

    initClass(field->clazz);

    *frame->ostack++ = field->static_value.data[0];
    if (field->category_two) {
        *frame->ostack++ = field->static_value.data[1];
    }
    DISPATCH
}
opc_putstatic: {
    index = bcr_readu2(reader);
    Field *field = resolve_field(cp, index);
    if (!ACC_IS_STATIC(field->access_flags)) {
        // throw java_lang_IncompatibleClassChangeError(get_field_info(field));
        raise_exception(S(java_lang_IncompatibleClassChangeError), get_field_info(field));  
    }
    // printf("%s\n", field->name); // todo ////////////////////////////////////////////////////////////////////////////////////
    initClass(field->clazz);

    if (field->category_two) {
        frame->ostack -= 2;
        field->static_value.data[0] = frame->ostack[0];
        field->static_value.data[1] = frame->ostack[1];
    } else {
        field->static_value.data[0] = *--frame->ostack;
    }

    // if (strcmp(field->name, "soleInstance") == 0) {
    //     printf("%p\n", field->static_value.r); // todo ////////////////////////////////////////////////////////////////
    // }

    DISPATCH
}                
opc_getfield: {
    index = bcr_readu2(reader);
    Field *field = resolve_field(cp, index);
    if (ACC_IS_STATIC(field->access_flags)) {
        // throw java_lang_IncompatibleClassChangeError(get_field_info(field));
        raise_exception(S(java_lang_IncompatibleClassChangeError), get_field_info(field));  
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
    if (ACC_IS_STATIC(field->access_flags)) {
        // throw java_lang_IncompatibleClassChangeError(get_field_info(field));
        raise_exception(S(java_lang_IncompatibleClassChangeError), get_field_info(field));  
    }

    // 如果是final字段，则只能在构造函数中初始化，否则抛出java.lang.IllegalAccessError。
    if (ACC_IS_FINAL(field->access_flags)) {
        if (!class_equals(clazz, field->clazz) || !utf8_equals(frame->method->name, S(object_init))) {
            // throw java_lang_IllegalAccessError(get_field_info(field));
            raise_exception(S(java_lang_IllegalAccessError), get_field_info(field));   
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
    if (m == NULL) {
        // todo
        JVM_PANIC("m == NULL");
    }

    if (is_signature_polymorphic(m)) {
        assert(ACC_IS_NATIVE(m->access_flags));
//        frame->ostack -= m->arg_slot_count;
        u2 arg_slots_count = cal_method_args_slots_count(m->descriptor, true);
        frame->ostack -= arg_slots_count;
        resolved_method = m;
        goto _invoke_method;
    }

    if (ACC_IS_STATIC(m->access_flags)) {
        // throw java_lang_IncompatibleClassChangeError(get_method_info(m));
        raise_exception(S(java_lang_IncompatibleClassChangeError), get_method_info(m));  
    }

    frame->ostack -= m->arg_slot_count;
    jref obj = slot_get_ref(frame->ostack);
    NULL_POINTER_CHECK(obj);

    if (ACC_IS_PRIVATE(m->access_flags)) {
        resolved_method = m;
    } else {
        // assert(m->vtable_index >= 0);
        // assert(m->vtable_index < (int) obj->clazz->vtable.size());
        // resolved_method = obj->clazz->vtable[m->vtable_index];
        resolved_method = lookup_method(obj->clazz, m->name, m->descriptor);
    }

    // assert(resolved_method == obj->clazz->lookupMethod(m->name, m->descriptor));
    goto _invoke_method;
}
opc_invokespecial: {
    // invokespecial指令用于调用一些需要特殊处理的实例方法， 包括：
    // 1. 构造函数
    // 2. 私有方法
    // 3. 通过super关键字调用的超类方法，或者超接口中的默认方法。
    index = bcr_readu2(reader);
    Method *m = resolve_method_or_interface_method(cp, index);

    /*
     * 如果调用的中超类中的函数，但不是构造函数，不是private 函数，且当前类的ACC_SUPER标志被设置，
     * 需要一个额外的过程查找最终要调用的方法；否则前面从方法符号引用中解析出来的方法就是要调用的方法。
     * todo 详细说明
     */
    if (ACC_IS_SUPER(m->clazz->access_flags)
        && !ACC_IS_PRIVATE(m->access_flags)   
        && is_subclass_of(clazz, m->clazz) // todo
        && !utf8_equals(m->name, S(object_init))) {
        m = lookup_method(clazz->super_class, m->name, m->descriptor);
    }

    if (ACC_IS_ABSTRACT(m->access_flags)) {
        // throw java_lang_AbstractMethodError(get_method_info(m));
        raise_exception(S(java_lang_AbstractMethodError), get_method_info(m));  
    }
    if (ACC_IS_STATIC(m->access_flags)) {
        // throw java_lang_IncompatibleClassChangeError(get_method_info(m));
        raise_exception(S(java_lang_IncompatibleClassChangeError), get_method_info(m));  
    }

    frame->ostack -= m->arg_slot_count;
    jref obj = slot_get_ref(frame->ostack);
    NULL_POINTER_CHECK(obj);

    resolved_method = m;
    goto _invoke_method;
}
opc_invokestatic: {
    // invokestatic指令用来调用静态方法。
    // 如果类还没有被初始化，会触发类的初始化。
    index = bcr_readu2(reader);
    Method *m = resolve_method_or_interface_method(cp, index);
    if (ACC_IS_ABSTRACT(m->access_flags)) {
        // throw java_lang_AbstractMethodError(get_method_info(m));
        raise_exception(S(java_lang_AbstractMethodError), get_method_info(m));  
    }
    if (!ACC_IS_STATIC(m->access_flags)) {
        // throw java_lang_IncompatibleClassChangeError(get_method_info(m));
        raise_exception(S(java_lang_IncompatibleClassChangeError), get_method_info(m));  
    }

    // printf("%s\n", m->name);
    // if (strcmp(m->name, "setJavaLangReflectAccess") == 0) {
    //     int iii = 0;
    // }

    initClass(m->clazz);

    frame->ostack -= m->arg_slot_count;
    resolved_method = m;
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
    assert(ACC_IS_INTERFACE(m->clazz->access_flags));

    /* todo 本地方法 */

    frame->ostack -= m->arg_slot_count;
    jref obj = slot_get_ref(frame->ostack);
    NULL_POINTER_CHECK(obj);

    // itable的实现还不对 todo
    // resolved_method = obj->clazz->findFromITable(m->clazz, m->itable_index);
    // assert(resolved_method != NULL);
    // assert(resolved_method == obj->clazz->lookupMethod(m->name, m->descriptor));
    resolved_method = lookup_method(obj->clazz, m->name, m->descriptor);
    if (ACC_IS_ABSTRACT(resolved_method->access_flags)) {
        // throw java_lang_AbstractMethodError(get_method_info(resolved_method));
        raise_exception(S(java_lang_AbstractMethodError), get_method_info(resolved_method));  
    }

    if (!ACC_IS_PUBLIC(resolved_method->access_flags)) {
        // throw java_lang_IllegalAccessError(get_method_info(resolved_method));
        raise_exception(S(java_lang_IllegalAccessError), get_method_info(resolved_method));   
    }

    goto _invoke_method;
}           
opc_invokedynamic: {
    printvm("invokedynamic\n"); /////////////////////////////////////////////////////////////////////////////////

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
            Class *bootstrap_class = loadClass(clazz->loader, class_name);

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
            jobject call_set = exec_java_func_r(bootstrap_method, args);

            // public abstract MethodHandle dynamicInvoker()
            Method *dyn_invoker = lookup_inst_method(call_set->clazz, "dynamicInvoker", "()Ljava/lang/invoke/MethodHandle;");
            jobject exact_method_handle = slot_get_ref(exec_java_func1(dyn_invoker, call_set));

            // public final Object invokeExact(Object... args) throws Throwable
            Method *invokeExact = lookup_inst_method(exact_method_handle->clazz,
                                        S(invokeExact), "([Ljava/lang/Object;)Ljava/lang/Object;");
            assert(ACC_IS_VARARGS(invokeExact->access_flags));
            int slots_count = cal_method_args_slots_count(invoked_descriptor, false);
            slot_t _args[slots_count];
            slot_set_ref(_args, exact_method_handle);
            slots_count--; // 减去"this"
            frame->ostack -= slots_count; // pop all args
            memcpy(_args + 1, frame->ostack, slots_count * sizeof(slot_t));
            // invoke exact method, invokedynamic completely execute over.
            slot_t *ret = exec_java_func(invokeExact, _args);
            ostack_pushr(frame, slot_get_ref(ret));
            break;
        }
        case JVM_REF_newInvokeSpecial:
            JVM_PANIC("JVM_REF_newInvokeSpecial"); // todo
            break;
        default:
            JVM_PANIC("never goes here"); // todo
            break;
    }
    DISPATCH
}
opc_invokenative: {
    TRACE("%s\n", get_frame_info(frame));
    if (frame->method->native_method == NULL){ // todo
        JVM_PANIC("not find native method: %s\n", get_method_info(frame->method));
    }

    // todo 不需要在这里做任何同步的操作

    assert(frame->method->native_method != NULL);


    callJNIMethod(frame);

//    if (Thread::checkExceptionOccurred()) {
//        TRACE("native method throw a exception\n");
//        jref eo = Thread::getException();
//        Thread::clearException();
//        ostack_pushr(frame, eo);
//        goto opc_athrow;
//    }

//    if (frame->method->isSynchronized()) {
//        _this->unlock();
//    }
    DISPATCH
}
//opc_invokehandle: {
//    assert(resolved_method);
//    Frame *new_frame = thread->allocFrame(resolved_method, false);
//    TRACE("Alloc new frame: %s\n", new_frame->toString().c_str());
//
//    new_frame->lvars = frame->ostack; // todo 什么意思？？？？？？？？
//    CHANGE_FRAME(new_frame)
//    if (ACC_IS_SYNCHRONIZED(resolved_method->access_flags)) {
////        _this->unlock(); // todo why unlock 而不是 lock ................................................
//    }
//    goto opc_invokenative;
//}
_invoke_method: {
    assert(resolved_method);
    Frame *new_frame = alloc_frame(thread, resolved_method, false);
    TRACE("Alloc new frame: %s\n", get_frame_info(new_frame));

    new_frame->lvars = frame->ostack; // todo 什么意思？？？？？？？？
    CHANGE_FRAME(new_frame);
    if (ACC_IS_SYNCHRONIZED(resolved_method->access_flags)) {
//        _this->unlock(); // todo why unlock 而不是 lock ................................................
    }
    DISPATCH
}
opc_new: {
    // new指令专门用来创建类实例。数组由专门的指令创建
    // 如果类还没有被初始化，会触发类的初始化。
    Class *c = resolve_class(cp, bcr_readu2(reader));
    initClass(c);

    if (ACC_IS_INTERFACE(c->access_flags) || ACC_IS_ABSTRACT(c->access_flags)) { 
        // throw java_lang_InstantiationException(c->class_name);
        raise_exception(S(java_lang_InstantiationException), c->class_name);  
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
        raise_exception(S(java_lang_NegativeArraySizeException), NULL);  // todo msg
    }

    u1 arr_type = bcr_readu1(reader);
    Class *c = loadTypeArrayClass((ArrayType) arr_type);
    ostack_pushr(frame, alloc_array(c, arr_len));
    DISPATCH
}
opc_anewarray: {
    // 创建一维引用类型数组
    jint arr_len = ostack_popi(frame);
    if (arr_len < 0) {
        // throw java_lang_NegativeArraySizeException("len is " + to_string(arr_len));
        raise_exception(S(java_lang_NegativeArraySizeException), NULL);  // todo msg
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
        raise_exception(S(java_lang_UnknownError), NULL); // todo msg
        // throw java_lang_UnknownError("The dimensions must be greater than or equal to 1.");
    }

    jint lens[dim];
    for (int i = dim - 1; i >= 0; i--) {
        lens[i] = ostack_popi(frame);
        if (lens[i] < 0) {
            // throw java_lang_NegativeArraySizeException("len is %d" + to_string(lens[i]));
            raise_exception(S(java_lang_NegativeArraySizeException), NULL);  // todo msg
        }
    }
    ostack_pushr(frame, alloc_multi_array(ac, dim, lens));
    DISPATCH
}
opc_arraylength: {
    Object *o = ostack_popr(frame);
    NULL_POINTER_CHECK(o);
    if (!is_array_object(o)) {
        raise_exception(S(java_lang_UnknownError), NULL); // todo msg
        // throw java_lang_UnknownError("not a array");
    }
    
    ostack_pushi(frame, o->arr_len);
    DISPATCH
}
opc_athrow: {
    jref eo = ostack_popr(frame); // exception object
    if (eo == jnull) {
        // 异常对象有可能为空
        // 比如下面的Java代码:
        // try {
        //     Exception x = null;
        //     throw x;
        // } catch (NullPointerException e) {
        //     e.printStackTrace();
        // }

        // throw java_lang_NullPointerException();
        raise_exception(S(java_lang_NullPointerException), NULL);
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

            TRACE("athrow: find exception handler: %s\n", get_frame_info(frame));
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

        TRACE("athrow: pop frame: %s\n", get_frame_info(frame));
        CHANGE_FRAME(frame->prev);
    }
    DISPATCH
}

opc_checkcast: {
    jref obj = slot_get_ref(frame->ostack - 1); // 不改变操作数栈
    index = bcr_readu2(reader);

    // 如果引用是null，则指令执行结束。也就是说，null 引用可以转换成任何类型
    if (obj != jnull) {
        Class *c = resolve_class(cp, index);
        if (!checkcast(obj->clazz, c)) {
            // throw java_lang_ClassCastException(
            //         string(obj->clazz->class_name) + " cannot be cast to " + c->class_name);
            raise_exception(S(java_lang_ClassCastException), NULL);  // todo msg
        }
    }
    DISPATCH
}

opc_instanceof: {
    index =  bcr_readu2(reader);
    Class *c = resolve_class(cp, index);

    jref obj = ostack_popr(frame);
    if (obj == jnull) {
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
            raise_exception(S(java_lang_UnknownError), NULL); // todo msg
            // throw java_lang_UnknownError("never goes here.");
    }  
opc_ifnull: {
    s2 offset = bcr_reads2(reader);
    if (ostack_popr(frame) == jnull) {
        bcr_skip(reader, offset - opcode_len[JVM_OPC_ifnull]);
    }
    DISPATCH
}
opc_ifnonnull: {
    s2 offset = bcr_reads2(reader);
    if (ostack_popr(frame) != jnull) {
        bcr_skip(reader, offset - opcode_len[JVM_OPC_ifnonnull]);
    }
    DISPATCH
}
opc_goto_w:
    raise_exception(S(java_lang_InternalError), "goto_w doesn't support");  
    // throw java_lang_InternalError("goto_w doesn't support");
    DISPATCH
opc_jsr_w:
    raise_exception(S(java_lang_InternalError), "jsr_w doesn't support after jdk 6.");  
    // throw java_lang_InternalError("jsr_w doesn't support after jdk 6.");
    DISPATCH
opc_breakpoint:
    raise_exception(S(java_lang_InternalError), "breakpoint doesn't support in this jvm.");  
    // throw java_lang_InternalError("breakpoint doesn't support in this jvm.");
    DISPATCH
opc_impdep2:
    raise_exception(S(java_lang_InternalError), "opc_impdep2 isn't used.");  
    // throw java_lang_InternalError("opc_impdep2 isn't used.");
    DISPATCH            
opc_unused:
    raise_exception(S(java_lang_InternalError), NULL);  // todo msg
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
        if (ACC_IS_INTERFACE(t->access_flags)) {
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

slot_t *exec_java_func(Method *method, const slot_t *args)
{
    assert(method != NULL);
    assert(method->arg_slot_count > 0 ? args != NULL : true);

    Frame *frame = alloc_frame(getCurrentThread(), method, true);

    // 准备参数
    for (int i = 0; i < method->arg_slot_count; i++) {
        // 传递参数到被调用的函数。
        frame->lvars[i] = args[i];
    }

    jref excep = NULL;
    slot_t *result = exec(&excep);
    if (result == NULL) { // 发生了Java代码无法处理的异常，交由虚拟机处理
        assert(excep != NULL);
        printStackTrace(excep);
        JVM_EXIT // todo
    }
    return result;

    // while (true) {
    //     try {
    //         slot_t *result = exec(&excep);
    //         if (result == NULL) { // 发生了Java代码无法处理的异常，交由虚拟机处理
    //             assert(excep != NULL);
    //             printStackTrace(excep);
    //             JVM_EXIT // todo
    //         }
    //         return result;
    //     } 
    //     // catch (JavaException &e) {
    //     //     excep = e.getExcep();
    //     // } 
    //     // catch (UncaughtException &e) {
    //     //     printStackTrace(e.java_excep);
    //     //     JVM_EXIT // todo
    //     // } catch (...) {
    //     //     JVM_PANIC(""); // todo
    //     // }
    // }
}

// slot_t *execJavaFunc(Method *method, initializer_list<slot_t> args)
// {
//     assert(method != NULL);
//     assert(method->arg_slot_count == args.size());

//     slot_t slots[args.size()];
//     int i = 0;
//     for (slot_t arg : args) {
//         slots[i++] = arg;
//     }

//     return exec_java_func(method, slots);
// }

// slot_t *exec_java_func1(Method *method, std::initializer_list<jref> args)
// {
//     assert(method != NULL);
//     assert(method->arg_slot_count == args.size());

//     slot_t slots[args.size()];
//     int i = 0;
//     for (jref arg : args) {
//         slots[i++] = rslot(arg);
//     }

//     return exec_java_func(method, slots);
// }

slot_t *exec_java_func0(Method *m, jref _this, jarray args)
{
    assert(m != NULL);

    // If m is static, _this is NULL.
    if (args == NULL) {
        if (_this != NULL)
            return exec_java_func1(m, _this);
        else
            return exec_java_func(m, NULL);
    }

    // Class[]
    jarray types = get_parameter_types(m);
    assert(types != NULL);
    assert(types->arr_len == args->arr_len);

    // 因为有 category two 的存在，result 的长度最大为 types_len * 2 + this_obj
    slot_t *real_args = (slot_t *)vm_malloc(sizeof(slot_t) * (2 * types->arr_len + 1)); //new slot_t[2 * types->arr_len + 1];
    int k = 0;
    if (_this != NULL) {
        slot_set_ref(real_args, _this);
        k++;
    }
    for (int i = 0; i < types->arr_len; i++) {
        Class *c = jarray_get(ClsObj *, types, i)->jvm_mirror;
        jref o = jarray_get(jref, args, i);

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

    return exec_java_func(m, real_args);
}

slot_t *exec_java_func1(struct Method *method, jref arg)
{
    assert(method != NULL && arg != NULL);
    assert(method->arg_slot_count == 1);
    slot_t s = rslot(arg);
    return exec_java_func(method, &s);
}

slot_t *exec_java_func2(struct Method *method, jref arg1, jref arg2)
{
    assert(method != NULL && arg1 != NULL && arg2 != NULL);
    assert(method->arg_slot_count == 2);

    slot_t slots[2] = { rslot(arg1), rslot(arg2) };
    return exec_java_func(method, slots);
}

slot_t *exec_java_func3(struct Method *method, jref arg1, jref arg2, jref arg3)
{
    assert(method != NULL && arg1 != NULL && arg2 != NULL && arg3 != NULL);
    assert(method->arg_slot_count == 3);

    slot_t slots[3] = { rslot(arg1), rslot(arg2), rslot(arg3) };
    return exec_java_func(method, slots);
}

static void callJNIMethod(Frame *frame)
{
    assert(frame != NULL && frame->method != NULL);
    assert(ACC_IS_NATIVE(frame->method->access_flags) && frame->method->native_method != NULL);
    assert(frame->method->native_method->func != NULL);

    const slot_t *lvars = frame->lvars;
    // const type_info &type = frame->method->native_method->type;
    void *func = frame->method->native_method->func;
    assert(func != NULL);

    // 应对 java/lang/invoke/MethodHandle.java 中的 invoke* native methods.
    // 比如：
    // public final native @PolymorphicSignature Object invoke(Object... args) throws Throwable;
    // if (is_signature_polymorphic(frame->method)
    //         && !ACC_IS_STATIC(frame->method->access_flags)
    //         && type == typeid(jref(*)(const slot_t *))) {
    //     jref ret = ((jref(*)(const slot_t *)) func)(lvars);
    //     ostack_pushr(frame, ret);
    //     return;
    // }

    // // 应对 java/lang/invoke/MethodHandle.java 中的 linkTo* native methods.
    // // 比如：
    // // static native @PolymorphicSignature Object linkToStatic(Object... args) throws Throwable;
    // if (is_signature_polymorphic(frame->method)
    //     && ACC_IS_STATIC(frame->method->access_flags)
    //     && type == typeid(jref(*)(u2, const slot_t *))) {
    //     jref ret = ((jref(*)(u2, const slot_t *)) func)(frame->method->arg_slot_count, lvars);
    //     ostack_pushr(frame, ret);
    //     return;
    // }

    const utf8_t *desc = frame->method->native_simple_descriptor;

#undef B
#undef Z
#undef I
#undef F
#undef R
#undef J
#undef D

#define B(name) jbyte name = slot_get_byte(lvars++);
#define Z(name) jbool name = slot_get_bool(lvars++);
#define I(name) jint name = slot_get_int(lvars++);
#define F(name) jfloat name = slot_get_float(lvars++);
#define R(name) jref name = slot_get_ref(lvars++);
#define J(name) jlong name = slot_get_long(lvars); lvars += 2;
#define D(name) jdouble name = slot_get_double(lvars); lvars += 2;

#define INVOKE_0(_desc, func_type, push_func) \
    if (desc == _desc) { \
        push_func(frame, ((func_type) func)()); \
        return; \
    }

#define INVOKE_1(_desc, func_type, arg, push_func) \
    if (desc == _desc) { \
        arg(a) \
        push_func(frame, ((func_type) func)(a)); \
        return; \
    }

#define INVOKE_2(_desc, func_type, arg1, arg2, push_func) \
    if (desc == _desc) { \
        arg1(a) arg2(b) \
        push_func(frame, ((func_type) func)(a, b)); \
        return; \
    }

#define INVOKE_3(_desc, func_type, arg1, arg2, arg3, push_func) \
    if (desc == _desc) { \
        arg1(a) arg2(b) arg3(c) \
        push_func(frame, ((func_type) func)(a, b, c)); \
        return; \
    }

#define INVOKE_4(_desc, func_type, arg1, arg2, arg3, arg4, push_func) \
    if (desc == _desc) { \
        arg1(a) arg2(b) arg3(c) arg4(d) \
        push_func(frame, ((func_type) func)(a, b, c, d)); \
        return; \
    }

#define INVOKE_5(_desc, func_type, arg1, arg2, arg3, arg4, arg5, push_func) \
    if (desc == _desc) { \
        arg1(a) arg2(b) arg3(c) arg4(d) arg5(e) \
        push_func(frame, ((func_type) func)(a, b, c, d, e)); \
        return; \
    }

#define INVOKE_7(_desc, func_type, arg1, arg2, arg3, arg4, arg5, arg6, arg7, push_func) \
    if (desc == _desc) { \
        arg1(a) arg2(b) arg3(c) arg4(d) arg5(e) arg6(f) arg7(g) \
        push_func(frame, ((func_type) func)(a, b, c, d, e, f, g)); \
        return; \
    }

    INVOKE_0(S(V_V), void(*)(), )
    INVOKE_0(S(V_Z), jbool(*)(), ostack_pushi)
    INVOKE_0(S(V_I), jint(*)(), ostack_pushi)
    INVOKE_0(S(V_R), jref(*)(), ostack_pushr)
    INVOKE_0(S(V_J), jlong(*)(), ostack_pushl)

    INVOKE_1(S(I_V), void(*)(jint), I, )
    INVOKE_1(S(R_V), void(*)(jref), R, )
    INVOKE_1(S(J_V), void(*)(jlong), J, )
    INVOKE_1(S(Z_R), jref(*)(jbool), Z, ostack_pushr)
    INVOKE_1(S(I_I), jint(*)(jint), I, ostack_pushi)
    INVOKE_1(S(I_Z), jbool(*)(jint), I, ostack_pushi)
    INVOKE_1(S(I_J), jlong(*)(jint), I, ostack_pushl)
    INVOKE_1(S(D_J), jlong(*)(jdouble), D, ostack_pushl)
    INVOKE_1(S(J_D), jdouble(*)(jlong), J, ostack_pushd)
    INVOKE_1(S(J_J), jlong(*)(jlong), J, ostack_pushl)
    INVOKE_1(S(J_B), jbyte(*)(jlong), J, ostack_pushi)
    INVOKE_1(S(F_I), jint(*)(jfloat), F, ostack_pushi)
    INVOKE_1(S(R_R), jref(*)(jref), R, ostack_pushr)
    INVOKE_1(S(R_Z), jbool(*)(jref), R, ostack_pushi)
    INVOKE_1(S(R_I), jint(*)(jref), R, ostack_pushi)
    INVOKE_1(S(R_J), jlong(*)(jref), R, ostack_pushl)

    INVOKE_2(S(JJ_V), void(*)(jlong, jlong), J, J, )
    INVOKE_2(S(RZ_V), void(*)(jref, jbool), R, Z, )
    INVOKE_2(S(RI_V), void(*)(jref, jint), R, I, )
    INVOKE_2(S(RR_V), void(*)(jref, jref), R, R, )
    INVOKE_2(S(RJ_V), void(*)(jref, jlong), R, J, )
    INVOKE_2(S(IJ_J), jlong(*)(jint, jlong), I, J, ostack_pushl)
    INVOKE_2(S(RI_R), jref(*)(jref, jint), R, I, ostack_pushr)
    INVOKE_2(S(RI_J), jlong(*)(jref, jint), R, I, ostack_pushl)
    INVOKE_2(S(RZ_Z), jbool(*)(jref, jbool), R, Z, ostack_pushi)
    INVOKE_2(S(RR_Z), jbool(*)(jref, jref), R, R, ostack_pushi)
    INVOKE_2(S(RR_I), jint(*)(jref, jref), R, R, ostack_pushi)
    INVOKE_2(S(RR_J), jlong(*)(jref, jref), R, R, ostack_pushl)
    INVOKE_2(S(RJ_B), jbyte(*)(jref, jlong), R, J, ostack_pushi)
    INVOKE_2(S(RJ_I), jint(*)(jref, jlong), R, J, ostack_pushi)
    INVOKE_2(S(RJ_R), jref(*)(jref, jlong), R, J, ostack_pushr)
    INVOKE_2(S(RJ_J), jlong(*)(jref, jlong), R, J, ostack_pushl)
    INVOKE_2(S(RZ_R), jref(*)(jref, jbool), R, Z, ostack_pushr)
    INVOKE_2(S(RR_R), jref(*)(jref, jref), R, R, ostack_pushr)

    INVOKE_3(S(RIR_V), void(*)(jref, jint, jref), R, I, R, )
    INVOKE_3(S(RJJ_V), void(*)(jref, jlong, jlong), R, J, J, )
    INVOKE_3(S(RRZ_V), void(*)(jref, jref, jbool), R, R, Z, )
    INVOKE_3(S(RRZ_R), jref(*)(jref, jref, jbool), R, R, Z, ostack_pushr)
    INVOKE_3(S(RRR_R), jref(*)(jref, jref, jref), R, R, R, ostack_pushr)
    INVOKE_3(S(RRR_Z), jbool(*)(jref, jref, jref), R, R, R, ostack_pushi)
    INVOKE_3(S(RRR_J), jlong(*)(jref, jref, jref), R, R, R, ostack_pushl)
    INVOKE_3(S(RRJ_R), jref(*)(jref, jref, jlong), R, R, J, ostack_pushr)
    INVOKE_3(S(RRJ_I), jint(*)(jref, jref, jlong), R, R, J, ostack_pushi)

    INVOKE_4(S(RIIZ_V), void(*)(jref, jint, jint, jbool), R, I, I, Z, )
    INVOKE_4(S(RRJR_V), void(*)(jref, jref, jlong, jref), R, R, J, R, )
    INVOKE_4(S(RJII_Z), jbool(*)(jref, jlong, jint, jint), R, J, I, I, ostack_pushi)
    INVOKE_4(S(RRII_I), jint(*)(jref, jref, jint, jint), R, R, I, I, ostack_pushi)
    INVOKE_4(S(RJJJ_Z), jbool(*)(jref, jlong, jlong, jlong), R, J, J, J, ostack_pushi)
    INVOKE_4(S(RZRR_R), jref(*)(jref, jbool, jref, jref), R, Z, R, R, ostack_pushr)
    INVOKE_4(S(RRRR_R), jref(*)(jref, jref, jref, jref), R, R, R, R, ostack_pushr)

    INVOKE_5(S(RRIIZ_V), void(*)(jref, jref, jint, jint, jbool), R, R, I, I, Z, )
    INVOKE_5(S(RIRII_V), void(*)(jref, jint, jref, jint, jint), R, I, R, I, I, )
    INVOKE_5(S(RRJJJ_Z), jbool(*)(jref, jref, jlong, jlong, jlong), R, R, J, J, J, ostack_pushi)
    INVOKE_5(S(RRJRR_Z), jbool(*)(jref, jref, jlong, jref, jref), R, R, J, R, R, ostack_pushi)
    INVOKE_5(S(RRIIJ_R), jref(*)(jref, jref, jint, jint, jlong), R, R, I, I, J, ostack_pushr)
    INVOKE_5(S(RRJII_Z), jbool(*)(jref, jref, jlong, jint, jint), R, R, J, I, I, ostack_pushi)

    INVOKE_7(S(RRRIIRR_R), jref(*)(jref, jref, jref, jint, jint, jref, jref),
             R, R, R, I, I, R, R, ostack_pushr)
    INVOKE_7(S(RRRIRIR_I), jint(*)(jref, jref, jref, jint, jref, jint, jref),
             R, R, R, I, R, I, R, ostack_pushi)

    JVM_PANIC("error.");
    // JVM_PANIC((get_method_info(frame->method) + ", " + frame->method->native_method->type.name()).c_str()); todo
//    throw java_lang_VirtualMachineError(string("未实现的方法类型: ") + frame->method->toString());
}
