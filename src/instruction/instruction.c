/*
 * Author: Jia Yang
 */

#include <stdbool.h>
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

// 指令集
void (* instructions[])(struct stack_frame *) = {
        nop,

        // Constants [0x01 ... 0x14]
        aconst_null,
        iconst_m1, iconst_0, iconst_1, iconst_2, iconst_3, iconst_4, iconst_5,
        lconst_0, lconst_1,
        fconst_0, fconst_1, fconst_2,
        dconst_0, dconst_1,
        bipush /* Byte Integer push */, sipush /* Short Integer push */,
        ldc, ldc_w, ldc2_w, // LoaD Constant

        // Loads [0x15 ... 0x35]
        iload, lload, fload, dload, aload,
        iload_0, iload_1, iload_2, iload_3,
        lload_0, lload_1, lload_2, lload_3,
        fload_0, fload_1, fload_2, fload_3,
        dload_0, dload_1, dload_2, dload_3,
        aload_0, aload_1, aload_2, aload_3,
        iaload, laload, faload, daload, aaload /* todo 包括一维和多维吗 */, baload, caload, saload,

        // Stores [0x36 ... 0x56]
        istore, lstore, fstore, dstore, astore,
        istore_0, istore_1, istore_2, istore_3,
        lstore_0, lstore_1, lstore_2, lstore_3,
        fstore_0, fstore_1, fstore_2, fstore_3,
        dstore_0, dstore_1, dstore_2, dstore_3,
        astore_0, astore_1, astore_2, astore_3,
        iastore, lastore, fastore, dastore, aastore, /* todo 包括一维和多维吗 */ bastore, castore, sastore,

        // Stack [0x57 ... 0x5f]
        pop, pop2, dup, dup_x1, dup_x2, dup2, dup2_x1, dup2_x2, __swap,

        // Math [0x60 ... 0x84]
        __iadd, __ladd, __fadd, __dadd,
        __isub, __lsub, __fsub, __dsub,
        __imul, __lmul, __fmul, __dmul,
        __idiv, __ldiv, __fdiv, __ddiv,
        __irem, __lrem, __frem,  //  todo fmodf  or dremf
        __drem,   //  todo  fmod or drem
        ineg, lneg, fneg, dneg,
        ishl, lshl,
        ishr, lshr,   // 逻辑右移 shift logical right
        iushr, lushr, // 算术右移 shift arithmetic right
        __iand, __land, __ior, __lor, __ixor, __lxor, iinc,

        // Conversions [0x85 ... 0x93]
        __i2l, __i2f, __i2d,
        __l2i, __l2f, __l2d,
        __f2i, __f2l, __f2d,
        __d2i, __d2l, __d2f,
        __i2b, __i2c, __i2s,

        // Comparisons [0x94 ... 0xa6]
        lcmp, fcmpl, fcmpg, dcmpl, dcmpg,
        ifeq, ifne, iflt, ifge, ifgt, ifle,
        if_icmpeq, if_icmpne, if_icmplt, if_icmpge, if_icmpgt, if_icmple,
        if_acmpeq, if_acmpne,

        // Control [0xa7 ... 0xb1]
        __goto, jsr, ret, tableswitch, lookupswitch,
        ireturn, lreturn, freturn, dreturn, areturn, __return,

        // References [0xb2 ... 0xc3]
        getstatic, putstatic, getfield, putfield,
        invokevirtual, invokespecial, invokestatic, invokeinterface, invokedynamic,
        __new, newarray, anewarray, arraylength,
        athrow, checkcast, instanceof, monitorenter, monitorexit,

        // Extended [0xc4 ... 0xc9]
        wide, multianewarray, ifnull, ifnonnull, goto_w, jsr_w,

        // Reserved [0xca ... 0xff]
        breakpoint, // debugger used instruction
        [0xcb ... 0xfd] = notused,
        invoke_native_method, // jvm used instruction，本 jvm 用来调用本地方法。
        impdep2,  // jvm used instruction, not used in this jvm.
};
