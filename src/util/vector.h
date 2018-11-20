/*
 * Author: Jia Yang
 */

#ifndef JVM_VECTOR_H
#define JVM_VECTOR_H

struct vector* vector_create();

int vector_len(struct vector *vec);

void vector_push_back(struct vector *vec, void *value);

void* vector_back(struct vector *vec);

void* vector_pop_back(struct vector *vec);

void vector_destroy(struct vector *vec);

#endif //JVM_VECTOR_H
