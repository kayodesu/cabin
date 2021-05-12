#include <assert.h>
#include "frame.h"
#include "../metadata/class.h"

void init_frame(Frame *_this, Method *m, bool vm_invoke, slot_t *lvars, slot_t *ostack, Frame *prev)
{
    assert(m != NULL && lvars != NULL && ostack != NULL);

    _this->method = m;
    _this->vm_invoke = vm_invoke;
    _this->lvars = lvars;
    _this->ostack = ostack;
    _this->prev = prev;

    bcr_init(&_this->reader, m->code, m->code_len);
}

char *get_frame_info(const Frame *f)
{
    assert(f != NULL);

    char *info_buf = (char *) vm_malloc(sizeof(char) * INFO_MSG_MAX_LEN);
    Method *m = f->method;
    int n = snprintf(info_buf, INFO_MSG_MAX_LEN - 1, "(%p)%s%s~%s~%s, pc = %u", 
                    f, ACC_IS_NATIVE(m->access_flags) ? "(native)" : "", 
                    m->clazz->class_name, m->name, m->descriptor, f->reader.pc);
    if (n == -1) {
        return NULL; // todo error
    }
    info_buf[n] = 0;
    return info_buf;
}