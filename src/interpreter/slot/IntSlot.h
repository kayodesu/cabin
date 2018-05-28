/*
 * Author: Jia Yang
 */

#ifndef JVM_INTSLOT_H
#define JVM_INTSLOT_H

#include "../../jvm.h"
#include "Slot.h"

class IntSlot: public Slot {
    jint v;
public:
    explicit IntSlot(jint v0): v(v0) {
        type = PRIMITIVE_INT;
    }

    void setValue(jint v0) {
        v = v0;
    }

    jint getValue() {
        return v;
    }

    void negValue() override {
        v = -v;
    }

    void convertToChar() {
        // todo
    }

    void convertToShort() {
// todo
    }

    void convertToLong() {
// todo
    }

    void convertToFloat() {
// todo
    }

    void convertToDouble() {
// todo
    }

    std::string toString() const override {
        return "int: " + std::to_string(v);
    }
};

#endif //JVM_INTSLOT_H
