/*
 * Author: Jia Yang
 */

#include "../../registry.h"
#include "../../../rtda/thread/frame.h"
#include "../../../rtda/heap/object.h"
#include "../../../rtda/ma/descriptor.h"
#include "../../../rtda/heap/arrobj.h"
#include "../../../rtda/heap/clsobj.h"

struct slot* convert_args(jref this_obj, struct method *m, jref args)
{
    struct object *types = jmethod_get_parameter_types(m);
    int types_len = arrobj_len(types);
    assert(types_len == arrobj_len(args));

    // 因为有 category two 的存在，result 的长度最大为 types_len * 2 + this_obj
    VM_MALLOCS(struct slot, types_len * 2 + this_obj != NULL ? 1 : 0, result);
    int k = 0;
    if (this_obj != NULL) {
        result[k++] = rslot(this_obj);
    }

    for (int i = 0; i < types_len; i++) {
        jref clsobj = arrobj_get(jref, types, i);
        assert(clsobj != NULL);
        jref o = arrobj_get(jref, args, i);

        if (jclass_is_primitive(clsobj->u.entity_class)) {
            result[k] = priobj_unbox(o);
            if (slot_is_category_two(result + k)) {
                result[++k] = phslot;
            }
            k++;
        } else {
            result[k++] = rslot(o);
        }
    }

    return result;
}

/*
 * private static native Object newInstance0(Constructor<?> c, Object[] os)
 * throws InstantiationException, IllegalArgumentException, InvocationTargetException;
 */
static void newInstance0(struct frame *frame)
{
    jref constructor_obj = frame_locals_getr(frame, 0);
    /*
     * init_args array of objects to be passed as arguments to
     * the constructor call; values of primitive types are wrapped in
     * a wrapper object of the appropriate type
     */
    jref init_args = frame_locals_getr(frame, 1); // may be NULL

    // which class this constructor belongs to.
    struct class *clazz
            =slot_getr(get_instance_field_value_by_nt(constructor_obj, "clazz", "Ljava/lang/Class;"))->u.entity_class;
    struct object *this_obj = object_create(clazz);
    frame_stack_pushr(frame, this_obj); // return value

    struct method *constructor = NULL;
    if (init_args == NULL) { // 构造函数没有参数
        constructor = class_get_constructor(clazz, "()V");
        assert(constructor != NULL);
        assert(constructor->arg_slot_count == 1); // this
        struct slot s = rslot(this_obj);
        jthread_invoke_method(frame->thread, constructor, &s);
    } else {
        // parameter types of this constructor
        jref parameter_types
                = slot_getr(get_instance_field_value_by_nt(constructor_obj, "parameterTypes", "[Ljava/lang/Class;"));
        constructor = class_get_constructor(clazz, types_to_descriptor(parameter_types));
        assert(constructor != NULL);

        struct slot *args = convert_args(this_obj, constructor, init_args);
        jthread_invoke_method(frame->thread, constructor, args);
        free(args);
    }

    if (!constructor->clazz->inited) {
        // todo java.lang.reflect/Constructor 的 clinit
        class_clinit(constructor_obj->clazz, frame->thread);
    }
}

void sun_reflect_NativeConstructorAccessorImpl_registerNatives()
{
    register_native_method("sun/reflect/NativeConstructorAccessorImpl~registerNatives~()V", registerNatives);
    register_native_method("sun/reflect/NativeConstructorAccessorImpl~newInstance0~"
                                   "(Ljava/lang/reflect/Constructor;[Ljava/lang/Object;)Ljava/lang/Object;",
                           newInstance0);
}
