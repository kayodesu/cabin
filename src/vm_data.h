/*
 * Author: Jia Yang
 */

#ifndef JVM_VM_DATA_H
#define JVM_VM_DATA_H

#include <stdint.h>
#include "jtypes.h"
#include "jvm.h"

typedef intptr_t* vm_data;

#define vm_data_init(vd, len) vd = vm_malloc((len)*sizeof(intptr_t))
#define vm_data_size(len) (sizeof(intptr_t)*(len))

#define VM_DATA_INT(vd, i)    *(jint *) ((vd) + (i))
#define VM_DATA_FLOAT(vd, i)  *(jfloat *) ((vd) + (i))
#define VM_DATA_LONG(vd, i)   *(jlong *) ((vd) + (i))
#define VM_DATA_DOUBLE(vd, i) *(jdouble *) ((vd) + (i))
#define VM_DATA_REF(vd, i)    *(jref *) ((vd) + (i))

#endif //JVM_VM_DATA_H
