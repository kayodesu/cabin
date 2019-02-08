/*
 * Author: Jia Yang
 */

#ifndef JVM_PRIMITIVE_TYPES_H
#define JVM_PRIMITIVE_TYPES_H

#include <stdbool.h>

void load_primitive_types();

bool is_primitive_class_name(const char *class_name);
bool is_primitive_descriptor(char descriptor);

const char* pt_get_array_class_name_by_class_name(const char *class_name);
const char* pt_get_class_name_by_descriptor(char descriptor);
char pt_get_descriptor_by_class_name(const char *class_name);


#endif //JVM_PRIMITIVE_TYPES_H
