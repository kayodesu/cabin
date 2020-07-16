/*
 * Author: Yo Ka
 */

#ifndef JVM_PRIM_H
#define JVM_PRIM_H

#include "../jvmstd.h"
#include "../slot.h"

bool isPrimClassName(const utf8_t *class_name);
bool isPrimDescriptor(utf8_t descriptor);
bool isPrimWrapperClassName(const utf8_t *class_name);
const utf8_t *getPrimArrayClassName(const utf8_t *class_name);
const utf8_t *getPrimClassName(utf8_t descriptor);

const slot_t *primObjUnbox(const Object *box);

jref voidBox();
jref byteBox(jbyte x);
jref boolBox(jbool x);
jref charBox(jchar x);
jref shortBox(jshort x);
jref intBox(jint x);
jref floatBox(jfloat x);
jref longBox(jlong x);
jref doubleBox(jdouble x);

#endif // JVM_PRIM_H