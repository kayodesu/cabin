/*
 * Author: kayo
 */

#ifndef JVM_PRIM_H
#define JVM_PRIM_H

#include "../vmdef.h"
#include "../slot.h"

bool isPrimClassName(const utf8_t *class_name);
bool isPrimDescriptor(utf8_t descriptor);
const utf8_t *getPrimArrayClassName(const utf8_t *class_name);
const utf8_t *getPrimClassName(utf8_t descriptor);

const slot_t *primObjUnbox(jprimref po);

#endif // JVM_PRIM_H