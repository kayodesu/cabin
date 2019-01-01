/*
 * Author: Jia Yang
 */

#include <stdlib.h>
#include "halloc.h"
#include "../../../jvm.h"

void *halloc(size_t len)
{
    // 分配内存的时候就清空，后面创建对象时就不会有清空的动作了
    void *p = calloc(len, 1);  // todo 暂时先这么搞
//    printf("-------------------------            %p, %d\n", p, len);
    if (p == NULL) {
        vm_stack_overflow_error();
    }
    return p;
}

void hfree(void *p)
{
    if (p == NULL)
        return;

    // todo
}