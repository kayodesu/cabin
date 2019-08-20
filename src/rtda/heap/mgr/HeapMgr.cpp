/*
 * Author: kayo
 */

#include <cstring>
#include "HeapMgr.h"
#include "../../../jvm.h"
#include "../../../config.h"

HeapMgr::HeapMgr()
{
//    heap = new u1[VM_HEAP_SIZE];
//    memset(heap, 0, VM_HEAP_SIZE); // todo 不应该这么搞
    heap = (u1 *)vm_calloc(1, VM_HEAP_SIZE);

    index = 0;
    free_list.head = (uintptr_t) heap;
    free_list.len = VM_HEAP_SIZE;
    free_list.next = NULL;
}

void *HeapMgr::get(size_t len)
{
    size_t new_index = index + len;
    if (new_index > VM_HEAP_SIZE) {
        vm_stack_overflow_error(); // todo 堆可以扩张
        return nullptr;
    }

    void *p = heap + index;
    index = new_index;
    return p;
}
