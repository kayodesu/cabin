#include <assert.h>
#include "cabin.h"
#include "heap.h"
#include "cabin.h"

/*
 * 分析一个可达的对象
 */
static void analysis_reachable_object(jref obj)
{
    assert (obj != NULL);

    if (is_ref_array_class(obj->clazz)) {
        jarrRef arr = obj;
        assert(arr != NULL);
        for (int i = 0; i < arr->arr_len; i++) {
            jref o = array_get(jref, arr, i);
            if (o != NULL) {
                o->accessible = 1; // 此对象可达
                analysis_reachable_object(o);
            }
        }
    } else {
        int count = obj->clazz->inst_fields_count;
        for (int i = 0; i < count; i++) {
            slot_t *slot = obj->data + i;
            // todo  这里要先判断 slot 存放的是不是 jref
            jref o = slot_get_ref(slot);
            o->accessible = 1;
            analysis_reachable_object(o);
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
static void reachability_analysis()
{
#if 0    
    /****** 虚拟机栈(栈桢中的本地变量表)中的引用的对象 ******/
    for (int i = 0; i < g_all_threads_count; i++) {
        for (Frame *frame = g_all_threads[i]->top_frame; frame != NULL; frame = frame->prev) {
            slot_t *lvars = frame->lvars;
            u2 max_locals = frame->method->max_locals;
            for (u2 j = 0; j < max_locals; j++) {
                // todo 这里要先判断 locals[j] 存放的是不是 jref
//                jref o = RSLOT(lvars + j);
//                o->accessible = 1;
//                analysisObject(o);
            }

            // todo How about frame->ostack？？？？
        }
    }

    /****** 类静态属性引用的对象 和 类对象中引用的对象 ******/
    const unordered_set<const Object *> &loaders = getAllClassLoaders();
    for (const Object *loader: loaders) {
        unordered_map<const utf8_t *, Class *, Utf8Hash, Utf8Comparator> *classes;
        if (loader == BOOT_CLASS_LOADER) {
            classes = getAllBootClasses();
        } else {
            classes = loader->classes;
        }

        assert(classes != NULL);
        for (auto &x: *(classes)) {
            Class *c = x.second;

            // 1. 分析类静态属性引用的对象

            // for (Field *f: c->fields) {
            //     if (!f->isStatic() || f->isPrim())
            //         continue;

            //     // static and reference field of a class
            //     jref o = f->static_value.r;
            //     o->accessible = 1;
            //     analysisReachableObject(o);
            // }
            for (u2 i = 0; i < c->fields_count; i++) {
                Field *f = c->fields + i;
                if (!ACC_IS_STATIC(f->access_flags) || is_prim_field(f))
                    continue;

                // static and reference field of a class
                jref o = f->static_value.r;
                o->accessible = 1;
                analysisReachableObject(o);
            }

            // 2. 分析类对象中引用的对象
            ClsObj *co = c->java_mirror;
            assert(co->accessible = 1);
            analysis_reachable_object(co);
        }
    }

    // todo ..... c,d 中的判断
#endif    
}

void gc()
{
    assert(g_heap != NULL);
    lock_heap(g_heap);

    // todo 是不是要先将全部对象视为不可达？？？？ 然后再判断，可达的予以标记？？？？

    address mem = g_heap->mem;
    const address end = mem + g_heap->size;

    while (mem < end) {
        mem = jump_freelist(g_heap, mem);

        Object *obj = (Object *) mem;
        obj->accessible = 0;
//        if (!accessible(obj)) {
//            obj->marked = 1;
//            // todo 调用 finalize() 后进行二次标记，然后才可以归还
//            heap_free(g_heap, mem, obj->size());
//        }
        mem += object_size(obj);
    }

    reachability_analysis();

    unlock_heap(g_heap);
}
