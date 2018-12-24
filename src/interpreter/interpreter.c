/*
 * Author: Jia Yang
 */

#include "interpreter.h"
#include "../rtda/thread/jthread.h"
#include "../jvm.h"
#include "../rtda/thread/frame.h"

#if (JVM_DEBUG)
// the mapping of instructions's code and name
static char* code_names[] = {
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

extern void (* instructions[])(struct frame *);

void* interpret(void *thread0)
{
    assert(thread0 != NULL);
    struct jthread *thread = thread0;

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
        while (bcr_has_more(&frame->reader)) {
            u1 opcode = bcr_readu1(&frame->reader);
            printvm_debug("%d(0x%x), %s, pc = %lu\n", opcode, opcode, code_names[opcode], frame->reader.pc);
            instructions[opcode](frame);

            if (frame_is_exe_over(frame)) {
                jthread_recycle_frame(frame);
                printvm_debug("frame(%p) exe over, destroy.\n", frame);
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
