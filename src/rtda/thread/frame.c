/*
 * Author: Jia Yang
 */

#include <stdlib.h>
#include <assert.h>
#include "frame.h"
#include "../ma/access.h"
#include "thread.h"

char *frame_to_string(const Frame *f)
{
    assert(f != NULL);
    int len = strlen(f->method->clazz->class_name) + strlen(f->method->name) + strlen(f->method->descriptor) + 16;
    char *buf = vm_malloc(sizeof(char) * len);
    strcpy(buf, IS_NATIVE(f->method->access_flags) ? "(native)" : "");
    strcat(buf, f->method->clazz->class_name);
    strcat(buf, "~");
    strcat(buf, f->method->name);
    strcat(buf, "~");
    strcat(buf, f->method->descriptor);
    return buf; // todo
}
