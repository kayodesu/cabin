/*
 * Author: Jia Yang
 */

#ifndef JVM_BOOLSLOT_H
#define JVM_BOOLSLOT_H

#include "Slot.h"

class BoolSlot: public Slot {
    jbool v;
public:
    explicit BoolSlot(jbool v0): v(v0) {
        type = PRIMITIVE_BOOLEAN;
    }

    jbool getValue() {
        return v;
    }

    void negValue() override {
        jvmAbort("error.\n");  // todo
    }

    std::string toString() const override {
        return "bool: " + std::to_string(v);
    }
};

#endif //JVM_BOOLSLOT_H
