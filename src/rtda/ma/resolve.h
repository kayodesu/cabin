/*
 * Author: Jia Yang
 */

#ifndef JVM_RESOLVE_H
#define JVM_RESOLVE_H

#include <stdint.h>
#include "../../jvmdef.h"

Class* resolve_class(Class *visitor, int cp_index);

Method* resolve_method(Class *visitor, int cp_index);

Field* resolve_field(Class *visitor, int cp_index);

Object* resolve_string(Class *c, int cp_index);

uintptr_t resolve_single_constant(Class *c, int cp_index);

#endif //JVM_RESOLVE_H
