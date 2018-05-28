/*
 * Author: Jia Yang
 */

#ifndef JVM_INVALIDSLOT_H
#define JVM_INVALIDSLOT_H


#include "Slot.h"

class InvalidSlot: public Slot {
public:
    explicit InvalidSlot() {
        type = INVALID_JVM_TYPE;
    }

//    jint value() {  // todo
//        return v;  jvmAbort("error.\n");  // todo
//    }

    void negValue() override {
        jvmAbort("error.\n");  // todo
    }

    std::string toString() const override {
        return "InvalidSlot";
    }
};


#endif //JVM_INVALIDSLOT_H
