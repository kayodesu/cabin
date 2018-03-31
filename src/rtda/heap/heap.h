/*
 * Author: Jia Yang
 */

#ifndef JVM_HEAP_H
#define JVM_HEAP_H

#include <cstdlib>

// heap alloc
static void* halloc(size_t len) {
    return malloc(len);
}

//static void heapFree(void *p)
//{
//    if (p == NULL)
//        return;
//
//    free(p);
//}

#endif //JVM_HEAP_H
