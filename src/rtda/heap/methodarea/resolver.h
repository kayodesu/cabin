/*
 * Author: Jia Yang
 */

#ifndef JVM_RESOLVER_H
#define JVM_RESOLVER_H

#include "Jclass.h"
#include "Jmethod.h"
#include "Jfield.h"

static Jclass* resolveClass(const Jclass *visitor, const std::string& className) {
    if (className.empty()) {
        jvmAbort("error. class_name is empty\n");
    }

    Jclass *jclass = visitor->loader->loadClass(className);

    if (jclass->isAccessibleTo(visitor)) {
        return jclass;
    }

    // todo  java.lang.IllegalAccessError
    jvmAbort("java.lang.IllegalAccessError\n");
}


static Jmethod* resolveMethod(const Jclass *visitor, MethodRef &methodRef, bool isStatic) {
    if (methodRef.method == NULL) {
        Jclass *jclass = resolveClass(visitor, methodRef.className);
        if (!jclass->isInited) {   // todo
            jvmAbort("error\n");
        }
        methodRef.method = jclass->lookupMethod(methodRef.nameAndType.name, methodRef.nameAndType.descriptor);
        if (methodRef.method == nullptr) {
            jvmAbort("error\n"); // todo
        }
    }

    if (isStatic != methodRef.method->isStatic()) {
        // todo java.lang.IncompatibleClassChangeError
        jvmAbort("java.lang.IncompatibleClassChangeError\n");
    }

    if (!methodRef.method->isAccessibleTo(visitor)) {
        // java.lang.IllegalAccessError   todo
        jvmAbort("java.lang.IllegalAccessError\n");
    }

    return methodRef.method;
}

static Jfield* resolveField(const Jclass *visitor, FieldRef &fieldRef, bool isStatic) {
    if (fieldRef.field == nullptr) {
        Jclass *jclass = resolveClass(visitor, fieldRef.className);
        if (!jclass->isInited) {   // todo
            jvmAbort("error\n");
        }
        fieldRef.field = jclass->lookupField(fieldRef.nameAndType.name, fieldRef.nameAndType.descriptor);
    }

    if (isStatic != fieldRef.field->isStatic()) {
        // todo java.lang.IncompatibleClassChangeError
        jvmAbort("java.lang.IncompatibleClassChangeError\n");
    }

    if (!fieldRef.field->isAccessibleTo(visitor)) {
        // java.lang.IllegalAccessError   todo
        jvmAbort("java.lang.IllegalAccessError\n");
    }

    return fieldRef.field;
}

#endif //JVM_RESOLVER_H
