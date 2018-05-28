/*
 * Author: Jia Yang
 */

#include <sstream>
#include "StackFrame.h"
#include "../rtda/thread/Jthread.h"
#include "../rtda/heap/methodarea/Jmethod.h"

using namespace std;

StackFrame::StackFrame(Jthread *thread, Jmethod *method)
        : interruptedStatus(false), exeStatus(false), procExceptionStatus(false) {
    assert(thread != nullptr);
    assert(method != nullptr);

    localVars = new Slot*[method->getMaxLocals()];
    maxLocals = method->getMaxLocals();

//    for (int i = 0; i < maxLocals; i++) {
//        localVars[i]->type = INVALID_JVM_TYPE;  // todo
//    }

//    // 准备参数
//    for (int i = method->getArgSlotCount() - 1; i >= 0; i--) {
//        setLocalVars(i, operandStack.popSlot());
//    }

    this->method = method;
    this->thread = thread;

    reader = new BytecodeReader(method->code, method->codeLength);
}

void StackFrame::invokeMethod(Jmethod *method, Slot **args) {
    assert(method != nullptr);
//    if (method->isNative()) {  //todo
//        auto nativeMethod = findNativeMethod(method->jclass->className, method->name, method->descriptor);
//        if (nativeMethod == nullptr) {
//            jvmAbort("error. not find native function: %s, %s, %s\n",
//                       method->jclass->className.c_str(), method->name.c_str(), method->descriptor.c_str());    // todo
//        }
//        nativeMethod(this);
//        return;
//    }

    auto newFrame = new StackFrame(thread, method);
    thread->pushFrame(newFrame);

    // 准备参数
    if (args != nullptr) {
        for (int i = 0; i < method->getArgSlotCount(); i++) {
            newFrame->setLocalVars(i, args[i]);
        }
    } else {
        for (int i = method->getArgSlotCount() - 1; i >= 0; i--) {
            newFrame->setLocalVars(i, operandStack.popSlot());
        }
    }

    // 中断 invokeFrame 的执行，执行 new_frame
    interrupt();

    // todo
}

string StackFrame::toString() {
    stringstream ss;
    ss << method->toString();
    ss << " [Operand Stack count: ";
    ss << operandStack.size();
    ss << "]";
    return ss.str();
}
