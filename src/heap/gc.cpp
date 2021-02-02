#include <vector>
#include "gc.h"
#include "../cabin.h"
#include "heap.h"
#include "../runtime/vm_thread.h"
#include "../runtime/frame.h"
#include "../objects/class_loader.h"
#include "../objects/object.h"
#include "../objects/array.h"
#include "../metadata/class.h"
#include "../util/encoding.h"

using namespace std;

/*
 * 分析一个可达的对象
 */
static void analysisReachableObject(jref obj)
{
    assert (obj != nullptr);
    assert (obj->accessible = 1);

    if (obj->clazz->isRefArrayClass()) {
        Array *arr = dynamic_cast<Array *>(obj);
        assert(arr != nullptr);
        for (int i = 0; i < arr->arr_len; i++) {
            jref o = arr->get<jref>(i);
            if (o != nullptr) {
                o->accessible = 1; // 此对象可达
                analysisReachableObject(o);
            }
        }
    } else {
        int count = obj->clazz->inst_fields_count;
        for (int i = 0; i < count; i++) {
            slot_t slot = obj->data[i];
            // todo  这里要先判断 slot 存放的是不是 jref
            jref o = slot::getRef(&slot);
            o->accessible = 1;
            analysisReachableObject(o);
        }
    }
}

/*
 * 判断对象是否可达(GC Roots Analysis)
 *
 * 可作为GC Roots对象的包括如下几种：
    a.虚拟机栈(栈桢中的本地变量表)中的引用的对象  todo 操作栈中的引用的对象呢？？？？？
    b.方法区中的类静态属性引用的对象
    c.方法区中的常量引用的对象
    d.本地方法栈中JNI的引用的对象
    e.ClassObject对象（保存在本地内存）中所引用的对象
 */
static void reachabilityAnalysis()
{
    /****** 虚拟机栈(栈桢中的本地变量表)中的引用的对象 ******/
    for (Thread *thread : g_all_threads) {
        for (Frame *frame = thread->getTopFrame(); frame != nullptr; frame = frame->prev) {
            slot_t *lvars = frame->lvars;
            u2 max_locals = frame->method->max_locals;
            for (u2 i = 0; i < max_locals; i++) {
                // todo 这里要先判断 locals[i] 存放的是不是 jref
//                jref o = RSLOT(lvars + i);
//                o->accessible = 1;
//                analysisObject(o);
            }

            // todo How about frame->ostack？？？？
        }
    }

    /****** 类静态属性引用的对象 和 类对象中引用的对象 ******/
    const unordered_set<const Object *> &loaders = getAllClassLoaders();
    for (const Object *loader: loaders) {
        unordered_map<const utf8_t *, Class *, utf8::Hash, utf8::Comparator> *classes;
        if (loader == BOOT_CLASS_LOADER) {
            classes = getAllBootClasses();
        } else {
            classes = loader->classes;
        }

        assert(classes != nullptr);
        for (auto &x: *(classes)) {
            Class *c = x.second;

            // 1. 分析类静态属性引用的对象
            for (Field *f: c->fields) {
                if (!f->isStatic() || f->isPrim())
                    continue;

                // static and reference field of a class
                jref o = f->static_value.r;
                o->accessible = 1;
                analysisReachableObject(o);
            }

            // 2. 分析类对象中引用的对象
            ClsObj *co = c->java_mirror;
            assert(co->accessible = 1);
            analysisReachableObject(co);
        }
    }

    // todo ..... c,d 中的判断
}

void gc()
{
    assert(g_heap != nullptr);
    g_heap->lock();

    // todo 是不是要先将全部对象视为不可达？？？？ 然后再判断，可达的予以标记？？？？

    address mem = g_heap->mem;
    const address end = mem + g_heap->size;

    while (mem < end) {
        mem = g_heap->jumpFreelist(mem);

        auto obj = (Object *) mem;
        obj->accessible = 0;
//        if (!accessible(obj)) {
//            obj->marked = 1;
//            // todo 调用 finalize() 后进行二次标记，然后才可以归还
//            g_heap->back(mem, obj->size());
//        }
        mem += obj->size();
    }

    reachabilityAnalysis();

    g_heap->unlock();
}
