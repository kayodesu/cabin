/*
 * Author: Jia Yang
 */

#ifndef JVM_JMETHOD_H
#define JVM_JMETHOD_H

#include "Jmember.h"

class Jmethod: public Jmember {
    u2 maxStack;
    u2 maxLocals;
    u2 argSlotCount;

    u2 exceptionTableCount;
    struct ExceptionTable {
        u2 startPc;
        u2 endPc;
        u2 handlerPc;
        Jclass *catchType;
    };
    ExceptionTable *exceptionTables;

    u4 lineNumberTableCount;
    // key: u2 startPc;
    // value: u2 lineNumber;
    std::pair<u2, u2> *lineNumberTables;

public:
    int findExceptionHandler(Jclass *exceptionType, size_t pc) const;

//        jclass exceptions[];  // 此方法要抛出的异常类型   todo
    /*   private CodeAttribute.ExceptionHandle[] exceptionTable; */

    s1 *code;
    size_t codeLength;

    Jmethod() = default;

    Jmethod(Jclass *jclass, const MemberInfo &methodInfo);

    void calArgSlotCount();

    u2 getArgSlotCount() {
        return argSlotCount;
    }

    u2 getMaxStack() const {
        return maxStack;
    }

    u2 getMaxLocals() const {
        return maxLocals;
    }
//
//    void reset() {
//        reader->reset();
//    }

    std::string toString() const {
        return jclass->className + "::"
               + (isStatic() ? "(static)": "") + (isNative() ? "(native)": "") + name + "~" + descriptor;
    }
};


#endif //JVM_JMETHOD_H
