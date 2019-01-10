#include "../../../rtda/thread/frame.h"
#include "../../../rtda/heap/object.h"
#include "../../../rtda/heap/arrobj.h"

/*
 * Author: Jia Yang
 */


void arraylength(struct frame *frame)
{
    struct object *o = frame_stack_popr(frame);
    if (o == NULL) {
        thread_throw_null_pointer_exception(frame->thread);
    }
    if (!object_is_array(o)) {
        vm_unknown_error("not a array");
    }

    frame_stack_pushi(frame, arrobj_len(o));
}