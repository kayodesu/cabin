/*
 * Author: Jia Yang
 */

#include <windows.h> // todo
#include "../../registry.h"
#include "../../../jvm.h"
#include "../../../rtda/thread/frame.h"

// public native int availableProcessors();
static void availableProcessors(struct frame *frame)
{
    // todo 这是windows 方式，不利于平台移植
    SYSTEM_INFO sysInfo;
    GetSystemInfo(&sysInfo);
    frame_stack_pushi(frame, (jint) sysInfo.dwNumberOfProcessors);
}

// public native long freeMemory();
static void freeMemory(struct frame *frame)
{
    jvm_abort("");
}

void java_lang_Runtime_registerNatives()
{
    register_native_method("java/lang/Runtime~registerNatives~()V", registerNatives);
    register_native_method("java/lang/Runtime~availableProcessors~()I", availableProcessors);
    register_native_method("java/lang/Runtime~freeMemory~()J", freeMemory);
}