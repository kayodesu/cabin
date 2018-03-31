/*
 * Author: Jia Yang
 */

#ifndef JVM_CONSTANTS_H
#define JVM_CONSTANTS_H

#include "../interpreter/StackFrame.h"
#include "../rtda/heap/objectarea/Jobject.h"
#include "../rtda/heap/objectarea/strpool.h"

template <int indexBytes = 1, bool width = false>
static void __ldc(StackFrame *frame) {
    int index;
    if (indexBytes == 1) {
        index = frame->reader->readu1();
    } else {
        index = frame->reader->readu2();
    }

    RTCP *rtcp = frame->method->jclass->rtcp;
    const auto &c = rtcp->get(index);

    OperandStack &os = frame->operandStack;
    if (!width) {
        if (c.first == INTEGER_CONSTANT) {
            os.push(rtcp->getInt(index));
        } else if (c.first == FLOAT_CONSTANT) {
            os.push(rtcp->getFloat(index));
        } else if (c.first == STRING_CONSTANT) {
            JstringObj *o = getStrFromPool(frame->method->jclass->loader, strToJstr(rtcp->getStr(index)));
            os.push(o);
        } else if (c.first == CLASS_CONSTANT) {
            auto className = rtcp->getClassName(index);
   //         jprintf("ldc the object of class %s\n", className.c_str()); // todo
            Jclass *clazz = frame->method->jclass->loader->loadClass(className);
            os.push(clazz->classObj);
        } else {
            jvmAbort("error. %d\n", c.first);
        }
    } else {
        if (c.first == LONG_CONSTANT) {
            os.push(rtcp->getLong(index));
        } else if (c.first == DOUBLE_CONSTANT) {
            os.push(rtcp->getDouble(index));
        } else {
            jvmAbort("error. %d\n", c.first);
        }
    }
}

#endif
