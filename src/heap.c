#include <assert.h>
#include <string.h>
#include "heap.h"
#include "../cabin.h"

typedef struct heap_node Node;

static Node *create_node(address head, size_t len, Node *next)
{
    Node *n = vm_malloc(sizeof(Node));
    n->head = head;
    n->len = len;
    n->next = next;
    return n;
}

Heap *create_heap()
{
    Heap *h = vm_malloc(sizeof(Heap));

    h->size = VM_HEAP_SIZE;
    h->mem = (address) vm_malloc(h->size);
    h->freelist = create_node(h->mem, h->size, NULL);
    
    pthread_mutex_init(&h->mutex, &g_pthread_mutexattr_recursive);

    return h;
}

void destroy_heap(Heap *heap)
{    
    for (Node *p = heap->freelist; p != NULL;) {
        Node *t = p->next;
        free(p);
        p = t;
    }

    free((void *) heap->mem);
}

#define IS_IN(heap, p) (heap->mem <= p && p < heap->mem + heap->size)

address jump_freelist(Heap *heap, address p)
{
    assert(heap != NULL);
    assert(IS_IN(heap, p));

    Node *curr = heap->freelist;
    for (; curr != NULL; curr = curr->next) {
        if (p < curr->head)
            return p; // p is not in freelist
        address offset = p - curr->head;
        if (0 <= offset && offset <= curr->len) {
            // p is in freelist, jump
            return p + curr->len;
        }
    }

    return p; // p is not in freelist
}

void *heap_malloc(Heap *heap, size_t len)
{
    assert(heap != NULL);
    lock_heap(heap);

    void *p = NULL;
    Node *prev = NULL;
    Node *curr = heap->freelist;
    for (; curr != NULL; prev = curr, curr = curr->next) {
        if (curr->len == len) {
            if (prev != NULL) {
                prev->next = curr->next;
            } else {
                assert(curr == heap->freelist);
                heap->freelist = curr->next;
            }
            p = (void *) curr->head;
            free(curr);

            goto over;
        }

        if (curr->len > len) {
            p = (void *) (curr->head);
            curr->head += len;
            curr->len -= len;

            goto over;
        }
    }

over:
    unlock_heap(g_heap);

    if (p != NULL) {
        memset(p, 0, len);
        return p;
    }

//    throw "java_lang_OutOfMemoryError";
    JVM_PANIC("java_lang_OutOfMemoryError"); // todo 堆可以扩张
}

void heap_free(Heap *heap, address p, size_t len)
{    
    assert(heap != NULL);
    assert(IS_IN(heap, p));

    lock_heap(heap);

    Node *prev = NULL;
    Node *curr = heap->freelist;
    for (; curr != NULL; prev = curr, curr = curr->next) {
        if (p > curr->head)
            continue;

        assert(p + len <= curr->head);
        if (prev == NULL) {
            assert(curr == heap->freelist);
            if (p + len == curr->head) { // 空间右连续
                curr->head = p;
                curr->len += len;
                goto over;
            } else { // 右不连续
                heap->freelist = create_node(p, len, curr);
                goto over;
            }
        }

        assert(prev != NULL);
        assert(prev->head + prev->len <= p);
        if (prev->head + prev->len == p) {
            if (p + len == curr->head) {
                // 左右都连续
                prev->len += (len + curr->len);
                prev->next = curr->next;
                free(curr);
                goto over;
            } else {
                // 左连续，右不连续
                prev->len += len;
                goto over;
            }
        } else {
            if (p + len == curr->head) {
                // 左不连续，右连续
                curr->len += len;
                curr->head = p;
                goto over;
            } else {
                // 左右都不连续
                prev->next = create_node(p, len, curr);
                goto over;
            }
        }
    }

over:
    unlock_heap(g_heap);
}

size_t heap_free_memory(Heap *heap)
{
    assert(heap != NULL);

    size_t free_mem = 0;
    for (Node *node = heap->freelist; node != NULL; node = node->next) {
        free_mem += node->len;
    }
    return free_mem;
}

void print_heap_info(Heap *heap)
{
    assert(heap != NULL);

    lock_heap(heap);

    printf("freelist: \n|");
    for (Node *node = heap->freelist; node != NULL; node = node->next) {
        printf("%p,", (void *) node->head);
        printf("%zu|", node->len);
    }

    unlock_heap(g_heap);
}