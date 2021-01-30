#ifndef CABIN_PRIM_H
#define CABIN_PRIM_H

#include "../cabin.h"
#include "../slot.h"

bool isPrimClassName(const utf8_t *class_name);
bool isPrimDescriptor(utf8_t descriptor);
bool isPrimWrapperClassName(const utf8_t *class_name);
const utf8_t *getPrimArrayClassName(const utf8_t *class_name);
const utf8_t *getPrimClassName(utf8_t descriptor);
const utf8_t *getPrimDescriptorByClassName(const utf8_t *class_name);
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

#endif // CABIN_PRIM_H