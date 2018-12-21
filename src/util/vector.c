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

void* vector_get(struct vector *vec, int index)
{
    assert(vec != NULL);

    if (index < 0 || index >= vec->len) {
        jvm_abort(""); // todo
    }

    return vec->data[index];
}

void* vector_rget(struct vector *vec, int rindex)
{
    return vector_get(vec, vec->len - 1 - rindex);
}

int vector_len(const struct vector *vec)
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

void** vector_to_array(const struct vector *vec, int *len)
{
    assert(vec != NULL);
    assert(len != NULL);

    *len = vec->len;
    if (*len <= 0) {
        return NULL;
    }

    VM_MALLOCS(void *, vec->len, arr);
    memcpy(arr, vec->data, vec->len * sizeof(void *));
    return arr;
}

void vector_clear(struct vector *vec)
{
    assert(vec != NULL);
    vec->len = 0;
}

void vector_destroy(struct vector *vec)
{
    assert(vec != NULL);
    free(vec->data);
    free(vec);
}