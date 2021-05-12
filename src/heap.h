#ifndef CABIN_HEAP_H
#define CABIN_HEAP_H

#include <stddef.h>
#include <stdint.h>
#include <pthread.h>

typedef uintptr_t address;

typedef struct heap {
    address mem;
    size_t size; // 堆总大小，以字节为单位。

    struct heap_node {
        address head;
        size_t len;
        struct heap_node *next;
    } *freelist;

    pthread_mutex_t mutex;
} Heap;

Heap *create_heap();
void destroy_heap(Heap *);

// heap malloc
void *heap_malloc(Heap *heap, size_t len);

// heap free
void heap_free(Heap *heap, address p, size_t len);

#define lock_heap(heap) pthread_mutex_lock(&((heap)->mutex))
#define unlock_heap(heap) pthread_mutex_unlock(&((heap)->mutex))

/*
 * 如果不在 freelist 里面，返回 p，
 * 负责跳过此 freelist's Node.
 */
address jump_freelist(Heap *, address p);

// 堆还有多少剩余空间，以字节为单位。
size_t heap_free_memory(Heap *);

void print_heap_info(Heap *);

#endif //CABIN_HEAP_H
