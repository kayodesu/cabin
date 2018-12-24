/*
 * Author: Jia Yang
 */

#ifndef JVM_VECTOR_H
#define JVM_VECTOR_H

#include <stddef.h>
#include <stdbool.h>
#include <assert.h>

struct vector {
    void **data;
    size_t len;  // 当前长度
    size_t size; // 总容量
};

bool vector_init(struct vector *vec);
struct vector* vector_create();

//static inline int vector_len(const struct vector *vec)
//{
//    assert(vec != NULL);
//    return vec->len;
//}

#define vector_len(vec) (vec)->len
#define vector_empty(vec) (vector_len(vec) == 0)

static inline void* vector_get(struct vector *vec, int index)
{
    assert(vec != NULL);
    assert(0 <= index && index < vec->len);
    return vec->data[index];
}

static inline void* vector_rget(struct vector *vec, int rindex)
{
    return vector_get(vec, vec->len - 1 - rindex);
}

void vector_push_back(struct vector *vec, void *value);

static inline void* vector_back(struct vector *vec)
{
    assert(vec != NULL);
    return vec->len == 0 ? NULL : vec->data[vec->len - 1];
}

static inline void* vector_pop_back(struct vector *vec)
{
    assert(vec != NULL);
    return vec->len == 0 ? NULL : vec->data[--vec->len];
}

/*
 * 由调用者释放返回的array
 */
void** vector_to_array(const struct vector *vec, int *len);

void vector_clear(struct vector *vec);

void vector_release(struct vector *vec);
void vector_destroy(struct vector *vec);

#endif //JVM_VECTOR_H
