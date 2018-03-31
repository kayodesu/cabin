/*
 * Author: Jia Yang
 */

#ifndef JVM_REFERENCES_H
#define JVM_REFERENCES_H

#include <string>
#include <iostream>
#include "../interpreter/StackFrame.h"
#include "../rtda/heap/objectarea/Jobject.h"
#include "../rtda/heap/objectarea/JarrayObj.h"
#include "../rtda/heap/methodarea/resolver.h"

using namespace std;

static void __new(StackFrame *frame) {
    BytecodeReader *reader = frame->reader;
    int s = reader->readu2(); // todo reads2  ?????

    const string className = frame->method->jclass->rtcp->getClassName(s);
    if (className.empty()) {
        jvmAbort("error. className is empty\n");
    }

    Jclass *c = resolveClass(frame->method->jclass, className);  // todo
    if (!c->isInited) {
        c->clinit(frame);
        frame->reader->setPc(frame->thread->pc); // 将pc指向本条指令之前，初始化完类后，继续执行本条指令。
        return;
    }

    if (c->isInterface() || c->isAbstract()) {
//        // todo 抛出 InstantiationError 异常
        jvmAbort("error\n");
    }

    Jobject *o;
    if (c->className == "java/lang/String") {
        o = new JstringObj(classLoader); // todo ............................
    } else {
        o = new Jobject(c);
    }

    frame->operandStack.push(o);
}

/*
 * putstatic指令给类的某个静态变量赋值，它需要两个操作数。
 * 第一个操作数是uint16索引，来自字节码。通过这个索引可以从当
 *    前类的运行时常量池中找到一个字段符号引用，解析这个符号引用
 *    就可以知道要给类的哪个静态变量赋值。
 * 第二个操作数是要赋给静态变量的值，从操作数栈中弹出。
 */
static void putstatic(StackFrame *frame) {

    Jclass *currClass = frame->method->jclass;

    int index = frame->reader->readu2();
    FieldRef &ref = currClass->rtcp->getFieldRef(index);

    Jclass *jclass = resolveClass(currClass, ref.className);
    if (!jclass->isInited) {
        jclass->clinit(frame);
        frame->reader->setPc(frame->thread->pc); // 将pc指向本条指令之前，初始化完类后，继续执行本条指令。
        return;
    }

    Jfield *field = resolveField(currClass, ref, true);
    if (field == nullptr) {
        // todo
        jvmAbort("error\n");
    }

    if (!field->isStatic()) {
        jvmAbort("java.lang.IncompatibleClassChangeError\n");
    }

    /*
     * 如果是final字段，则实际操作的是静态常量，只能在类初始化方法中给它赋值。
     * 否则，会抛出 IllegalAccessError异常。类初始化方法由编译器生成，名字是<clinit>，
     */
    if (field->isFinal()) {
        // todo
        if (currClass != jclass || frame->method->name != "<clinit>") {
            // panic("java.lang.IllegalAccessError")
            jvmAbort("error\n");
        }
    }

    OperandStack &os = frame->operandStack;
    if (strchr("BCISZ", field->descriptor[0]) != NULL) {
        jclass->staticFieldValues[field->id].i = os.popInt();
    } else if (field->descriptor[0] == 'F') {
        jclass->staticFieldValues[field->id].f = os.popFloat();
    } else if (field->descriptor[0] == 'J') {
        jclass->staticFieldValues[field->id].l = os.popLong();
    } else if (field->descriptor[0] == 'D') {
        jclass->staticFieldValues[field->id].d = os.popDouble();
    } else if (field->descriptor[0] == 'L' || field->descriptor[0] == '[') {
        jclass->staticFieldValues[field->id].r = os.popReference();
    } else {
        // todo error
        jvmAbort("error\n");
    }
}

static void getstatic(StackFrame *frame) {
    Jclass *currClass = frame->method->jclass;

    int index = frame->reader->readu2();
    FieldRef &ref = currClass->rtcp->getFieldRef(index);

    Jclass *jclass = resolveClass(currClass, ref.className);
    if (!jclass->isInited) {
        jclass->clinit(frame);
        frame->reader->setPc(frame->thread->pc); // 将pc指向本条指令之前，初始化完类后，继续执行本条指令。
        return;
    }

    Jfield *field = resolveField(currClass, ref, true);
    if (field == NULL) {
        // todo
        jvmAbort("error\n");
    }

    if (!field->isStatic()) {
        jvmAbort("java.lang.IncompatibleClassChangeError\n");
    }

    // todo 如果声明字段的类还没有初始化好，也需要先初始化。

    OperandStack &os = frame->operandStack;
    if (strchr("BCISZ", field->descriptor[0]) != NULL) {
        os.push(jclass->staticFieldValues[field->id].i);
    } else if (field->descriptor[0] == 'F') {
        os.push(jclass->staticFieldValues[field->id].f);
    } else if (field->descriptor[0] == 'J') {
        os.push(jclass->staticFieldValues[field->id].l);
    } else if (field->descriptor[0] == 'D') {
        os.push(jclass->staticFieldValues[field->id].d);
    } else if (field->descriptor[0] == 'L' || field->descriptor[0] == '[') {
        os.push(jclass->staticFieldValues[field->id].r);
    } else {
        // todo error
        jvmAbort("error\n");
    }
}

/*
 * putfield指令给实例变量赋值，它需要三个操作数。
 * 前两个操作数是常量池索引和变量值，用法和putstatic一样。
 * 第三个操作数是对象引用，从操作数栈中弹出
 */
static void putfield(StackFrame *frame) {
    Jclass *currClass = frame->method->jclass;

    int index = frame->reader->readu2();
    FieldRef &ref = currClass->rtcp->getFieldRef(index);

    auto jclass = resolveClass(currClass, ref.className);
    if (!jclass->isInited) {
        jclass->clinit(frame);
        frame->reader->setPc(frame->thread->pc); // 将pc指向本条指令之前，初始化完类后，继续执行本条指令。
        return;
    }

    Jfield *field = resolveField(currClass, ref, false);
    if (field == nullptr) {
        // todo
        jvmAbort("error\n");
    }

//    field->print(); //////////////////////////////////////////////////////////////////////  todo

    if (field->isStatic()) {
        jvmAbort("java.lang.IncompatibleClassChangeError\n");
    }

    /*
     * 如果是final字段，则只能在构造函数中初始化，否则抛出java.lang.IllegalAccessError。
     */
    if (field->isFinal()) {
        // todo
        if (currClass != field->jclass || frame->method->name != "<init>") {
            // panic("java.lang.IllegalAccessError")
            jvmAbort("error\n");
        }
    }

    Slot s = frame->operandStack.popSlotJumpInvalid();
    jreference r = frame->operandStack.popReference();
    if (r == nullptr) {
        // todo
        // java.lang.NullPointerException
        jprintf("error\n");
    }
    Jobject *obj = static_cast<Jobject *>(r);

//    obj->setInstanceField(field->id, s.value);

    Jvalue v;
    if (strchr("BCISZ", field->descriptor[0]) != NULL) {
        v.i = s.getInt();
    } else if (field->descriptor[0] == 'F') {
        v.f = s.getFloat();
    } else if (field->descriptor[0] == 'J') {
        v.l = s.getLong();
    } else if (field->descriptor[0] == 'D') {
        v.d = s.getDouble();
    } else if (field->descriptor[0] == 'L' || field->descriptor[0] == '[') {
        v.r = s.getReference();
    } else {
        // todo error
        jvmAbort("error\n");
    }

    obj->setInstanceFieldValue(field->id, v);
}

/*
 * getfield指令获取对象的实例变量值，然后推入操作数栈，它需
 * 要两个操作数。第一个操作数是uint16索引，用法和前面三个指令
 * 一样。第二个操作数是对象引用，用法和putfield一样。
 */
static void getfield(StackFrame *frame) {
    Jclass *currClass = frame->method->jclass;

    int index = frame->reader->readu2();
    FieldRef &ref = currClass->rtcp->getFieldRef(index);

    auto jclass = resolveClass(currClass, ref.className);
    if (!jclass->isInited) {
        jclass->clinit(frame);
        frame->reader->setPc(frame->thread->pc); // 将pc指向本条指令之前，初始化完类后，继续执行本条指令。
        return;
    }

    Jfield *field = resolveField(currClass, ref, false);
    if (field == nullptr) {
        // todo
        jvmAbort("error\n");
    }

    if (field->isStatic()) {
        jvmAbort("java.lang.IncompatibleClassChangeError\n");
    }

    OperandStack &os = frame->operandStack;

    jreference r = os.popReference();
    if (r == nullptr) {
        // todo
        // java.lang.NullPointerException
        jvmAbort("error\n");
    }

    Jobject *obj = static_cast<Jobject *>(r);

    if (strchr("BCISZ", field->descriptor[0]) != NULL) {
        os.push(obj->getInstanceFieldValue(field->id).i);
    } else if (field->descriptor[0] == 'F') {
   //     jprintf("error\n");
        os.push(obj->getInstanceFieldValue(field->id).f);
    } else if (field->descriptor[0] == 'J') {
        os.push(obj->getInstanceFieldValue(field->id).l);
    } else if (field->descriptor[0] == 'D') {
       // jprintf("error\n");
        os.push(obj->getInstanceFieldValue(field->id).d);
    } else if (field->descriptor[0] == 'L' || field->descriptor[0] == '[') {
        os.push(obj->getInstanceFieldValue(field->id).r);
    } else {
        jvmAbort("error  %s\n", field->descriptor);
        // todo error
    }
}

static void athrow(StackFrame *frame) {
    jreference r = frame->operandStack.popReference();
    if (r == nullptr) {
        // todo
        // java.lang.NullPointerException
        jvmAbort("error\n");
    }

    auto exception = static_cast<Jobject *>(r);
    Jthread *currThread = frame->thread;

    // 遍历虚拟机栈找到可以处理此异常的方法
    while (!currThread->isStackEmpty()) {
        StackFrame *top = currThread->topFrame();
        size_t pc = top->reader->pc - 1; // todo why -1 ???
        int handlerPc = top->method->findExceptionHandler(exception->getClass(), pc);
        if (handlerPc >= 0) {  // todo 可以等于0吗
            /*
             * 找到可以处理的函数了
             * 操作数栈清空
             * 把异常对象引用推入栈顶
             * 跳转到异常处理代码之前
             */
            top->operandStack.clear();
            top->operandStack.push(exception);
            top->reader->setPc(handlerPc);  // todo 是setPc还是skip
            top->procException();
            return;
        }

        currThread->popFrame();
    }

    // todo UncaughtException
    JstringObj *o
            = static_cast<JstringObj *>(exception->getInstanceFieldValue("detailMessage", "Ljava/lang/String;").r);
    jvmAbort("UncaughtException. %s.%s\n", exception->toString().c_str(), jstrToStr(o->value()).c_str());
}

/*
 * instanceof指令需要两个操作数。
 * 第一个操作数是uint16索引，从方法的字节码中获取，通过这个索引可以从当前类的运行时常量池中找到一个类符号引用。
 * 第二个操作数是对象引用，从操作数栈中弹出。
 */
static void instanceof(StackFrame *frame) {
    int index = frame->reader->readu2();
    const string &className = frame->method->jclass->rtcp->getClassName(index);

    Jclass *jclass = frame->method->jclass->loader->loadClass(className);  // todo resolve_class ???

    jreference r = frame->operandStack.popReference();
    if (r == nullptr) {
        frame->operandStack.push((jint)0);
        return;
    }

    auto obj = static_cast<Jobject *>(r);
    frame->operandStack.push((jint)(obj->isInstanceOf(jclass) ? 1 : 0));  // todo
}

/*
 * checkcast指令和instanceof指令很像，区别在于：instanceof指令会改变操作数栈（弹出对象引用，推入判断结果）
 * checkcast则不改变操作数栈（如果判断失败，直接抛出ClassCastException异常）
 */
static void checkcast(StackFrame *frame) {
    jreference r = frame->operandStack.peek().getReference();
    int index = frame->reader->readu2();
    if (r == nullptr) {
        // 如果引用是null，则指令执行结束。也就是说，null 引用可以转换成任何类型
        return;
    }

    auto obj = static_cast<Jobject *>(r);
    const string &className = frame->method->jclass->rtcp->getClassName(index);

    Jclass *jclass = frame->method->jclass->loader->loadClass(className);// todo resolve_class ???
    if (!obj->isInstanceOf(jclass)) {
        // java.lang.ClassCastException
        jvmAbort("java.lang.ClassCastException. %s can not cast to %s\n",
                 obj->getClass()->className.c_str(), jclass->className.c_str());
    }
}


/*
 * 方法调用系列：
 * invokestatic指令用来调用静态方法。
 * invokespecial指令用来调用无须动态绑定的实例方法，包括构造函数、私有方法和通过super关键字调用的超类方法。
 * 剩下的情况则属于动态绑定。
 *     如果是针对接口类型的引用调用方法，就使用invokeinterface指令，
 *     否则使用invokevirtual指令。
 */
static void invokestatic(StackFrame *frame) {
    Jclass *currClass = frame->method->jclass;

    int index = frame->reader->readu2();
    MethodRef &ref = currClass->rtcp->getMethodRef(index);

    // todo 本地方法

    Jclass *c = resolveClass(currClass, ref.className);
    if (!c->isInited) {
        c->clinit(frame);
        // 将pc指向本条指令之前，初始化完类后，继续执行本条指令。
        frame->reader->setPc(frame->thread->pc);
        return;
    }

    Jmethod *method = resolveMethod(currClass, ref, true);
    if (method->isAbstract()) {
        // todo java.lang.AbstractMethodError
        jvmAbort("java.lang.AbstractMethodError\n");
    }

    frame->invokeMethod(method);
}

static void invokespecial(StackFrame *frame) {
    Jclass *currClass = frame->method->jclass;

    int index = frame->reader->readu2();
    MethodRef &ref = currClass->rtcp->getMethodRef(index);

    Jclass *c = resolveClass(currClass, ref.className);
    if (!c->isInited) {
        c->clinit(frame);
        // 将pc指向本条指令之前，初始化完类后，继续执行本条指令。
        frame->reader->setPc(frame->thread->pc);
        return;
    }

    Jmethod *method = resolveMethod(currClass, ref, false);

    // 假定从方法符号引用中解析出来的类是C，方法是M。如果M是构造函数，则声明M的类必须是C，
    if (method->name == "<init>" && method->jclass != c) {
        // todo java.lang.NoSuchMethodError
        jvmAbort("java.lang.NoSuchMethodError\n");
    }

    /*
     * 如果调用的中超类中的函数，但不是构造函数，且当前类的ACC_SUPER标志被设置，
     * 需要一个额外的过程查找最终要调用的方法；否则前面从方法符号引用中解析出来的方法就是要调用的方法。
     * todo 详细说明
     */
    if (currClass->isSuper() && currClass->isSubclassOf(c) && c->className != "<init>") {
        auto tmp = currClass->superClass->lookupMethod(ref.nameAndType.name, ref.nameAndType.descriptor);
        if (tmp != nullptr) {
            method = tmp;
        }
    }

    if (method->isAbstract()) {
        // todo java.lang.AbstractMethodError
        jvmAbort("java.lang.AbstractMethodError\n");
    }

    if (method->getArgSlotCount() < 1) {
        jvmAbort("error\n");
    }

    Slot args[method->getArgSlotCount()];
    for (int i = method->getArgSlotCount() - 1; i >= 0; i--) {
        args[i] = frame->operandStack.popSlot();
    }

    jreference r = args[0].getReference();
    if (r == nullptr) {
        // todo  java.lang.NullPointerException
        jvmAbort("java.lang.NullPointerException\n");
    }
    Jobject *obj = static_cast<Jobject *>(r);
    // todo 返回的指针是不是jobject *   ？？？？？？？？

    frame->invokeMethod(method, args);
}

static void invokevirtual(StackFrame *frame) {
    Jclass *currClass = frame->method->jclass;

    int index = frame->reader->readu2();
    MethodRef &ref = currClass->rtcp->getMethodRef(index);

    Jclass *c = resolveClass(currClass, ref.className);
    if (!c->isInited) {
        c->clinit(frame);
        // 将pc指向本条指令之前，初始化完类后，继续执行本条指令。
        frame->reader->setPc(frame->thread->pc);
        return;
    }

    Jmethod *method = resolveMethod(currClass, ref, false);
    if (method->isStatic()) {
        // todo  java.lang.IncompatibleClassChangeError
        jvmAbort("java.lang.IncompatibleClassChangeError\n");
    }

    if (method->getArgSlotCount() < 1) {
        jvmAbort("error\n");
    }

    Slot args[method->getArgSlotCount()];
    for (int i = method->getArgSlotCount() - 1; i >= 0; i--) {
        args[i] = frame->operandStack.popSlot();
    }

    jreference r = args[0].getReference();
    if (r == nullptr) {
        if (method->name == "println") {
            // todo   println  暂时先这么搞
            JstringObj *aaa = static_cast<JstringObj *>(args[1].getReference());
            jstring sss = aaa->value();
            jprintf("%s\n", jstrToStr(sss).c_str());

            return;
        }
        // todo  java.lang.NullPointerException
        jvmAbort("java.lang.NullPointerException         %s\n", method->toString().c_str());
    }
    Jobject *obj = static_cast<Jobject *>(r);
    // todo 返回的指针是不是jobject *   ？？？？？？？？
//    char *s = obj;
//    if (obj->getClass() != c) {
//        jprintf("error, %s, %s\n", obj->getClass()->toString().c_str(), c->toString().c_str());
//    }

//    jprintf("RRRRRRRRRRRRRRRRRR, %s, %s\n", obj->getClass()->className.c_str(), ref.className.c_str());

    method = obj->getClass()->lookupMethod(ref.nameAndType.name, ref.nameAndType.descriptor);
    if (method == nullptr) {
        jvmAbort("Can't find method: %s::%s:%s\n",
                 ref.className.c_str(), ref.nameAndType.name.c_str(), ref.nameAndType.descriptor.c_str()); // todo
    }

    if (method->isAbstract()) {
        // todo java.lang.AbstractMethodError
        jvmAbort("java.lang.AbstractMethodError\n");
    }

    frame->invokeMethod(method, args);
}

static void invokeinterface(StackFrame *frame) {
    Jclass *currClass = frame->method->jclass;

    int index = frame->reader->readu2();
    frame->reader->readu1();
    frame->reader->readu1();

    MethodRef &ref = currClass->rtcp->getMethodRef(index);
    Jclass *c = resolveClass(currClass, ref.className);
    if (!c->isInited) {
        c->clinit(frame);
        // 将pc指向本条指令之前，初始化完类后，继续执行本条指令。
        frame->reader->setPc(frame->thread->pc);
        return;
    }

    Jmethod *method = resolveMethod(currClass, ref, false);
//    if (method->isStatic()) {
//        // todo  java.lang.IncompatibleClassChangeError
//        jvmAbort("java.lang.IncompatibleClassChangeError\n");
//    }

    if (method->getArgSlotCount() < 1) {
        jvmAbort("error\n");
    }

    Slot args[method->getArgSlotCount()];
    for (int i = method->getArgSlotCount() - 1; i >= 0; i--) {
        args[i] = frame->operandStack.popSlot();
    }

    jreference r = args[0].getReference();
    if (r == nullptr) {
        // todo  java.lang.NullPointerException
        jvmAbort("java.lang.NullPointerException\n");
    }
    auto obj = static_cast<Jobject *>(r);
    // todo 返回的指针是不是jobject *   ？？？？？？？？

    method = obj->getClass()->lookupMethod(ref.nameAndType.name, ref.nameAndType.descriptor);
    if (method == nullptr) {
        jvmAbort("error\n"); // todo
    }

    if (method->isAbstract()) {
        // todo java.lang.AbstractMethodError
        jvmAbort("java.lang.AbstractMethodError\n");
    }
    if (!method->isPublic()) {
        // todo java.lang.IllegalAccessError
        jvmAbort("java.lang.IllegalAccessError\n");
    }

    frame->invokeMethod(method, args);
}


static void newarray(StackFrame *frame) {
    int arrLen = frame->operandStack.popInt();
    if (arrLen < 0) {
        // todo  java.lang.NegativeArraySizeException
        jvmAbort("error. java.lang.NegativeArraySizeException. %d \n", arrLen);
        return;
    }

    // todo arrLen == 0 的情况

    /*
        AT_BOOLEAN = 4
        AT_CHAR = 5
        AT_FLOAT = 6
        AT_DOUBLE = 7
        AT_BYTE = 8
        AT_SHORT = 9
        AT_INT = 10
        AT_LONG = 11
     */
    int arrType = frame->reader->readu1();
    string arrName;
    switch (arrType) {
        case 4: arrName = "[Z"; break;
        case 5: arrName = "[C"; break;
        case 6: arrName = "[F"; break;
        case 7: arrName = "[D"; break;
        case 8: arrName = "[B"; break;
        case 9: arrName = "[S"; break;
        case 10: arrName = "[I"; break;
        case 11: arrName = "[J"; break;
        default: arrName = ""; break;
    }

    if (arrName.empty()) {
        // todo
        jvmAbort("error. Invalid array type: %d.\n", arrType);
        return;
    }

    Jclass *c = frame->method->jclass->loader->loadClass(arrName);
    frame->operandStack.push(new JarrayObj(c, arrLen));
}

static void anewarray(StackFrame *frame) {
    int arrLen = frame->operandStack.popInt();
    if (arrLen < 0) {
        // todo  java.lang.NegativeArraySizeException
        jvmAbort("error. java.lang.NegativeArraySizeException \n");
        return;
    }

    // todo arrLen == 0 的情况

    int index = frame->reader->readu2();
    const string &className = frame->method->jclass->rtcp->getClassName(index);
    Jclass *componentClass = resolveClass(frame->method->jclass, className);
    Jclass *arrClass = componentClass->arrayClass();
    frame->operandStack.push(new JarrayObj(arrClass, arrLen));
}

static void arraylength(StackFrame *frame) {
    jreference r = frame->operandStack.popReference();
    if (r == nullptr) {
        // todo java.lang.NullPointerException
        jvmAbort("error. java.lang.NullPointerException\n");
    }
    JarrayObj *arr = static_cast<JarrayObj *>(r);


    // todo 多维数组也可以？？？？？
//    if (!is_one_dimension_array(arr->class)) {
//        jprintf("error.\n"); // todo 一维数组?
//    }


    frame->operandStack.push(arr->length());
}

static void monitorenter(StackFrame *frame) {
    Jobject *o = static_cast<Jobject *>(frame->operandStack.popReference());
    // todo
}

static void monitorexit(StackFrame *frame) {
    Jobject *o = static_cast<Jobject *>(frame->operandStack.popReference());
    // todo
}

#endif
