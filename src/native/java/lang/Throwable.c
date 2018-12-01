/*
 * Author: Jia Yang
 */

#include "../../registry.h"
#include "../../../interpreter/stack_frame.h"
#include "../../../slot.h"
#include "../../../rtda/heap/jobject.h"

struct stack_trace_element {
    char file_name[PATH_MAX]; // todo
    char class_name[PATH_MAX];
    char method_name[PATH_MAX];
    int line_number;
};

// private native Throwable fillInStackTrace(int dummy);
static void fillInStackTrace(struct stack_frame *frame)
{
//    jvm_abort("error");
    // todo
//    JObject *throwable = frame->getLocalVar(0).getRef();
//    // todo
//    frame->operandStack.push(throwable);

    struct jobject *throwable = slot_getr(frame->local_vars);
    os_pushr(frame->operand_stack, throwable);

    int num;
    struct stack_frame **frames = jthread_get_frames(frame->thread, &num);
    if (num > 0) {
        VM_MALLOCS(struct stack_trace_element, num, eles);

        for (int i = 0; i < num; i++) {
            strcpy(eles[i].file_name, frames[i]->method->jclass->source_file_name);
            strcpy(eles[i].class_name, frames[i]->method->jclass->class_name);
            strcpy(eles[i].method_name, frames[i]->method->name);
            eles[i].line_number = jmethod_get_line_number(frames[i]->method, bcr_get_pc(frames[i]->reader) - 1); // todo why 减1？
        }

        free(frames);
        throwable->extra = eles;
    }
}

void java_lang_Throwable_registerNatives()
{
    register_native_method("java/lang/Throwable", "registerNatives", "()V", empty_method);
    register_native_method("java/lang/Throwable", "fillInStackTrace", "(I)Ljava/lang/Throwable;", fillInStackTrace);
}