/*
 * Author: Jia Yang
 */

#include "interpreter.h"
#include "../rtda/thread/jthread.h"
#include "../jvm.h"
#include "stack_frame.h"

#ifdef JVM_DEBUG
// the mapping of instructions's code and name
static char* ins_code_name_mapping[] = {
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

extern void (* instructions[])(struct stack_frame *);

void interpret(struct jthread *thread)
{
    while (!jthread_is_stack_empty(thread)) {
        struct stack_frame *frame = jthread_top_frame(thread);
        if (frame->type == SF_TYPE_SHIM) {
            if (frame->shim_action != NULL) {
                frame->shim_action(frame);
            }

            jthread_pop_frame(thread);
            sf_destroy(frame);
#ifdef JVM_DEBUG
            printvm("shim frame(%p) exe over, destroy.\n", frame);
#endif
            continue;
        }

        struct bytecode_reader *reader = frame->reader;
#ifdef JVM_DEBUG
        printvm("executing frame(%p): %s, pc = %lu\n", frame, sf_to_string(frame), bcr_get_pc(reader));
#endif
        while (bcr_has_more(reader)) {
//            jthread_set_pc(frame->thread, bcr_get_pc(reader)); // store pc
            u1 opcode = bcr_readu1(reader);
#ifdef JVM_DEBUG
            printvm("%d(0x%x), %s, pc = %lu\n", opcode, opcode, ins_code_name_mapping[opcode], bcr_get_pc(reader));
#endif
            instructions[opcode](frame);

            if (sf_is_exe_over(frame)) {
                sf_destroy(frame);
#ifdef JVM_DEBUG
                printvm("frame(%p) exe over, destroy.\n", frame);
#endif
                break;
            }

            if (sf_interrupted(frame)) {
#ifdef JVM_DEBUG
                printvm("frame(%p) interrupted.\n", frame);
#endif
                break; // 当前函数执行被中断。跳出循环，终止当前 frame 的执行。
            }

            /*
             * 如果当前栈顶帧正在处理异常，
             * 则需重置frame和reader，因为在异常处理中可能有一些帧被弹出
             */
//            if (sf_is_proc_exception(jthread_top_frame(thread))) {
//                frame = jthread_top_frame(thread);
//                reader = frame->reader;
//            }
        }
    }
#ifdef JVM_DEBUG
    printvm("interpret exit.\n");
#endif
}
