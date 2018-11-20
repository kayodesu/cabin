#include <stdio.h>
#include "../src/util/vector.h"

/*
 * Author: Jia Yang
 */

void test_vector()
{
    int i1 = 1, i2 = 2, i3 = 3;
    struct vector *vec = vector_create();
    vector_push_back(vec, &i1);
    vector_push_back(vec, &i2);
    vector_push_back(vec, &i3);

    printf("%d, len = %d\n", *(int *)vector_back(vec), vector_len(vec));
    printf("len = %d\n", vector_len(vec));
    vector_pop_back(vec);

    printf("%d, len = %d\n", *(int *)vector_back(vec), vector_len(vec));
    printf("len = %d\n", vector_len(vec));
    vector_pop_back(vec);

    printf("%d, len = %d\n", *(int *)vector_back(vec), vector_len(vec));
    printf("len = %d\n", vector_len(vec));
    vector_pop_back(vec);
    printf("len = %d\n", vector_len(vec));
}