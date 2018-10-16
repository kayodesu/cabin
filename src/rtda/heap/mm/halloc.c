/*
 * Author: Jia Yang
 */

#include <stdlib.h>
#include "halloc.h"

void *halloc(size_t len)
{
    return calloc(1, len);  // todo 暂时先这么搞
}

void hfree(void *p)
{
    if (p == NULL)
        return;

    // todo
}