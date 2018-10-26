/*
 * Author: Jia Yang
 */

#include <stdbool.h>
#include "instruction.h"
#include "../interpreter/stack_frame.h"
#include "constants.h"
#include "comparisons.h"
#include "loads.h"
#include "stores.h"
#include "stacks.h"
#include "maths.h"
#include "conversions.h"
#include "controls.h"
#include "reserved.h"
#include "extended.h"
#include "references.h"

/*
 * 标识当前指令是否有 wide 扩展。
 * 只对
 * iload, fload, aload, lload, dload,
 * istore, fstore, astore, lstore, dstore,
 * ret, iinc
 * 有效。
 * 以上指令执行前需检查此标志，执行后需复位（置为false）此标志。
 */
bool wide_extending = false;

jint fetch_index(struct stack_frame *frame)
{
    if (wide_extending) {
        wide_extending = false;
        return bcr_readu2(frame->reader);
    }
    return bcr_readu1(frame->reader);
}

static void nop(struct stack_frame *frame) { }

struct instruction instructions[] = {
        { 0x00, "nop", nop },

        // Constants
        { 0x01, "aconst_null", aconst_null },

        { 0x02, "iconst_m1", iconst_m1 },
        { 0x03, "iconst_0", iconst_0 },
        { 0x04, "iconst_1", iconst_1 },
        { 0x05, "iconst_2", iconst_2 },
        { 0x06, "iconst_3", iconst_3 },
        { 0x07, "iconst_4", iconst_4 },
        { 0x08, "iconst_5", iconst_5 },

        { 0x09, "lconst_0", lconst_0 },
        { 0x0a, "lconst_1", lconst_1 },

        { 0x0b, "fconst_0", fconst_0 },
        { 0x0c, "fconst_1", fconst_1 },
        { 0x0d, "fconst_2", fconst_2 },

        { 0x0e, "dconst_0", dconst_0 },
        { 0x0f, "dconst_1", dconst_1 },

        { 0x10, "bipush", bipush },  // Byte Integer push
        { 0x11, "sipush", sipush },  // Short Integer push

        { 0x12, "ldc", ldc},  // LoaD Constant
        { 0x13, "ldc_w", ldc_w },
        { 0x14, "ldc2_w", ldc2_w },

        // Loads
        { 0x15, "iload", iload },
        { 0x16, "lload", lload },
        { 0x17, "fload", fload },
        { 0x18, "dload", dload },
        { 0x19, "aload", aload },

        { 0x1a, "iload_0", iload_0 },
        { 0x1b, "iload_1", iload_1 },
        { 0x1c, "iload_2", iload_2 },
        { 0x1d, "iload_3", iload_3 },

        { 0x1e, "lload_0", lload_0 },
        { 0x1f, "lload_1", lload_1 },
        { 0x20, "lload_2", lload_2 },
        { 0x21, "lload_3", lload_3 },

        { 0x22, "fload_0", fload_0 },
        { 0x23, "fload_1", fload_1 },
        { 0x24, "fload_2", fload_2 },
        { 0x25, "fload_3", fload_3 },

        { 0x26, "dload_0", dload_0 },
        { 0x27, "dload_1", dload_1 },
        { 0x28, "dload_2", dload_2 },
        { 0x29, "dload_3", dload_3 },

        { 0x2a, "aload_0", aload_0 },
        { 0x2b, "aload_1", aload_1 },
        { 0x2c, "aload_2", aload_2 },
        { 0x2d, "aload_3", aload_3 },

        { 0x2e, "iaload", iaload },
        { 0x2f, "laload", laload },
        { 0x30, "faload", faload },
        { 0x31, "daload", daload },
        { 0x32, "aaload", aaload }, // todo 包括一维和多维吗
        { 0x33, "baload", baload },
        { 0x34, "caload", caload },
        { 0x35, "saload", saload },

        // Stores
        { 0x36, "istore", istore },
        { 0x37, "lstore", lstore },
        { 0x38, "fstore", fstore },
        { 0x39, "dstore", dstore },
        { 0x3a, "astore", astore },

        { 0x3b, "istore_0", istore_0 },
        { 0x3c, "istore_1", istore_1 },
        { 0x3d, "istore_2", istore_2 },
        { 0x3e, "istore_3", istore_3 },

        { 0x3f, "lstore_0", lstore_0 },
        { 0x40, "lstore_1", lstore_1 },
        { 0x41, "lstore_2", lstore_2 },
        { 0x42, "lstore_3", lstore_3 },

        { 0x43, "fstore_0", fstore_0 },
        { 0x44, "fstore_1", fstore_1 },
        { 0x45, "fstore_2", fstore_2 },
        { 0x46, "fstore_3", fstore_3 },

        { 0x47, "dstore_0", dstore_0 },
        { 0x48, "dstore_1", dstore_1 },
        { 0x49, "dstore_2", dstore_2 },
        { 0x4a, "dstore_3", dstore_3 },

        { 0x4b, "astore_0", astore_0 },
        { 0x4c, "astore_1", astore_1 },
        { 0x4d, "astore_2", astore_2 },
        { 0x4e, "astore_3", astore_3 },

        { 0x4f, "iastore", iastore },
        { 0x50, "lastore", lastore },
        { 0x51, "fastore", fastore },
        { 0x52, "dastore", dastore },
        { 0x53, "aastore", aastore }, // todo 包括一维和多维吗
        { 0x54, "bastore", bastore },
        { 0x55, "castore", castore },
        { 0x56, "sastore", sastore },

        // Stack
        { 0x57, "pop",     pop },
        { 0x58, "pop2",    pop2 },
        { 0x59, "dup",     dup },
        { 0x5a, "dup_x1",  dup_x1 },
        { 0x5b, "dup_x2",  dup_x2 },
        { 0x5c, "dup2",    dup2 },
        { 0x5d, "dup2_x1", dup2_x1 },
        { 0x5e, "dup2_x2", dup2_x2 },
        { 0x5f, "swap",    __swap },

        // Math
        { 0x60, "iadd", __iadd },
        { 0x61, "ladd", __ladd },
        { 0x62, "fadd", __fadd },
        { 0x63, "dadd", __dadd },

        { 0x64, "isub", __isub },
        { 0x65, "lsub", __lsub },
        { 0x66, "fsub", __fsub },
        { 0x67, "dsub", __dsub },

        { 0x68, "imul", __imul },
        { 0x69, "lmul", __lmul },
        { 0x6a, "fmul", __fmul },
        { 0x6b, "dmul", __dmul },

        { 0x6c, "idiv", __idiv },
        { 0x6d, "ldiv", __ldiv },
        { 0x6e, "fdiv", __fdiv },
        { 0x6f, "ddiv", __ddiv },

        { 0x70, "irem", __irem },
        { 0x71, "lrem", __lrem },
        { 0x72, "frem", __frem },  //  todo fmodf  or dremf
        { 0x73, "drem", __drem },   //  todo  fmod or drem

        { 0x74, "ineg", ineg },
        { 0x75, "lneg", lneg },
        { 0x76, "fneg", fneg },
        { 0x77, "dneg", dneg },

        { 0x78, "ishl", ishl },
        { 0x79, "lshl", lshl },

        // 逻辑右移 shift logical right
        { 0x7a, "ishr", ishr },
        { 0x7b, "lshr", lshr },

        // 算术右移 shift arithmetic right
        { 0x7c, "iushr", iushr },
        { 0x7d, "lushr", lushr },

        { 0x7e, "iand", __iand },
        { 0x7f, "land", __land },

        { 0x80, "ior", __ior },
        { 0x81, "lor", __lor },

        { 0x82, "ixor", __ixor },
        { 0x83, "lxor", __lxor },

        { 0x84, "iinc", iinc },

        // Conversions
        { 0x85, "i2l", __i2l },
        { 0x86, "i2f", __i2f },
        { 0x87, "i2d", __i2d },

        { 0x88, "l2i", __l2i },
        { 0x89, "l2f", __l2f },
        { 0x8a, "l2d", __l2d },

        { 0x8b, "f2i", __f2i },
        { 0x8c, "f2l", __f2l },
        { 0x8d, "f2d", __f2d },

        { 0x8e, "d2i", __d2i },
        { 0x8f, "d2l", __d2l },
        { 0x90, "d2f", __d2f },

        { 0x91, "i2b", __i2b },
        { 0x92, "i2c", __i2c },
        { 0x93, "i2s", __i2s },

        // Comparisons
        { 0x94, "lcmp", lcmp },
        { 0x95, "fcmpl", fcmpl },
        { 0x96, "fcmpg", fcmpg },
        { 0x97, "dcmpl", dcmpl },
        { 0x98, "dcmpg", dcmpg },

        { 0x99, "ifeq", ifeq },
        { 0x9a, "ifne", ifne },
        { 0x9b, "iflt", iflt },
        { 0x9c, "ifge", ifge },
        { 0x9d, "ifgt", ifgt },
        { 0x9e, "ifle", ifle },

        { 0x9f, "if_icmpeq", if_icmpeq },
        { 0xa0, "if_icmpne", if_icmpne },
        { 0xa1, "if_icmplt", if_icmplt },
        { 0xa2, "if_icmpge", if_icmpge },
        { 0xa3, "if_icmpgt", if_icmpgt },
        { 0xa4, "if_icmple", if_icmple },

        { 0xa5, "if_acmpeq", if_acmpeq },
        { 0xa6, "if_acmpne", if_acmpne },

        // Control
        { 0xa7, "goto", __goto },

        { 0xa8, "jsr", NULL },
        { 0xa9, "ret", NULL }, // todo ret 有 wide 扩展
        { 0xaa, "tableswitch", tableswitch },
        { 0xab, "lookupswitch", lookupswitch },

        { 0xac, "ireturn", ireturn },
        { 0xad, "lreturn", lreturn },
        { 0xae, "freturn", freturn },
        { 0xaf, "dreturn", dreturn },

        // todo areturn: objectref必须是一个reference类型的数据，
        // todo 并且必须指向一个类型与当前方法的方法描述符（§4.3.3）中的返回值相匹配（JLS §5.2）的对象。
        { 0xb0, "areturn", areturn },

        { 0xb1, "return", __return },

        // References
        { 0xb2, "getstatic", getstatic },
        { 0xb3, "putstatic", putstatic },
        { 0xb4, "getfield", getfield },
        { 0xb5, "putfield", putfield },

        { 0xb6, "invokevirtual", invokevirtual },
        { 0xb7, "invokespecial", invokespecial },
        { 0xb8, "invokestatic", invokestatic },
        { 0xb9, "invokeinterface", invokeinterface },
        { 0xba, "invokedynamic", invokedynamic },

        { 0xbb, "new", __new },

        { 0xbc, "newarray", newarray },
        { 0xbd, "anewarray", anewarray },
        { 0xbe, "arraylength", arraylength },

        { 0xbf, "athrow", athrow },
        { 0xc0, "checkcast", checkcast },
        { 0xc1, "instanceof", instanceof },
        { 0xc2, "monitorenter", monitorenter },
        { 0xc3, "monitorexit", monitorexit },

        // Extended
        { 0xc4, "wide", wide },
        { 0xc5, "multianewarray", multianewarray },
        { 0xc6, "ifnull", ifnull },
        { 0xc7, "ifnonnull", ifnonnull },
        { 0xc8, "goto_w", NULL },
        { 0xc9, "jsr_w", NULL },

        // Reserved
        { 0xca, "breakpoint", breakpoint }, // debugger used instruction

#define NOTUSED(opcode) { opcode, "notused", notused }

        NOTUSED(0xcb), NOTUSED(0xcc), NOTUSED(0xcd), NOTUSED(0xce), NOTUSED(0xcf), NOTUSED(0xd0), NOTUSED(0xd1),
        NOTUSED(0xd2), NOTUSED(0xd3), NOTUSED(0xd4), NOTUSED(0xd5), NOTUSED(0xd6), NOTUSED(0xd7), NOTUSED(0xd8),
        NOTUSED(0xd9), NOTUSED(0xda), NOTUSED(0xdb), NOTUSED(0xdc), NOTUSED(0xdd), NOTUSED(0xde), NOTUSED(0xdf),
        NOTUSED(0xe0), NOTUSED(0xe1), NOTUSED(0xe2), NOTUSED(0xe3), NOTUSED(0xe4), NOTUSED(0xe5), NOTUSED(0xe6),
        NOTUSED(0xe7), NOTUSED(0xe8), NOTUSED(0xe9), NOTUSED(0xea), NOTUSED(0xeb), NOTUSED(0xec), NOTUSED(0xed),
        NOTUSED(0xee), NOTUSED(0xef), NOTUSED(0xf0), NOTUSED(0xf1), NOTUSED(0xf2), NOTUSED(0xf3), NOTUSED(0xf4),
        NOTUSED(0xf5), NOTUSED(0xf6), NOTUSED(0xf7), NOTUSED(0xf8), NOTUSED(0xf9), NOTUSED(0xfa), NOTUSED(0xfb),
        NOTUSED(0xfc), NOTUSED(0xfd),

#undef NOTUSED

        { 0xfe, "impdep1", invoke_native_method }, // jvm used instruction，本 jvm 用来调用本地方法。
        { 0xff, "impdep2", NULL },  // jvm used instruction, not used in this jvm.
};
