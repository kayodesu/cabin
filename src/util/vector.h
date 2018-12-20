/*
 * Author: Jia Yang
 */

#ifndef JVM_VECTOR_H
#define JVM_VECTOR_H

struct vector* vector_create();

int vector_len(const struct vector *vec);

void vector_push_back(struct vector *vec, void *value);

void* vector_back(struct vector *vec);

void* vector_pop_back(struct vector *vec);

/*
 * 由调用者释放返回的array
 */
void** vector_to_array(const struct vector *vec, int *len);

void vector_clear(struct vector *vec);

void vector_destroy(struct vector *vec);

#endif //JVM_VECTOR_H
