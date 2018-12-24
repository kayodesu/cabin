/*
 * Author: Jia Yang
 */

#include <string.h>
#include "../jvm.h"
#include "vector.h"


#define DEFAULT_INITIAL_CAPACITY 128

bool vector_init(struct vector *vec)
{
    assert(vec != NULL);
    vec->len = 0;
    vec->size = DEFAULT_INITIAL_CAPACITY;
    vec->data = malloc(sizeof(*(vec->data)) * vec->size);
    return vec->data != NULL;
}

struct vector* vector_create()
{
    struct vector *vec = malloc(sizeof(struct vector));
    if (vec == NULL) {
        return NULL;
    }

    if (!vector_init(vec)) {
        free(vec);
        return NULL;
    }
    return vec;
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

void vector_release(struct vector *vec)
{
    assert(vec != NULL);
    free(vec->data);
}

void vector_destroy(struct vector *vec)
{
    assert(vec != NULL);
    vector_release(vec);
    free(vec);
}
