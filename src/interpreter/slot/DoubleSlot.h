/*
 * Author: Jia Yang
 */

#ifndef JVM_DOUBLESLOT_H
#define JVM_DOUBLESLOT_H


#include "Slot.h"

class DoubleSlot: public Slot {
    jdouble v;
public:
    explicit DoubleSlot(jdouble v0): v(v0) {
        type = PRIMITIVE_DOUBLE;
    }

    jdouble getValue() {
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

    void convertToFloat() {
// todo
    }

    std::string toString() const override {
        return "double: " + std::to_string(v);
    }
};


#endif //JVM_DOUBLESLOT_H
