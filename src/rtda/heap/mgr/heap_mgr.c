/*
 * Author: Jia Yang
 */

#include <assert.h>
#include "heap_mgr.h"
#include "../../../jvm.h"
#include "../../../config.h"

void hm_init(HeapMgr *hm)
{
    assert(hm != NULL);

    void *p = vm_calloc(1, VM_HEAP_SIZE);
    hm->heap = p;
    hm->index = 0;
    hm->free_list.head = (uintptr_t) p;
    hm->free_list.len = VM_HEAP_SIZE;
    hm->free_list.next = NULL;
}

void* hm_get(HeapMgr *hm, size_t len)
{
    assert(hm != NULL);

    size_t new_index = hm->index + len;
    if (new_index > VM_HEAP_SIZE) {
        vm_stack_overflow_error(); // todo 堆可以扩张
        return NULL;
    }

    void *p = hm->heap + hm->index;
    hm->index = new_index;
    return p;
}
