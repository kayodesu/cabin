/*
 * Author: Jia Yang
 */

#ifndef JVM_HALLOC_H
#define JVM_HALLOC_H

#include <limits.h>

// heap alloc
// 从堆中分配大小为len的空间，并清0。
void *halloc(size_t len);

void hfree(void *p);


#define HEAP_ALLOCS(type, count, var_name) type *(var_name) = halloc(sizeof(type) * (count))
#define HEAP_ALLOC(type, var_name) HEAP_ALLOCS(type, 1, var_name)

#endif //JVM_HALLOC_H
