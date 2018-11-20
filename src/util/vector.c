/*
 * Author: Jia Yang
 */

#include <stddef.h>
#include <assert.h>
#include <string.h>
#include "../jvm.h"

struct vector {
    void **data;
    size_t len;  // 当前长度
    size_t size; // 总容量
};

#define DEFAULT_INITIAL_CAPACITY 128

struct vector* vector_create()
{
    VM_MALLOC(struct vector, vec);
    vec->len = 0;
    vec->size = DEFAULT_INITIAL_CAPACITY;
    vec->data = malloc(sizeof(*(vec->data)) * vec->size);  // todo NULL
    return vec;
}

int vector_len(struct vector *vec)
{
    assert(vec != NULL);
    return vec->len;
}

void vector_push_back(struct vector *vec, void *value)
{
    assert(vec != NULL);

    if (vec->len + 1 > vec->size) {
        vec->size <<= 1;  // todo 溢出
        VM_MALLOCS(void *, vec->size, new_data);
        memcpy(new_data, vec->data, vec->size);
        free(vec->data);
        vec->data = new_data;
    }
    vec->data[vec->len++] = value;
}

void* vector_back(struct vector *vec)
{
    assert(vec != NULL);
    return vec->len == 0 ? NULL : vec->data[vec->len - 1];
}

void* vector_pop_back(struct vector *vec)
{
    assert(vec != NULL);
    if (vec->len == 0) {
        // todo error
        return NULL;
    } else {
        return vec->data[--vec->len];
    }
}

void vector_destroy(struct vector *vec)
{
    assert(vec != NULL);
    free(vec->data);
    free(vec);
}