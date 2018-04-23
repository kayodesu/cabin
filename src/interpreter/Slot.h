/*
 * Author: Jia Yang
 */

#ifndef JVM_SLOT_H
#define JVM_SLOT_H

#include "../jvm.h"


//template<typename T>
class Slot {
public:
    Jtype type;
    Jvalue value;

    Slot(Jtype type) {
        this->type = type;
    }
//
//    Slot(jtype type, jfloat f) {
//        this->type = type;
//        this->value.f = f;
//    }
public:
    Slot() {}

    jint getInt() {
        if (type != PRIMITIVE_INT) {
            // todo
            jvmAbort("error. %d\n", type);
        }
        return value.i;
    }

    jfloat getFloat() {
        if (type != PRIMITIVE_FLOAT) {
            // todo
            jvmAbort("error. %d\n", type);
        }
        return value.f;
    }

    jlong getLong() {
        if (type != PRIMITIVE_LONG) {
            // todo
            jvmAbort("error. %s\n", jtypeNames[type]);
        }
        return value.l;
    }

    jdouble getDouble() {
        if (type != PRIMITIVE_DOUBLE) {
            // todo
            jvmAbort("error. %d\n", type);
        }
        return value.d;
    }

    jreference getReference() {
        if (type != REFERENCE) {
            // todo
            jvmAbort("error. want reference, but get %s\n", jtypeNames[type]);
        }
        return value.r;
    }

    bool isClassificationTwo() const {
        return type == PRIMITIVE_LONG || type == PRIMITIVE_DOUBLE;
    }

    bool isClassificationOne() const {
        return !isClassificationTwo();
    }

//    bool isInvalid() const {
//        return type == INVALID_JVM_TYPE;
//    }

//    bool are_classification_one(const slot* s[], int count)
//    {
//        for (int i = 0; i < count; i++) {
//            if (!is_classification_one(s[i])) {
//                return false;
//            }
//        }
//        return true;
//    }

    void negValue() {  // todo ugly code
        if (type == PRIMITIVE_INT) {
            value.i = -value.i;
        } else if (type == PRIMITIVE_FLOAT) {
            value.f = -value.f;
        } else if (type == PRIMITIVE_LONG) {
            value.l = -value.l;
        } else if (type == PRIMITIVE_DOUBLE) {
            value.d = -value.d;
        } else {
            jprintf("error.\n");
        }
    }

    void x2x(Jtype src, Jtype want) {
        if (type != src) {
            // todo
            jvmAbort("error. src = %d, curr = %d\n", src, type);
            return;
        }

        type = want;
    }

//    static Slot boolSlot(jbool v) {
//        Slot s = Slot(PRIMITIVE_BOOLEAN);
//        s.value.z = v;
//        return s;
//    }

    std::string toString() {
        const char *name = jtypeNames[type];
        // todo value
        return name;
    }

    static Slot intSlot(jint v) {
        Slot s = Slot(PRIMITIVE_INT);
        s.value.i = v;
        return s;
    }

    static Slot floatSlot(jfloat v) {
        Slot s = Slot(PRIMITIVE_FLOAT);
        s.value.f = v;
        return s;
    }

    static Slot longSlot(jlong v) {
        Slot s = Slot(PRIMITIVE_LONG);
        s.value.l = v;
        return s;
    }

    static Slot doubleSlot(jdouble v) {
        Slot s = Slot(PRIMITIVE_DOUBLE);
        s.value.d = v;
        return s;
    }

    static Slot referenceSlot(jreference v) {
        Slot s = Slot(REFERENCE);
        s.value.r = v;
        return s;
    }

    static Slot invalidSlot() {
        return Slot(INVALID_JVM_TYPE);
    }
};

#endif //JVM_SLOT_H
