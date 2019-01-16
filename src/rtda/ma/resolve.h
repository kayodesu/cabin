/*
 * Author: Jia Yang
 */

#ifndef JVM_RESOLVE_H
#define JVM_RESOLVE_H

#include <stdint.h>

struct class;
struct method;
struct field;

struct class* resolve_class(struct class *visitor, int cp_index);

struct method* resolve_method(struct class *visitor, int cp_index);

struct field* resolve_field(struct class *visitor, int cp_index);

struct object* resolve_string(struct class *c, int cp_index);

uintptr_t resolve_single_constant(struct class *c, int cp_index);

#endif //JVM_RESOLVE_H
