/*
 * Author: Jia Yang
 */

#include "../../registry.h"
#include "../../../interpreter/stack_frame.h"
#include "../../../slot.h"
#include "../../../rtda/heap/jobject.h"
#include "../../../rtda/ma/util.h"

struct slot* convert_args(jref this_obj, struct jmethod *m, jref args)
{
    struct jobject *types = jmethod_get_parameter_types(m);
    int types_len = jarrobj_len(types);
    int args_len = jarrobj_len(args);
    assert(types_len == args_len);

    // 因为有 category two 的存在，result 的长度最大为 types_len * 2 + this_obj
    VM_MALLOCS(struct slot, types_len * 2 + this_obj != NULL ? 1 : 0, result);
    int k = 0;
    if (this_obj != NULL) {
        result[k++] = rslot(this_obj);
    }

    for (int i = 0; i < types_len; i++) {
        jref clsobj = jarrobj_get(jref, types, i);
        assert(clsobj != NULL);
        jref o = jarrobj_get(jref, args, i);

        if (jclass_is_primitive(jclsobj_entity_class(clsobj))) {
            result[k] = jpriobj_unbox(o);
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
static void newInstance0(struct stack_frame *frame)
{
    jref constructor_obj = slot_getr(frame->local_vars);
    /*
     * init_args array of objects to be passed as arguments to
     * the constructor call; values of primitive types are wrapped in
     * a wrapper object of the appropriate type
     */
    jref init_args = slot_getr(frame->local_vars + 1); // may be NULL

    // which class this constructor belongs to.
    struct jclass *clazz = jclsobj_entity_class(
            slot_getr(get_instance_field_value_by_nt(constructor_obj, "clazz", "Ljava/lang/Class;")));
    struct jobject *this_obj = jobject_create(clazz);
    os_pushr(frame->operand_stack, this_obj); // return value

    struct jmethod *constructor = NULL;
    if (init_args == NULL) { // 构造函数没有参数
        constructor = jclass_get_constructor(clazz, "()V");
        assert(constructor != NULL);
        assert(constructor->arg_slot_count == 1); // this
        struct slot s = rslot(this_obj);
        jthread_invoke_method(frame->thread, constructor, &s);
    } else {
        // parameter types of this constructor
        jref parameter_types
                = slot_getr(get_instance_field_value_by_nt(constructor_obj, "parameterTypes", "[Ljava/lang/Class;"));
        constructor = jclass_get_constructor(clazz, types_to_descriptor(parameter_types));
        assert(constructor != NULL);

        struct slot *args = convert_args(this_obj, constructor, init_args);
        jthread_invoke_method(frame->thread, constructor, args);
        free(args);
    }

    if (!constructor->jclass->inited) {
        // todo java.lang.reflect/Constructor 的 clinit
        jclass_clinit0(constructor_obj->jclass, frame->thread);
    }
}

void sun_reflect_NativeConstructorAccessorImpl_registerNatives()
{
    register_native_method("sun/reflect/NativeConstructorAccessorImpl", "registerNatives", "()V", empty_method);
    register_native_method("sun/reflect/NativeConstructorAccessorImpl", "newInstance0",
                           "(Ljava/lang/reflect/Constructor;[Ljava/lang/Object;)Ljava/lang/Object;", newInstance0);
}
