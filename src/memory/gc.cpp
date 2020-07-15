/*
 * Author: Yo Ka
 */

#include <vector>
#include "gc.h"
#include "../jvmstd.h"
#include "heap.h"
#include "../runtime/vm_thread.h"
#include "../runtime/frame.h"
#include "../objects/object.h"
#include "../metadata/class.h"

using namespace std;

/*
 * 判断对象是否可达(GC Roots Analysis)
 *
 * 可作为GC Roots对象的包括如下几种：
    a.虚拟机栈(栈桢中的本地变量表)中的引用的对象
    b.方法区中的类静态属性引用的对象
    c.方法区中的常量引用的对象
    d.本地方法栈中JNI的引用的对象
 */
static bool accessible(Object *obj)
{
    // 虚拟机栈(栈桢中的本地变量表)中的引用的对象
//    for (Thread *thread : g_all_threads) {
//        for (Frame *frame = thread->getTopFrame(); frame != nullptr; frame = frame->prev) {
//            slot_t *lvars = frame->lvars;
//            u2 maxLocals = frame->method->maxLocals;
//            for (u2 i = 0; i < maxLocals; i++) {
//                // todo 这里要先判断 locals[i] 存放的是不是 jref
//                if (RSLOT(lvars + i) == obj)
//                    return true;
//            }
//        }
//    }

    // todo 得到方法区的所有类，遍历
    jvm_abort("未实现");
//    MethodArea ma; // todo
//    vector<Class *> classes = ma.getClasses();
//    for (Class *c : classes) {
//        for (Field *f : c->fields) {
//            if (!f->isStatic() or f->isPrim())
//                continue;
//
//            // static and reference
//            if (f->staticValue.r == obj)
//                return true;
//        }
//    }

    // todo 方法区中的常量引用的对象


    // todo 本地方法栈中JNI的引用的对象

    return true;
}

void gc()
{
    assert(g_heap != nullptr);
    g_heap->lock();

    address mem = g_heap->mem;
    const address end = mem + g_heap->size;

    while (mem < end) {
        mem = g_heap->jumpFreelist(mem);

        auto obj = (Object *) mem;
        if (!accessible(obj)) {
            obj->marked = 1;
            // todo 调用 finalize() 后进行二次标记，然后才可以归还
            g_heap->back(mem, obj->size());
        }
        mem += obj->size();
    }

    g_heap->unlock();
}
