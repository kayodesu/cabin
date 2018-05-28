/*
 * Author: Jia Yang
 */

#ifndef JVM_REFERENCESLOT_H
#define JVM_REFERENCESLOT_H


#include "Slot.h"

class ReferenceSlot: public Slot {
    jreference v;
public:
    explicit ReferenceSlot(jreference v0): v(v0) {
        type = REFERENCE;
    }

    jreference getValue() {
        return v;
    }

    void negValue() override {
        jvmAbort("error.\n");  // todo
    }

    std::string toString() const override {
        return "bool: " + std::to_string((jlong)v); // todo
    }
};



#endif //JVM_REFERENCESLOT_H
