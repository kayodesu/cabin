#ifndef CABIN_EXCEPTION_H
#define CABIN_EXCEPTION_H

#include "cabin.h"

void raise_exception(const char *exception_class_name, const char *msg);
void set_exception(jref e);
Object *exception_occurred();
void clear_exception();
void print_stack_trace(Object *e);

#endif // CABIN_EXCEPTION_H