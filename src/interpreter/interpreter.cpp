/*
 * Author: kayo
 */

#include <iostream>
#include <cmath>
#include "interpreter.h"
#include "../kayo.h"
#include "../debug.h"
#include "../runtime/thread_info.h"
#include "../runtime/Frame.h"
#include "../classfile/constant.h"
#include "../objects/class.h"
#include "../objects/invoke.h"

using namespace std;
using namespace utf8;
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
#define TRACE PRINT_TRACE
#define PRINT_OPCODE TRACE("%d(0x%x), %s, pc = %d\n", opcode, opcode, instruction_names[opcode], (int)frame->reader.pc);
#else
#define TRACE(...)
#define PRINT_OPCODE
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

    jref _this = frame->method->isStatic() ? (jref) clazz : RSLOT(lvars);

#define CHANGE_FRAME(newFrame) \
    do { \
        /*frame->ostack = ostack;  stack指针在变动，需要设置一下 todo */ \
        frame = newFrame; \
        reader = &frame->reader; \
        clazz = frame->method->clazz; \
        cp = &frame->method->clazz->cp; \
        ostack = frame->ostack; \
        lvars = frame->lvars; \
        _this = frame->method->isStatic() ? (jref) clazz : RSLOT(lvars); \
        TRACE("executing frame: %s\n", frame->toString().c_str()); \
    } while (false)

    while (true) {
        u1 opcode = reader->readu1();
        PRINT_OPCODE
        switch (opcode) {
            case OPC_NOP:
                break;
            case OPC_ACONST_NULL:
                frame->pushr(jnull);
                break;
            case OPC_ICONST_M1:
                frame->pushi(-1);
                break;
            case OPC_ICONST_0:
                frame->pushi(0);
                break;
            case OPC_ICONST_1:
                frame->pushi(1);
                break;
            case OPC_ICONST_2:
                frame->pushi(2);
                break;
            case OPC_ICONST_3:
                frame->pushi(3);
                break;
            case OPC_ICONST_4:
                frame->pushi(4);
                break;
            case OPC_ICONST_5:
                frame->pushi(5);
                break;
            case OPC_LCONST_0:
                frame->pushl(0);
                break;
            case OPC_LCONST_1:
                frame->pushl(1);
                break;
            case OPC_FCONST_0:
                frame->pushf(0);
                break;
            case OPC_FCONST_1:
                frame->pushf(1);
                break;
            case OPC_FCONST_2:
                frame->pushf(2);
                break;
            case OPC_DCONST_0:
                frame->pushd(0);
                break;
            case OPC_DCONST_1:
                frame->pushd(1);
                break;
            case OPC_BIPUSH: // bipush, Byte Integer push
                frame->pushi(reader->readu1());
                break;
            case OPC_SIPUSH: // Short Integer push
                frame->pushi(reader->readu2());
                break;
{
            u2 index;
            case OPC_LDC:
                index = reader->readu1();
                goto __ldc;
            case OPC_LDC_W:
                index = reader->readu2();
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
                break;
}
            case OPC_LDC2_W: {
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
                break;
            }
            case OPC_ILOAD:
            case OPC_FLOAD:
            case OPC_ALOAD: {
                u1 index = reader->readu1();
                *frame->ostack++ = lvars[index];
                break;
            }
            case OPC_LLOAD:
            case OPC_DLOAD: {
                u1 index = reader->readu1();
                *frame->ostack++ = lvars[index];
                *frame->ostack++ = lvars[index + 1];
                break;
            }
            case OPC_ILOAD_0:
            case OPC_FLOAD_0:
            case OPC_ALOAD_0:
                *frame->ostack++ = lvars[0];
                break;
            case OPC_ILOAD_1:
            case OPC_FLOAD_1:
            case OPC_ALOAD_1:
                *frame->ostack++ = lvars[1];
                break;
            case OPC_ILOAD_2:
            case OPC_FLOAD_2:
            case OPC_ALOAD_2:
                *frame->ostack++ = lvars[2];
                break;
            case OPC_ILOAD_3:
            case OPC_FLOAD_3:
            case OPC_ALOAD_3:
                *frame->ostack++ = lvars[3];
                break;
            case OPC_LLOAD_0:
            case OPC_DLOAD_0:
                *frame->ostack++ = lvars[0];
                *frame->ostack++ = lvars[1];
                break;
            case OPC_LLOAD_1:
            case OPC_DLOAD_1:
                *frame->ostack++ = lvars[1];
                *frame->ostack++ = lvars[2];
                break;
            case OPC_LLOAD_2:
            case OPC_DLOAD_2:
                *frame->ostack++ = lvars[2];
                *frame->ostack++ = lvars[3];
                break;
            case OPC_LLOAD_3:
            case OPC_DLOAD_3:
                *frame->ostack++ = lvars[3];
                *frame->ostack++ = lvars[4];
                break;
#define GET_AND_CHECK_ARRAY \
    jint index = frame->popi(); \
    auto arr = (Array *) frame->popr(); \
    if ((arr) == jnull) \
        thread_throw(new NullPointerException); \
    if (!arr->checkBounds(index)) \
        thread_throw(new ArrayIndexOutOfBoundsException);
//#define ARRAY_LOAD_CATEGORY_ONE(type, t) \
//{ \
//    GET_AND_CHECK_ARRAY \
//    auto value = arr->get<type>(index); \
//    frame->push##t(value); \
//    break; \
//}
            case OPC_IALOAD: {
                GET_AND_CHECK_ARRAY
                auto value = arr->get<jint>(index);
                frame->pushi(value);
                break;
            }
            case OPC_FALOAD: {
                GET_AND_CHECK_ARRAY
                auto value = arr->get<jfloat>(index);
                frame->pushf(value);
                break;
            }
            case OPC_AALOAD: {
                GET_AND_CHECK_ARRAY
                auto value = arr->get<jref>(index);
                frame->pushr(value);
                break;
            }
            case OPC_BALOAD: {
                GET_AND_CHECK_ARRAY
                jint value = arr->get<jbyte>(index);
                frame->pushi(value);
                break;
            }
            case OPC_CALOAD: {
                GET_AND_CHECK_ARRAY
                jint value = arr->get<jchar>(index);
                frame->pushi(value);
                break;
            }
            case OPC_SALOAD: {
                GET_AND_CHECK_ARRAY
                jint value = arr->get<jshort>(index);
                frame->pushi(value);
                break;
            }
            case OPC_LALOAD: {
                GET_AND_CHECK_ARRAY
                auto value = arr->get<jlong>(index);
                frame->pushl(value);
                break;
            }
            case OPC_DALOAD: {
                GET_AND_CHECK_ARRAY
                auto value = arr->get<jdouble>(index);
                frame->pushd(value);
                break;
            }
            case OPC_ISTORE:
            case OPC_FSTORE:
            case OPC_ASTORE: {
                u1 index = reader->readu1();
                lvars[index] = *--frame->ostack;
                break;
            }
            case OPC_LSTORE:
            case OPC_DSTORE: {
                u1 index = reader->readu1();
                lvars[index + 1] = *--frame->ostack;
                lvars[index] = *--frame->ostack;
                break;
            }
            case OPC_ISTORE_0:
            case OPC_FSTORE_0:
            case OPC_ASTORE_0:
                lvars[0] = *--frame->ostack;
                break;
            case OPC_ISTORE_1:
            case OPC_FSTORE_1:
            case OPC_ASTORE_1:
                lvars[1] = *--frame->ostack;
                break;
            case OPC_ISTORE_2:
            case OPC_FSTORE_2:
            case OPC_ASTORE_2:
                lvars[2] = *--frame->ostack;
                break;
            case OPC_ISTORE_3:
            case OPC_FSTORE_3:
            case OPC_ASTORE_3:
                lvars[3] = *--frame->ostack;
                break;
            case OPC_LSTORE_0:
            case OPC_DSTORE_0:
                lvars[1] = *--frame->ostack;
                lvars[0] = *--frame->ostack;
                break;
            case OPC_LSTORE_1:
            case OPC_DSTORE_1:
                lvars[2] = *--frame->ostack;
                lvars[1] = *--frame->ostack;
                break;
            case OPC_LSTORE_2:
            case OPC_DSTORE_2:
                lvars[3] = *--frame->ostack;
                lvars[2] = *--frame->ostack;
                break;
            case OPC_LSTORE_3:
            case OPC_DSTORE_3:
                lvars[4] = *--frame->ostack;
                lvars[3] = *--frame->ostack;
                break;
//#define ARRAY_STORE_CATEGORY_ONE(type, t) \
//{ \
//    auto value = frame->pop##t(); \
//    GET_AND_CHECK_ARRAY \
//    arr->set(index, value); \
//    break; \
//}
            case OPC_IASTORE: {
                auto value = frame->popi();
                GET_AND_CHECK_ARRAY
                arr->set<jint>(index, value);
                break;
            }
            case OPC_FASTORE: {
                auto value = frame->popf();
                GET_AND_CHECK_ARRAY
                arr->set<jfloat>(index, value);
                break;
            }
            case OPC_AASTORE: {
                auto value = frame->popr();
                GET_AND_CHECK_ARRAY
                arr->set<jref>(index, value);
                break;
            }
            case OPC_BASTORE: {
                auto value = frame->popi();
                GET_AND_CHECK_ARRAY
                arr->set<jbyte>(index, (jbyte) value);
                break;
            }
            case OPC_CASTORE: {
                auto value = frame->popi();
                GET_AND_CHECK_ARRAY
                arr->set<jchar>(index, (jchar) value);
                break;
            }
            case OPC_SASTORE: {
                auto value = frame->popi();
                GET_AND_CHECK_ARRAY
                arr->set<jshort>(index, (jshort) value);
                break;
            }
            case OPC_LASTORE: {
                auto value = frame->popl();
                GET_AND_CHECK_ARRAY
                arr->set<jlong>(index, value);
                break;
            }
            case OPC_DASTORE: {
                auto value = frame->popd();
                GET_AND_CHECK_ARRAY
                arr->set<jdouble>(index, value);
                break;
            }
#undef GET_AND_CHECK_ARRAY
            case OPC_POP:
                frame->ostack--;
                break;
            case OPC_POP2:
                frame->ostack -= 2;
                break;
            case OPC_DUP:
                frame->ostack[0] = frame->ostack[-1];
                frame->ostack++;
                break;
            case OPC_DUP_X1:
                frame->ostack[0] = frame->ostack[-1];
                frame->ostack[-1] = frame->ostack[-2];
                frame->ostack[-2] = frame->ostack[0];
                frame->ostack++;
                break;
            case OPC_DUP_X2:
                frame->ostack[0] = frame->ostack[-1];
                frame->ostack[-1] = frame->ostack[-2];
                frame->ostack[-2] = frame->ostack[-3];
                frame->ostack[-3] = frame->ostack[0];
                frame->ostack++;
                break;
            case OPC_DUP2:
                frame->ostack[0] = frame->ostack[-2];
                frame->ostack[1] = frame->ostack[-1];
                frame->ostack += 2;
                break;
            case OPC_DUP2_X1:
                // ..., value3, value2, value1 →
                // ..., value2, value1, value3, value2, value1
                frame->ostack[1] = frame->ostack[-1];
                frame->ostack[0] = frame->ostack[-2];
                frame->ostack[-1] = frame->ostack[-3];
                frame->ostack[-2] = frame->ostack[1];
                frame->ostack[-3] = frame->ostack[0];
                frame->ostack += 2;
                break;
            case OPC_DUP2_X2:
                // ..., value4, value3, value2, value1 →
                // ..., value2, value1, value4, value3, value2, value1
                frame->ostack[1] = frame->ostack[-1];
                frame->ostack[0] = frame->ostack[-2];
                frame->ostack[-1] = frame->ostack[-3];
                frame->ostack[-2] = frame->ostack[-4];
                frame->ostack[-3] = frame->ostack[1];
                frame->ostack[-4] = frame->ostack[0];
                frame->ostack += 2;
                break;
            case OPC_SWAP:
                swap(frame->ostack[-1], frame->ostack[-2]);
                break;

#define BINARY_OP(type, t, oper) \
{ \
    type v2 = frame->pop##t();\
    type v1 = frame->pop##t();\
    frame->push##t(v1 oper v2); \
    break; \
}
            case OPC_IADD:
                BINARY_OP(jint, i, +);
            case OPC_LADD:
                BINARY_OP(jlong, l, +);
            case OPC_FADD:
                BINARY_OP(jfloat, f, +);
            case OPC_DADD:
                BINARY_OP(jdouble, d, +);
            case OPC_ISUB:
                BINARY_OP(jint, i, -);
            case OPC_LSUB:
                BINARY_OP(jlong, l, -);
            case OPC_FSUB:
                BINARY_OP(jfloat, f, -);
            case OPC_DSUB:
                BINARY_OP(jdouble, d, -);
            case OPC_IMUL:
                BINARY_OP(jint, i, *);
            case OPC_LMUL:
                BINARY_OP(jlong, l, *);
            case OPC_FMUL:
                BINARY_OP(jfloat, f, *);
            case OPC_DMUL:
                BINARY_OP(jdouble, d, *);
            case OPC_IDIV:
                BINARY_OP(jint, i, /);
            case OPC_LDIV:
                BINARY_OP(jlong, l, /);
            case OPC_FDIV:
                BINARY_OP(jfloat, f, /);
            case OPC_DDIV:
                BINARY_OP(jdouble, d, /);
            case OPC_IREM:
                BINARY_OP(jint, i, %);
            case OPC_LREM:
                BINARY_OP(jlong, l, %);
            case OPC_FREM: {
                jfloat v2 = frame->popf();
   				jfloat v1 = frame->popf();
                frame->pushf(fmod(v1, v2));
                break;
            }
            case OPC_DREM: {
                jdouble v2 = frame->popd();
                jdouble v1 = frame->popd();
                frame->pushd(fmod(v1, v2));
                break;
            }
            case OPC_INEG:
                frame->pushi(-frame->popi());
                break;
            case OPC_LNEG:
                frame->pushl(-frame->popl());
                break;
            case OPC_FNEG:
                frame->pushf(-frame->popf());
                break;
            case OPC_DNEG:
                frame->pushd(-frame->popd());
                break; 
            case OPC_ISHL: {
                // 与0x1f是因为低5位表示位移距离，位移距离实际上被限制在0到31之间。
                jint shift = frame->popi() & 0x1f;
                jint ivalue = frame->popi();
                frame->pushi(ivalue << shift);
                break;
            }
            case OPC_LSHL: {
                // 与0x3f是因为低6位表示位移距离，位移距离实际上被限制在0到63之间。
                jint shift = frame->popi() & 0x3f;
			    jlong lvalue = frame->popl();
			    frame->pushl(lvalue << shift);
                break;
            }
            case OPC_ISHR: {
                // 逻辑右移 shift logical right
			    jint shift = frame->popi() & 0x1f;
			    jint ivalue = frame->popi();
			    frame->pushi((~(((jint)1) >> shift)) & (ivalue >> shift));
                break;
            }
            case OPC_LSHR: {
			    jint shift = frame->popi() & 0x3f;
			    jlong lvalue = frame->popl();
			    frame->pushl((~(((jlong)1) >> shift)) & (lvalue >> shift));
                break;
            }
            case OPC_IUSHR: {
                // 算术右移 shift arithmetic right
                jint shift = frame->popi() & 0x1f;
                jint ivalue = frame->popi();
                frame->pushi(ivalue >> shift);
                break;
            }
            case OPC_LUSHR: {
			    jint shift = frame->popi() & 0x3f;
			    jlong lvalue = frame->popl();
			    frame->pushl(lvalue >> shift);
                break;
            }
            case OPC_IAND:
                BINARY_OP(jint, i, &);
            case OPC_LAND:
                BINARY_OP(jlong, l, &);
            case OPC_IOR:
                BINARY_OP(jint, i, |);
            case OPC_LOR:
                BINARY_OP(jlong, l, |);
            case OPC_IXOR:
                BINARY_OP(jint, i, ^);
            case OPC_LXOR:
                BINARY_OP(jlong, l, ^);
#undef BINARY_OP
            case OPC_IINC: {
                u1 index = reader->readu1();
                ISLOT(lvars + index) = ISLOT(lvars + index) + reader->reads1();
                break;
            }
            case OPC_I2L:
                frame->pushl(frame->popi());
                break;
            case OPC_I2F:
                frame->pushf(frame->popi());
                break;
            case OPC_I2D:
                frame->pushd(frame->popi());
                break;
            case OPC_L2I:
                frame->pushi((jint) frame->popl());
                break;
            case OPC_L2F:
                frame->pushf((jfloat) frame->popl());
                break;
            case OPC_L2D:
                frame->pushd(frame->popl());
                break;
            case OPC_F2I:
               frame->pushi((jint) frame->popf());
                break;
            case OPC_F2L:
                frame->pushl((jlong) frame->popf());
                break;
            case OPC_F2D:
                frame->pushd(frame->popf());
                break;
            case OPC_D2I:
                frame->pushi((jint) frame->popd());
                break;
            case OPC_D2L:
                frame->pushl((jlong) frame->popd());
                break;
            case OPC_D2F:
                frame->pushf((jfloat) frame->popd());
                break;
            case OPC_I2B:
                frame->pushi(jint2jbyte(frame->popi()));
                break;
            case OPC_I2C:
                frame->pushi(jint2jchar(frame->popi()));
                break;
            case OPC_I2S:
                frame->pushi(jint2jshort(frame->popi()));
                break;
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
    break; \
}
            case OPC_LCMP:
                CMP(jlong, l, DO_CMP(v1, v2, -1));
            case OPC_FCMPL:
                CMP(jfloat, f, DO_CMP(v1, v2, -1));
            case OPC_FCMPG:
                CMP(jfloat, f, DO_CMP(v1, v2, 1));
            case OPC_DCMPL:
                CMP(jdouble, d, DO_CMP(v1, v2, -1));
            case OPC_DCMPG:
                CMP(jdouble, d, DO_CMP(v1, v2, 1));

#define IF_COND(cond) \
{ \
    jint v = frame->popi(); \
    jint offset = reader->reads2(); \
    if (v cond 0) \
        reader->skip(offset - 3);  /* minus instruction length */ \
    break; \
}
            case OPC_IFEQ:
                IF_COND(==);
            case OPC_IFNE:
                IF_COND(!=);
            case OPC_IFLT:
                IF_COND(<);
            case OPC_IFGE:
                IF_COND(>=);
            case OPC_IFGT:
                IF_COND(>);
            case OPC_IFLE:
                IF_COND(<=);
#undef IF_COND

//#define IF_CMP_COND(cond)
//{
//    frame->ostack -= 2;
//    jint offset = reader->reads2();
//    if (frame->ostack[0] cond frame->ostack[1])
//        reader->skip(offset - 3); /* minus instruction length */
//    break;
//}
#define IF_CMP_COND(t, cond) \
{ \
    s2 offset = reader->reads2(); \
    auto v2 = frame->pop##t(); \
    auto v1 = frame->pop##t(); \
    if (v1 cond v2) \
        reader->skip(offset - 3); /* minus instruction length */ \
    break; \
}
            case OPC_IF_ICMPEQ:
                IF_CMP_COND(i, ==);
            case OPC_IF_ACMPEQ:
                IF_CMP_COND(r, ==);
            case OPC_IF_ICMPNE:
                IF_CMP_COND(i, !=);
            case OPC_IF_ACMPNE:
                IF_CMP_COND(r, !=);
            case OPC_IF_ICMPLT:
                IF_CMP_COND(i, <);
            case OPC_IF_ICMPGE:
                 IF_CMP_COND(i, >=);
            case OPC_IF_ICMPGT:
                IF_CMP_COND(i, >);
            case OPC_IF_ICMPLE:
                IF_CMP_COND(i, <=);
#undef IF_CMP_COND

            case OPC_GOTO: {
                s2 offset = reader->reads2();
                reader->skip(offset - 3);  // minus instruction length
                break;
            }

            // 在Java 6之前，Oracle的Java编译器使用 jsr, jsr_w 和 ret 指令来实现 finally 子句。
            // 从Java 6开始，已经不再使用这些指令
            case OPC_JSR:
                thread_throw(new InternalError("jsr doesn't support after jdk 6."));
            case OPC_RET:
                thread_throw(new InternalError("ret doesn't support after jdk 6."));

            case OPC_TABLESWITCH: {
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

                break;
            }
            case OPC_LOOKUPSWITCH: {
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

                break;
            }                
{
            int ret_value_slots_count;
            case OPC_IRETURN:
            case OPC_FRETURN:
            case OPC_ARETURN:
                ret_value_slots_count = 1;
                goto __method_return;
            case OPC_LRETURN:
            case OPC_DRETURN:
                ret_value_slots_count = 2;
                goto __method_return;
            case OPC_RETURN:
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
//                        _this->unlock();
                    }
                    return ret_value;
                }

                for (int i = 0; i < ret_value_slots_count; i++) {
                    *invokeFrame->ostack++ = *ret_value++;
                }
                if (frame->method->isSynchronized()) {
//                    _this->unlock();
                }
                CHANGE_FRAME(invokeFrame);
                break;
}                
            case OPC_GETSTATIC: {
			    u2 index = reader->readu2();
			    Field *field = cp->resolveField(index);
			    assert(field->isStatic()); // todo

                initClass(field->clazz);

                *frame->ostack++ = field->staticValue.data[0];
                if (field->category_two) {
                    *frame->ostack++ = field->staticValue.data[1];
                }
                break;
            }
            case OPC_PUTSTATIC: {
			    u2 index = reader->readu2();
			    Field *field = cp->resolveField(index);
			    assert(field->isStatic()); // todo

                initClass(field->clazz);

                if (field->category_two) {
                    frame->ostack -= 2;
                    field->staticValue.data[0] = frame->ostack[0];
                    field->staticValue.data[1] = frame->ostack[1];
                } else {
                    field->staticValue.data[0] = *--frame->ostack;
                }

                break;
            }                
            case OPC_GETFIELD: {
			    u2 index = reader->readu2();
			    Field *field = cp->resolveField(index);
			    assert(!field->isStatic()); // todo

			    jref obj = frame->popr();
			    if (obj == jnull) {
			        thread_throw(new NullPointerException);
			    }

                *frame->ostack++ = obj->data[field->id];
                if (field->category_two) {
                    *frame->ostack++ = obj->data[field->id + 1];
                }
                break;
            }
            case OPC_PUTFIELD: {
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

                if (field->category_two) {
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
                break;
            }                   
            case OPC_INVOKEVIRTUAL: {
                // invokevirtual指令用于调用对象的实例方法，根据对象的实际类型进行分派（虚方法分派）。
                u2 index = reader->readu2();
                Method *m = cp->resolveMethod(index);

                frame->ostack -= m->arg_slot_count;
                jref obj = RSLOT(frame->ostack);
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
            case OPC_INVOKESPECIAL: {
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
                }

			    if (m->isAbstract()) {
			        thread_throw(new AbstractMethodError());
			    }
			    if (m->isStatic()) {
			        thread_throw(new IncompatibleClassChangeError());
			    }

			    frame->ostack -= m->arg_slot_count;
                jref obj = RSLOT(frame->ostack);
			    if (obj == jnull) {
			        thread_throw(new NullPointerException);
			    }


                resolved_method = m;
                goto __invoke_method;
            }
            case OPC_INVOKESTATIC: {
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
                goto __invoke_method;
            }            
            case OPC_INVOKEINTERFACE: {
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
                jref obj = RSLOT(frame->ostack);
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
            case OPC_INVOKEDYNAMIC: {
			    u2 index = reader->readu2(); // point to CONSTANT_InvokeDynamic_info
			    reader->readu1(); // this byte must always be zero.
			    reader->readu1(); // this byte must always be zero.

			    const utf8_t *invokedName = cp->invokeDynamicMethodName(index);
			    const utf8_t *invokedDescriptor = cp->invokeDynamicMethodType(index);

			    auto invokedType = fromMethodDescriptor(invokedDescriptor, clazz->loader);
			    auto caller = getCaller();

			    BootstrapMethod &bm = clazz->bootstrap_methods.at(cp->invokeDynamicBootstrapMethodIndex(index));
			    u2 refKind = cp->methodHandleReferenceKind(bm.bootstrapMethodRef);
			    u2 refIndex = cp->methodHandleReferenceIndex(bm.bootstrapMethodRef);

                switch (refKind) {
                    case REF_invokeStatic: {
                        const utf8_t *className = cp->methodClassName(refIndex);
                        Class *bootstrapClass = loadClass(clazz->loader, className);

			            // bootstrap method is static,  todo 对不对
			            // 前三个参数固定为 MethodHandles.Lookup caller, String invokedName, MethodType invokedType todo 对不对
			            // 后续的参数由 ref->argc and ref->args 决定
			            Method *bootstrapMethod = bootstrapClass->getDeclaredStaticMethod(
                                cp->methodName(refIndex), cp->methodType(refIndex));
			            // args's length is big enough,多余的长度无所谓，bootstrapMethod 会按需读取的。
			            slot_t args[3 + bm.bootstrapArguments.size() * 2];
                        RSLOT(args) = caller;
                        RSLOT(args + 1) = newString(invokedName);
                        RSLOT(args + 2) = invokedType;
			            bm.resolveArgs(cp, args + 3);
			            auto callSet = RSLOT(execJavaFunc(bootstrapMethod, args));

			            // public abstract MethodHandle dynamicInvoker()
			            auto dynInvoker = callSet->clazz->lookupInstMethod("dynamicInvoker", "()Ljava/lang/invoke/MethodHandle;");
			            auto exactMethodHandle = RSLOT(execJavaFunc(dynInvoker, callSet));

			            // public final Object invokeExact(Object... args) throws Throwable
			            Method *invokeExact = exactMethodHandle->clazz->lookupInstMethod(
			                    "invokeExact", "([Ljava/lang/Object;)Ljava/lang/Object;");
			            assert(invokeExact->isVarargs());
			            u2 slotsCount = Method::calArgsSlotsCount(invokedDescriptor, true);
			            slot_t __args[slotsCount];
                        RSLOT(__args) = exactMethodHandle; // __args[0] = (slot_t) exactMethodHandle;
			            slotsCount--; // 减去"this"
			            frame->ostack -= slotsCount; // pop all args
			            memcpy(__args + 1, frame->ostack, slotsCount * sizeof(slot_t));
			            // invoke exact method, invokedynamic completely execute over.
			            execJavaFunc(invokeExact, __args);

                        break;
                    }
                    case REF_newInvokeSpecial:
                        jvm_abort(" "); // todo
                        break;
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
//        _this->unlock(); // todo why unlock 而不是 lock ................................................
    }
    break;
}
            case OPC_NEW: {
                // new指令专门用来创建类实例。数组由专门的指令创建
                // 如果类还没有被初始化，会触发类的初始化。
                Class *c = cp->resolveClass(reader->readu2());
                initClass(c);

                if (c->isInterface() || c->isAbstract()) {
                    thread_throw(new InstantiationException());
                }

                frame->pushr(newObject(c));
                break;
            }
            case OPC_NEWARRAY: { // newarray
                // 创建一维基本类型数组。
                // 包括 boolean[], byte[], char[], short[], int[], long[], float[] 和 double[] 8种。
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
                break;
            }
            case OPC_ANEWARRAY: { // anewarray
                // 创建一维引用类型数组
                jint arr_len = frame->popi();
                if (arr_len < 0) {
                    thread_throw(new ArrayIndexOutOfBoundsException);
                }

			    u2 index = reader->readu2();
			    Class *ac = cp->resolveClass(index)->arrayClass();
			    frame->pushr(newArray(ac, arr_len));
                break;
            }
            case OPC_MULTIANEWARRAY: {
                /*
                 * 创建多维数组
                 * todo 注意这种情况，基本类型的多维数组 int[][][]
                 */
                u2 index = reader->readu2();
                Class *ac = cp->resolveClass(index);

                u1 dim = reader->readu1(); // 多维数组的维度
                if (dim < 1) { // 必须大于或等于1
                    // todo error
                    jvm_abort("dim < 1");
                }

                jint lens[dim];
                for (int i = 0; i < dim; i++) {
                    lens[i] = frame->popi();
                }
                frame->pushr(newMultiArray(ac, dim, lens));
                break;
            }           
            case OPC_ARRAYLENGTH: {
			    Object *o = frame->popr();
			    if (o == jnull) {
			        thread_throw(new NullPointerException);
			    }
			    if (!o->isArrayObject()) {
			        thread_throw(new UnknownError("not a array")); // todo
			    }
				
			    frame->pushi(((Array *) o)->len);
                break;
            }
            case OPC_ATHROW: {
__opc_athrow:
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
                        break;
                    }

                    if (frame->vm_invoke) {
                        // frame 由虚拟机调用，则将异常抛给虚拟机
                        throw Throwable(eo);
                    }

                    // frame 无法处理异常，弹出
                    thread->popFrame();

                    if (frame->prev == nullptr) {
                        // 虚拟机栈已空，还是无法处理异常
                        TRACE("uncaught exception: %s\n",eo->toString().c_str());
                        thread_uncaught_exception(eo);
                    }

                    TRACE("athrow: pop frame: %s\n", frame->toString().c_str());
                    CHANGE_FRAME(frame->prev);
                }
                break;
            }
            case OPC_CHECKCAST: {
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
                break;
            }
            case OPC_INSTANCEOF: {
                u2 index =  reader->readu2();
                Class *c = cp->resolveClass(index);

                jref obj = frame->popr();
                if (obj == jnull) {
                    frame->pushi(0);
                } else {
                    frame->pushi(obj->isInstanceOf(c) ? 1 : 0);
                }
                break;
            }
            case OPC_MONITORENTER: {
			    jref o = frame->popr();
			    if (o == jnull) {
			        thread_throw(new NullPointerException);
			    }
//			    o->lock();
                break;
            }
            case OPC_MONITOREXIT: {
			    jref o = frame->popr();
			    if (o == jnull) {
			        thread_throw(new NullPointerException);
			    }
//			    o->unlock();
                break;
            }
            case OPC_WIDE: {
                int __opcode = reader->readu1();
                PRINT_OPCODE
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
                break;
            }
            case OPC_IFNULL: {
			    s2 offset = reader->reads2();
			    if (frame->popr() == jnull) {
			        reader->skip(offset - 3); // minus instruction length
			    }
                break;
            }
            case OPC_IFNONNULL: {
			    s2 offset = reader->reads2();
			    if (frame->popr() != jnull) {
			        reader->skip(offset - 3); // minus instruction length
			    }
                break;
            }
            case OPC_GOTO_W: // todo
                thread_throw(new InternalError("goto_w doesn't support"));
                break;
            case OPC_JSR_W: // todo
                thread_throw(new InternalError("jsr_w doesn't support after jdk 6."));
                break;
            case OPC_BREAKPOINT: // todo
                thread_throw(new InternalError("breakpoint doesn't support in this jvm."));
                break;
            case OPC_INVOKENATIVE: {
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
			        goto __opc_athrow;
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
                break;
            }
            case OPC_IMPDEP2:
                jvm_abort("This instruction isn't used.\n"); // todo
                break;            
            default:
                jvm_abort("This instruction isn't used. %d(0x%x)\n", opcode, opcode); // todo
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
    RSLOT(realArgs) = _this;
    int k = 1; // 第0位已经存放_this了，下面从第一位开始。
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
