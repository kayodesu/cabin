/*
 * Author: Jia Yang
 */

#include "../jvm.h"
#include "../rtda/thread/thread.h"
#include "../rtda/thread/frame.h"
#include "../instruction/constants.h"
#include "../instruction/stacks.h"
#include "../instruction/maths.h"
#include "../instruction/conversions.h"
#include "../instruction/comparisons.h"
#include "../instruction/controls.h"
#include "../instruction/references.h"
#include "../instruction/extended.h"

#if (JVM_DEBUG)
// the mapping of instructions's code and name
static char* instruction_names[] = {
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
        [0xcb ... 0xfd] = "notused",
        "impdep1", "impdep2",
};
#endif


/*
 * 标识当前指令是否有 wide 扩展。
 * 只对
 * iload, fload, aload, lload, dload,
 * istore, fstore, astore, lstore, dstore,
 * ret, iinc
 * 有效。
 * 以上指令执行前需检查此标志，执行后需复位（置为false）此标志。
 */
bool wide_extending = false; // todo 多线程并发访问有问题！！！！！！！！！！

jint fetch_index(struct frame *frame)
{
    if (wide_extending) {
        wide_extending = false; // recover
        return bcr_readu2(&frame->reader);
    }
    return bcr_readu1(&frame->reader);
}

void* interpret(void *thread0)
{
    assert(thread0 != NULL);
    struct thread *thread = thread0;

    while (!jthread_is_stack_empty(thread)) {
        struct frame *frame = jthread_top_frame(thread);
        if (frame->type == SF_TYPE_SHIM) {
            if (frame->m.shim_action != NULL) {
                frame->m.shim_action(frame);
            }

            jthread_pop_frame(thread);
            frame_destroy(frame);
            printvm_debug("shim frame(%p) exe over, destroy.\n", frame);
            continue;
        }

        printvm_debug("executing frame(%p): %s, pc = %lu\n", frame, frame_to_string(frame), frame->reader.pc);
        while (frame_has_more(frame)) {
            u1 opcode = frame_readu1(frame);

            printvm_debug("%d(0x%x), %s, pc = %lu\n", opcode, opcode, instruction_names[opcode], frame->reader.pc);

            switch (opcode) {
                case 0x00: break; // nop

                // Constants
                case 0x01: frame_stack_pushr(frame, NULL); break; // aconst_null

                case 0x02: frame_stack_pushi(frame, -1); break;   // iconst_m1
                case 0x03: frame_stack_pushi(frame, 0); break; // iconst_0
                case 0x04: frame_stack_pushi(frame, 1); break; // iconst_1
                case 0x05: frame_stack_pushi(frame, 2); break; // iconst_2
                case 0x06: frame_stack_pushi(frame, 3); break; // iconst_3
                case 0x07: frame_stack_pushi(frame, 4); break; // iconst_4
                case 0x08: frame_stack_pushi(frame, 5); break; // iconst_5

                case 0x09: frame_stack_pushl(frame, 0); break; // lconst_0
                case 0x0a: frame_stack_pushl(frame, 1); break; // lconst_1

                case 0x0b: frame_stack_pushf(frame, 0); break; // fconst_0
                case 0x0c: frame_stack_pushf(frame, 1); break; // fconst_1
                case 0x0d: frame_stack_pushf(frame, 2); break; // fconst_2

                case 0x0e: frame_stack_pushd(frame, 0); break; // dconst_0
                case 0x0f: frame_stack_pushd(frame, 1); break; // dconst_1

                case 0x10: frame_stack_pushi(frame, frame_readu1(frame)); break; // bipush, Byte Integer push
                case 0x11: frame_stack_pushi(frame, frame_readu2(frame)); break; // sipush, Short Integer push
                case 0x12: __ldc(frame, frame_readu1(frame)); break; // ldc
                case 0x13: __ldc(frame, frame_readu2(frame)); break; // ldc_w
                case 0x14: ldc2_w(frame); break; // ldc2_w

                // Loads
                case 0x15: frame_iload(frame, fetch_index(frame)); break; // iload
                case 0x16: frame_lload(frame, fetch_index(frame)); break; // lload
                case 0x17: frame_fload(frame, fetch_index(frame)); break; // fload
                case 0x18: frame_dload(frame, fetch_index(frame)); break; // dload
                case 0x19: frame_aload(frame, fetch_index(frame)); break; // aload

                case 0x1a: frame_iload(frame, 0); break; // iload_0
                case 0x1b: frame_iload(frame, 1); break; // iload_1
                case 0x1c: frame_iload(frame, 2); break; // iload_2
                case 0x1d: frame_iload(frame, 3); break; // iload_3

                case 0x1e: frame_lload(frame, 0); break; // lload_0
                case 0x1f: frame_lload(frame, 1); break; // lload_1
                case 0x20: frame_lload(frame, 2); break; // lload_2
                case 0x21: frame_lload(frame, 3); break; // lload_3

                case 0x22: frame_fload(frame, 0); break; // fload_0
                case 0x23: frame_fload(frame, 1); break; // fload_1
                case 0x24: frame_fload(frame, 2); break; // fload_2
                case 0x25: frame_fload(frame, 3); break; // fload_3

                case 0x26: frame_dload(frame, 0); break; // dload_0
                case 0x27: frame_dload(frame, 1); break; // dload_1
                case 0x28: frame_dload(frame, 2); break; // dload_2
                case 0x29: frame_dload(frame, 3); break; // dload_3

                case 0x2a: frame_aload(frame, 0); break; // aload_0
                case 0x2b: frame_aload(frame, 1); break; // aload_1
                case 0x2c: frame_aload(frame, 2); break; // aload_2
                case 0x2d: frame_aload(frame, 3); break; // aload_3

                case 0x2e: frame_iaload(frame); break; // iaload
                case 0x2f: frame_laload(frame); break; // laload
                case 0x30: frame_faload(frame); break; // faload
                case 0x31: frame_daload(frame); break; // daload
                case 0x32: frame_aaload(frame); break; // aaload
                case 0x33: frame_baload(frame); break; // baload
                case 0x34: frame_caload(frame); break; // caload
                case 0x35: frame_saload(frame); break; // saload

                // Stores
                case 0x36: frame_istore(frame, fetch_index(frame)); break; // istore
                case 0x37: frame_lstore(frame, fetch_index(frame)); break; // lstore
                case 0x38: frame_fstore(frame, fetch_index(frame)); break; // fstore
                case 0x39: frame_dstore(frame, fetch_index(frame)); break; // dstore
                case 0x3a: frame_astore(frame, fetch_index(frame)); break; // astore

                case 0x3b: frame_istore(frame, 0); break; // istore_0
                case 0x3c: frame_istore(frame, 1); break; // istore_1
                case 0x3d: frame_istore(frame, 2); break; // istore_2
                case 0x3e: frame_istore(frame, 3); break; // istore_3

                case 0x3f: frame_lstore(frame, 0); break; // lstore_0
                case 0x40: frame_lstore(frame, 1); break; // lstore_1
                case 0x41: frame_lstore(frame, 2); break; // lstore_2
                case 0x42: frame_lstore(frame, 3); break; // lstore_3

                case 0x43: frame_fstore(frame, 0); break; // fstore_0
                case 0x44: frame_fstore(frame, 1); break; // fstore_1
                case 0x45: frame_fstore(frame, 2); break; // fstore_2
                case 0x46: frame_fstore(frame, 3); break; // fstore_3

                case 0x47: frame_dstore(frame, 0); break; // dstore_0
                case 0x48: frame_dstore(frame, 1); break; // dstore_1
                case 0x49: frame_dstore(frame, 2); break; // dstore_2
                case 0x4a: frame_dstore(frame, 3); break; // dstore_3

                case 0x4b: frame_astore(frame, 0); break; // astore_0
                case 0x4c: frame_astore(frame, 1); break; // astore_1
                case 0x4d: frame_astore(frame, 2); break; // astore_2
                case 0x4e: frame_astore(frame, 3); break; // astore_3

                case 0x4f: frame_iastore(frame); break; // iastore
                case 0x50: frame_lastore(frame); break; // lastore
                case 0x51: frame_fastore(frame); break; // fastore
                case 0x52: frame_dastore(frame); break; // dastore
                case 0x53: frame_aastore(frame); break; // aastore
                case 0x54: frame_bastore(frame); break; // bastore
                case 0x55: frame_castore(frame); break; // castore
                case 0x56: frame_sastore(frame); break; // sastore

                // Stack
                case 0x57: frame->stack_top--; break;    // pop
                case 0x58: frame->stack_top -= 2; break; // pop2
                case 0x59: dup(frame); break;            // dup
                case 0x5a: dup_x1(frame); break;         // dup_x1
                case 0x5b: dup_x2(frame); break;         // dup_x2
                case 0x5c: dup2(frame);  break;          // dup2
                case 0x5d: dup2_x1(frame); break;        // dup2_x1
                case 0x5e: dup2_x2(frame); break;        // dup2_x2
                case 0x5f:  __swap(frame); break;        // swap

                // Math
                case 0x60:  __iadd(frame); break; // iadd
                case 0x61:  __ladd(frame); break; // ladd
                case 0x62:  __fadd(frame); break; // fadd
                case 0x63:  __dadd(frame); break; // dadd

                case 0x64:  __isub(frame); break; // isub
                case 0x65:  __lsub(frame); break; // lsub
                case 0x66:  __fsub(frame); break; // fsub
                case 0x67:  __dsub(frame); break; // dsub

                case 0x68:  __imul(frame); break; // imul
                case 0x69:  __lmul(frame); break; // lmul
                case 0x6a:  __fmul(frame); break; // fmul
                case 0x6b:  __dmul(frame); break; // dmul

                case 0x6c:  __idiv(frame); break; // idiv
                case 0x6d:  __ldiv(frame); break; // ldiv
                case 0x6e:  __fdiv(frame); break; // fdiv
                case 0x6f:  __ddiv(frame); break; // ddiv

                case 0x70:  __irem(frame); break; // irem
                case 0x71:  __frem(frame); break; // frem
                case 0x72:  __lrem(frame); break; // lrem
                case 0x73:  __drem(frame); break; // drem

                case 0x74:  ineg(frame); break; // ineg
                case 0x75:  lneg(frame); break; // lneg
                case 0x76:  fneg(frame); break; // fneg
                case 0x77:  dneg(frame); break; // dneg

                case 0x78:  ishl(frame); break; // ishl
                case 0x79:  lshl(frame); break; // lshl
                case 0x7a:  ishr(frame); break; // ishr
                case 0x7b:  lshr(frame); break; // lshr

                case 0x7c:  iushr(frame); break;  // iushr
                case 0x7d:  lushr(frame); break;  // lushr
                case 0x7e:  __iand(frame); break; // iand
                case 0x7f:  __land(frame); break; // land

                case 0x80:  __ior(frame); break;  // ior
                case 0x81:  __lor(frame); break;  // lor
                case 0x82:  __ixor(frame); break; // ixor
                case 0x83:  __lxor(frame); break; // lxor

                case 0x84:  iinc(frame); break; // iinc

                // Conversions
                case 0x85:  __i2l(frame); break; // i2l
                case 0x86:  __i2f(frame); break; // i2f
                case 0x87:  __i2d(frame); break; // i2d

                case 0x88:  __l2i(frame); break; // l2i
                case 0x89:  __l2f(frame); break; // l2f
                case 0x8a:  __l2d(frame); break; // l2d

                case 0x8b:  __f2i(frame); break; // f2i
                case 0x8c:  __f2l(frame); break; // f2l
                case 0x8d:  __f2d(frame); break; // f2d

                case 0x8e:  __d2i(frame); break; // d2i
                case 0x8f:  __d2l(frame); break; // d2l
                case 0x90:  __d2f(frame); break; // d2f

                case 0x91:  __i2b(frame); break; // i2b
                case 0x92:  __i2c(frame); break; // i2c
                case 0x93:  __i2s(frame); break; // i2s

                // Comparisons
                case 0x94: lcmp(frame); break; // lcmp
                case 0x95: fcmpl(frame); break; // fcmpl
                case 0x96: fcmpg(frame); break; // fcmpg
                case 0x97: dcmpl(frame); break; // dcmpl
                case 0x98: dcmpg(frame); break; // dcmpg

                case 0x99: IF_COND(==); break; // ifeq
                case 0x9a: IF_COND(!=); break; // ifne
                case 0x9b: IF_COND(<);  break; // iflt
                case 0x9c: IF_COND(>=); break; // ifge
                case 0x9d: IF_COND(>);  break; // ifgt
                case 0x9e: IF_COND(<=); break; // ifle

                case 0x9f: IF_ICMP_COND(==); break; // if_icmpeq
                case 0xa0: IF_ICMP_COND(!=); break; // if_icmpne
                case 0xa1: IF_ICMP_COND(<);  break; // if_icmplt
                case 0xa2: IF_ICMP_COND(>=); break; // if_icmpge
                case 0xa3: IF_ICMP_COND(>);  break; // if_icmpgt
                case 0xa4: IF_ICMP_COND(<=); break; // if_icmple

                case 0xa5: IF_ACMP_COND(==); break; // if_acmpeq
                case 0xa6: IF_ACMP_COND(!=); break; // if_acmpne

                // Control
                case 0xa7: __goto(frame); break; // goto
                case 0xa8: jsr(frame); break;    // jsr
                case 0xa9: ret(frame); break;    // ret
                case 0xaa: tableswitch(frame); break;  // tableswitch
                case 0xab: lookupswitch(frame); break; // lookupswitch
                case 0xac: TRETURN(i); break; // ireturn
                case 0xad: TRETURN(l); break; // lreturn
                case 0xae: TRETURN(f); break; // freturn
                case 0xaf: TRETURN(d); break; // dreturn
                case 0xb0: TRETURN(r); break; // areturn
                case 0xb1: frame_exe_over(jthread_pop_frame(frame->thread)); break; // return

                // References
                case 0xb2: getstatic(frame); break; // getstatic
                case 0xb3: putstatic(frame); break; // putstatic
                case 0xb4: getfield(frame); break;  // getfield
                case 0xb5: putfield(frame); break;  // putfield

                case 0xb6: invokevirtual(frame); break;   // invokevirtual
                case 0xb7: invokespecial(frame); break;   // invokespecial
                case 0xb8: invokestatic(frame); break;    // invokestatic
                case 0xb9: invokeinterface(frame); break; // invokeinterface
                case 0xba: invokedynamic(frame); break;   // invokedynamic

                case 0xbb: __new(frame); break;       // new
                case 0xbc: newarray(frame); break;    // newarray
                case 0xbd: anewarray(frame); break;   // anewarray
                case 0xbe: arraylength(frame); break; // arraylength

                case 0xbf: athrow(frame); break;       // athrow
                case 0xc0: checkcast(frame); break;    // checkcast
                case 0xc1: instanceof(frame); break;   // instanceof
                case 0xc2: monitorenter(frame); break; // monitorenter
                case 0xc3: monitorexit(frame); break;  // monitorexit

                // Extended
                case 0xc4: wide_extending = true; break; // wide
                case 0xc5: multianewarray(frame); break; // multianewarray
                case 0xc6: ifnull(frame); break; // ifnull
                case 0xc7: ifnonnull(frame); break; // ifnonnull
                case 0xc8: vm_internal_error("goto_w doesn't support"); break; // goto_w  // todo
                case 0xc9: vm_internal_error("jsr_w doesn't support after jdk 6."); // jsr_w // todo

                // Reserved
                case 0xca: jvm_abort("debugger used instructions. %s\n", frame_to_string(frame)); break; // breakpoint, // debugger used instruction// todo
                case 0xfe: frame->m.method->native_method(frame); break; // jvm used instruction，本 jvm 用来调用本地方法。
                case 0xff: jvm_abort("jvm used instruction, not used in this jvm. %s", frame_to_string(frame)); // todo
                default:
                    jvm_abort("This instruction isn't used. %s\n", frame_to_string(frame));
                    break;
            }

            if (frame_is_exe_over(frame)) {
                printvm_debug("frame(%p) exe over, destroy.\n", frame);
                thread_recycle_frame(frame);
                break;
            }

            if (frame_interrupted(frame)) {
                printvm_debug("frame(%p) interrupted.\n", frame);
                break; // 当前函数执行被中断。跳出循环，终止当前 frame 的执行。
            }
        }
    }

    printvm_debug("interpret exit.\n");
    return NULL; // todo
}
