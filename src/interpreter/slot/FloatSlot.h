/*
 * Author: Jia Yang
 */

#ifndef JVM_FLOATSLOT_H
#define JVM_FLOATSLOT_H


#include "Slot.h"

class FloatSlot: public Slot {
    jfloat v;
public:
    explicit FloatSlot(jfloat v0): v(v0) {
        type = PRIMITIVE_FLOAT;
    }

    jfloat getValue() {
        return v;
    }

    void negValue() override {
        v = -v;
    }

    void convertToInt() {
        // todo
    }

    void convertToLong() {
// todo
    }

    void convertToDouble() {
// todo
    }

    std::string toString() const override {
        return "float: " + std::to_string(v);
    }
};


#endif //JVM_FLOATSLOT_H
