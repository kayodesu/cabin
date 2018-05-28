/*
 * Author: Jia Yang
 */

#ifndef JVM_SLOT_H
#define JVM_SLOT_H

#include "../../jvm.h"
#include "../../Base.h"


/*
 * todo   增加注释： slot 应用在哪里
 */
//template<typename T>
class Slot: public Base {

//    Jvalue value;


//
//    Slot(jtype type, jfloat f) {
//        this->type = type;
//        this->value.f = f;
//    }
public:
    Jtype type;

    Slot(): type(INVALID_JVM_TYPE) {}

    jint getInt();

    jfloat getFloat();

    jlong getLong();

    jdouble getDouble();

    jreference getReference();

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

    virtual void negValue() = 0;
//    void negValue() {  // todo ugly code
//        if (type == PRIMITIVE_INT) {
//            value.i = -value.i;
//        } else if (type == PRIMITIVE_FLOAT) {
//            value.f = -value.f;
//        } else if (type == PRIMITIVE_LONG) {
//            value.l = -value.l;
//        } else if (type == PRIMITIVE_DOUBLE) {
//            value.d = -value.d;
//        } else {
//            jprintf("error.\n");
//        }
//    }

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

//    virtual auto value() = 0;

//    static Slot intSlot(jint v) {
//        Slot s = Slot(PRIMITIVE_INT);
//        s.value.i = v;
//        return s;
//    }
//
//    static Slot floatSlot(jfloat v) {
//        Slot s = Slot(PRIMITIVE_FLOAT);
//        s.value.f = v;
//        return s;
//    }
//
//    static Slot longSlot(jlong v) {
//        Slot s = Slot(PRIMITIVE_LONG);
//        s.value.l = v;
//        return s;
//    }
//
//    static Slot doubleSlot(jdouble v) {
//        Slot s = Slot(PRIMITIVE_DOUBLE);
//        s.value.d = v;
//        return s;
//    }
//
//    static Slot referenceSlot(jreference v) {
//        Slot s = Slot(REFERENCE);
//        s.value.r = v;
//        return s;
//    }
//
//    static Slot invalidSlot() {
//        return Slot(INVALID_JVM_TYPE);
//    }
};

#endif //JVM_SLOT_H
