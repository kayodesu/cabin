/*
 * Author: Jia Yang
 */

#include <assert.h>
#include "heap_mgr.h"
#include "../../../jvm.h"

void hm_init(struct heap_mgr *hm)
{
    assert(hm != NULL);

    void *p = vm_calloc(1, g_initial_heap_size);
    hm->heap = p;
    hm->index = 0;
    hm->free_list.head = (uintptr_t) p;
    hm->free_list.len = g_initial_heap_size;
    hm->free_list.next = NULL;
}

void* hm_get0(struct heap_mgr *hm, size_t len)
{
    for (struct free_list_node *node = &(hm->free_list); node != NULL && node->len < len; node = node->next);

    if (hm->free_list.len >= len) {
        void *p = (void *) hm->free_list.head;
        size_t redundance = hm->free_list.len - len;
        if (redundance == 0) {
            hm->free_list.head = hm->free_list.next;
        } else {
            hm->free_list.head += len;
            hm->free_list.len -= len;
        }
        return p;
    }
}

void* hm_get(struct heap_mgr *hm, size_t len)
{
    assert(hm != NULL);

    size_t new_index = hm->index + len;
    if (new_index > g_initial_heap_size) {
        vm_stack_overflow_error(); // todo 堆可以扩张
        return NULL;
    }

    void *p = hm->heap + hm->index;
    hm->index = new_index;
    return p;
}
