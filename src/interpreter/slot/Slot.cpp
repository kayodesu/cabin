#include "../../jvm.h"
#include "Slot.h"
#include "IntSlot.h"
#include "FloatSlot.h"
#include "BoolSlot.h"
#include "DoubleSlot.h"
#include "LongSlot.h"
#include "ReferenceSlot.h"

/*
 * Author: Jia Yang
 */

jint Slot::getInt() {
    if (type != PRIMITIVE_INT) {
        // todo
        jvmAbort("error. %d\n", type);
    }
    return ((IntSlot *)this)->getValue();
}

jfloat Slot::getFloat() {
    if (type != PRIMITIVE_FLOAT) {
        // todo
        jvmAbort("error. %d\n", type);
    }
    return ((FloatSlot *)this)->getValue();
}

jlong Slot::getLong() {
    if (type != PRIMITIVE_LONG) {
        // todo
        jvmAbort("error. %s\n", jtypeNames[type]);
    }
    return ((LongSlot *)this)->getValue();
}

jdouble Slot::getDouble() {
    if (type != PRIMITIVE_DOUBLE) {
        // todo
        jvmAbort("error. %d\n", type);
    }
    return ((DoubleSlot *)this)->getValue();
}

jreference Slot::getReference() {
    if (type != REFERENCE) {
        // todo
        jvmAbort("error. want reference, but get %s\n", jtypeNames[type]);
    }
    return ((ReferenceSlot *)this)->getValue();
}
