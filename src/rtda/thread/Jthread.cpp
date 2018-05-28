/*
 * Author: Jia Yang
 */

#include "Jthread.h"
#include "../heap/objectarea/JstringObj.h"
#include "../../interpreter/interpreter.h"

Jthread::Jthread() {
    auto jlThreadClass = classLoader->loadClass("java/lang/Thread");
    jlThreadObj = new Jobject(jlThreadClass);
    Jvalue v;
    v.i = 1;  // todo. why 1? I do not know. 参见 jvmgo/instructions/reserved/bootstrap.go
    jlThreadObj->setInstanceFieldValue("priority", "I", v);

    // 置此线程的 ThreadGroup
    if (mainThreadGroup != nullptr) {  // todo
//        auto constructor = jlThreadClass->getConstructor("(Ljava/lang/ThreadGroup;Ljava/lang/String;)V");
//        auto frame = new StackFrame(this, constructor);
//        frame->operandStack.push(jlThreadObj);
//        assert(mainThreadGroup != nullptr);
//        frame->operandStack.push(mainThreadGroup);
//        JstringObj *o = new JstringObj(classLoader, strToJstr("main"));
//        frame->operandStack.push(o);
//
//        pushFrame(frame);
//        interpret(this);
//
//        delete o;
        joinToMainThreadGroup();
    }
}

void Jthread::joinToMainThreadGroup() {
    assert(mainThreadGroup != nullptr);

    auto jlThreadClass = classLoader->loadClass("java/lang/Thread");
    auto constructor = jlThreadClass->getConstructor("(Ljava/lang/ThreadGroup;Ljava/lang/String;)V");
    auto frame = new StackFrame(this, constructor);

//    frame->operandStack.push(jlThreadObj);
//
//    frame->operandStack.push(mainThreadGroup);
    JstringObj *o = new JstringObj(classLoader, strToJstr("main"));
//    frame->operandStack.push(o);

    frame->setLocalVars(0, new ReferenceSlot(jlThreadObj));
    frame->setLocalVars(1, new ReferenceSlot(mainThreadGroup));
    frame->setLocalVars(2, new ReferenceSlot(o));

    pushFrame(frame);
    interpret(this);

    // 无需在这里 delete frame, interpret函数会delete调执行过的frame

    delete o;
}
