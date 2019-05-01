/*
 * Author: Jia Yang
 */

#ifndef JVM_HEAP_MANAGER_H
#define JVM_HEAP_MANAGER_H

#include <stddef.h>
#include <stdint.h>
#include "../../../jtypes.h"

struct free_list_node {
    uintptr_t head;
    size_t len;
    uintptr_t next;
};

typedef struct heap_mgr {
    struct free_list_node free_list;

    u1 *heap;
    size_t index;
} HeapMgr;

void hm_init(HeapMgr *hm);

void *hm_get(HeapMgr *hm, size_t len);

#endif //JVM_HEAP_MANAGER_H
