/*
 * Author: Jia Yang
 */

#include "JarrayObj.h"

void JarrayObj::calEleSize() {
    eleSize = sizeof(jreference);  // todo java中的引用......
    switch (jclass->className[1]) {
        case 'Z':
            eleSize = sizeof(jbool);
            break;
        case 'B':
            eleSize = sizeof(jbyte);
            break;
        case 'C':
            eleSize = sizeof(jchar);
            break;
        case 'S':
            eleSize = sizeof(jshort);
            break;
        case 'I':
            eleSize = sizeof(jint);
            break;
        case 'F':
            eleSize = sizeof(jfloat);
            break;
        case 'J':
            eleSize = sizeof(jlong);
            break;
        case 'D':
            eleSize = sizeof(jdouble);
            break;
        default:
            break;
    }
}

JarrayObj::JarrayObj(Jclass *jclass, jint arrLen): Jobject(jclass) {
    if (!jclass->isArray()) {
        jvmAbort("error. is not array. %s\n", jclass->className.c_str()); // todo
    }

//    jarray *arr = heap_malloc(sizeof(jarray) + arr_len * ele_size);
    this->jclass = jclass;
    calEleSize();
    data = new u1[arrLen * eleSize];

    for (int i = 0; i < arrLen * eleSize; i++) {
        ((u1 *)data)[i] = 0;
    }

    len = arrLen;
}

JarrayObj::JarrayObj(Jclass *jclass, jint arrDim, const jint *arrLens): Jobject(jclass) {
    int len = arrLens[0];
    new (this)JarrayObj(jclass, len); // todo
    if (arrDim == 1) {
        return;
    }

    Jclass *c = jclass->loader->loadClass(jclass->className.substr(1));
    for (jint i = 0; i < len; i++) {
        this->set(i, new JarrayObj(c, arrDim - 1, arrLens + 1));  // todo
    }
}
