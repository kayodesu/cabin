#ifndef CABIN_PRIMS_H
#define CABIN_PRIMS_H

#include "cabin.h"
#include "slot.h"

void init_prims();

bool is_prim_class_name(const utf8_t *class_name);
bool is_prim_descriptor(utf8_t descriptor);
bool is_prim_wrapper_class_name(const utf8_t *class_name);
const utf8_t *get_prim_array_class_name(const utf8_t *class_name);
const utf8_t *get_prim_class_name(utf8_t descriptor);
const utf8_t *get_prim_descriptor_by_wrapper_class_name(const utf8_t *wrapper_class_name);
const utf8_t *get_prim_descriptor_by_class_name(const utf8_t *class_name);
// const slot_t *primObjUnbox(const Object *box);

jref void_box();
jref byte_box(jbyte x);
jref bool_box(jbool x);
jref char_box(jchar x);
jref short_box(jshort x);
jref int_box(jint x);
jref float_box(jfloat x);
jref long_box(jlong x);
jref double_box(jdouble x);

#endif // CABIN_PRIMS_H 