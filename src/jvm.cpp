/*
 * Author: Jia Yang
 */

#include "interpreter/interpreter.h"
#include "rtda/heap/methodarea/Jmethod.h"

using namespace std;

static void initJVM(ClassLoader *&loader, Jthread *&mainThread) {
    Jclass *vmClass = loader->loadClass("sun/misc/VM");
    if (vmClass == nullptr) {
        jvmAbort("error %s\n", vmClass->toString().c_str());  // todo throw exception
        return;
    }

    Jmethod *initMethod = vmClass->lookupStaticMethod("initialize", "()V");
    if (initMethod == nullptr) {
        jvmAbort("error %s\n", vmClass);
    }

    auto initFrame = new StackFrame(mainThread, initMethod);
//    vmClass->clinit(initFrame);
    mainThread->pushFrame(initFrame);
    interpret(mainThread);
}

ClassLoader *classLoader = nullptr;

Jthread *mainThread = nullptr;
Jobject *mainThreadGroup = nullptr; // 主线程坐在的 thread group

void createMainThreadGroup() {
    auto threadGroupClass = classLoader->loadClass("java/lang/ThreadGroup");
    mainThreadGroup = new Jobject(threadGroupClass);

    Jmethod *constructor = threadGroupClass->getConstructor("()V");
//    auto frame = new StackFrame(mainThread, constructor);
    auto *frame = new StackFrame(mainThread, constructor);
//    threadGroupClass->clinit(frame);  todo
//    frame.operandStack.push(mainThreadGroup);
    frame->setLocalVars(0, Slot::referenceSlot(mainThreadGroup));

    mainThread->pushFrame(frame);
    interpret(mainThread);

    // 无需在这里 delete frame, interpret函数会delete调执行过的frame
}

void startJVM(const string &mainClassName) {
    auto loader = classLoader = new ClassLoader;
    mainThread = new Jthread;

    createMainThreadGroup();
    mainThread->joinToMainThreadGroup();

    //initJVM(loader, mainThread);
    jprintf("LOADING main class!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n");

    Jclass *mainClass = loader->loadClass(mainClassName);
//    if (mainClass == nullptr) {
//        jvmAbort("error %s\n", mainClassName);  // todo
//        return;
//    }

    Jmethod *mainMethod = mainClass->lookupMethod("main", "([Ljava/lang/String;)V");
    if (mainMethod == nullptr) {
        // todo
        jvmAbort("没有找到main方法\n");  // todo
    } else {
        if (!mainMethod->isPublic()) {
            // todo
            jvmAbort("..................");
        }
        if (!mainMethod->isStatic()) {
            // todo
            jvmAbort("...................");
        }
    }

    mainThread->pushFrame(new StackFrame(mainThread, mainMethod));
    interpret(mainThread);

    delete mainThread;
    delete loader;
}