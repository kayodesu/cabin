/*
 * Author: Jia Yang
 */

#ifndef JVM_LONGSLOT_H
#define JVM_LONGSLOT_H


#include "Slot.h"

class LongSlot: public Slot {
    jlong v;
public:
    explicit LongSlot(jlong v0): v(v0) {
        type = PRIMITIVE_LONG;
    }

    jlong getValue() {
        return v;
    }

    void negValue() override {
        v = -v;
    }

    void convertToInt() {
        // todo
    }

    void convertToFloat() {
// todo
    }

    void convertToDouble() {
// todo
    }

    std::string toString() const override {
        return "long: " + std::to_string(v);
    }
};

#endif //JVM_LONGSLOT_H
