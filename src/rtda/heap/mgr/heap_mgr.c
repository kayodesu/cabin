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

#if 0
    // 分配内存的时候就清空，后面创建对象时就不会有清空的动作了
    void *p = calloc(len, 1);  // todo 暂时先这么搞
//    printf("-------------------------            %p, %d\n", p, len);
    if (p == NULL) {
        vm_stack_overflow_error(); // todo 堆可以扩张
    }
    return p;
#endif
}
