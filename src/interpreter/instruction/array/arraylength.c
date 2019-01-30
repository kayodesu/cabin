#include "../../../rtda/thread/frame.h"
#include "../../../rtda/heap/object.h"
#include "../../../rtda/heap/arrobj.h"
#include "../../../rtda/thread/thread.h"

/*
 * Author: Jia Yang
 */

void arraylength(struct frame *frame)
{
    struct object *o = frame_stack_popr(frame);
    if (o == NULL) {
        thread_throw_null_pointer_exception();
    }
    if (!object_is_array(o)) {
        vm_unknown_error("not a array");
    }

    frame_stack_pushi(frame, arrobj_len(o));
}
