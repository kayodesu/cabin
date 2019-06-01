/*
 * Author: Jia Yang
 */

#include "../../registry.h"
#include "../../../rtda/thread/Frame.h"
#include "../../../rtda/heap/Object.h"
#include "../../../rtda/ma/descriptor.h"
#include "../../../rtda/ma/Method.h"
#include "../../../rtda/heap/ArrayObject.h"
#include "../../../rtda/heap/ClassObject.h"
#include "../../../interpreter/interpreter.h"
#include "../../../symbol.h"

static slot_t* convert_args(jref this_obj, Method *m, ArrayObject *args)
{
    ArrayObject *types = m->getParameterTypes();
    int types_len = types->len;
    assert(types_len == args->len);

    // 因为有 category two 的存在，result 的长度最大为 types_len * 2 + this_obj
    auto result = new slot_t[2 * types_len + 1];//vm_malloc(sizeof(slot_t) * 2 * types_len + 1);
    int k = 0;
    if (this_obj != nullptr) {
        result[k++] = (slot_t) &this_obj;
    }

    for (int i = 0; i < types_len; i++) {
        auto clsobj = types->get<ClassObject *>(i);//arrobj_get(jref, types, i);
        assert(clsobj != nullptr);
        auto o = args->get<jref>(i);//arrobj_get(jref, args, i);

        if (clsobj->entityClass->isPrimitive()) {
            const slot_t *unbox = o->unbox();
            result[k++] = *unbox;
            if (strcmp(o->clazz->className, "long") == 0 || strcmp(o->clazz->className, "double") == 0) // category_two
                result[k++] = *++unbox;
        } else {
            RSLOT(result + k) = o;
            k++;
        }
    }

    return result;
}

/*
 * private static native Object newInstance0(Constructor<?> c, Object[] os)
 * throws InstantiationException, IllegalArgumentException, InvocationTargetException;
 */
static void newInstance0(Frame *frame)
{
    jref constructor_obj = frame_locals_getr(frame, 0);
    /*
     * init_args array of objects to be passed as arguments to
     * the constructor call; values of primitive types are wrapped in
     * a wrapper Object of the appropriate type
     */
    ArrayObject *init_args = (ArrayObject *) frame_locals_getr(frame, 1); // may be NULL

    // which class this constructor belongs to.
    auto ac = constructor_obj->getInstFieldValue<ClassObject *>(S(clazz), S(sig_java_lang_Class));
    Class *clazz = ac->entityClass;
    Object *this_obj = Object::newInst(clazz);
    frame_stack_pushr(frame, this_obj); // return value

    Method *constructor = nullptr;
    if (init_args == nullptr) { // 构造函数没有参数
        constructor = clazz->getConstructor("()V");
        assert(constructor != nullptr);
        assert(constructor->arg_slot_count == 1); // this

        if (!constructor->clazz->inited) {
            // todo java.lang.reflect/Constructor 的 clinit
            constructor_obj->clazz->clinit();
        }

        exec_java_func(constructor, (slot_t *) &this_obj);
    } else {
        // parameter types of this constructor
        auto parameter_types
                =  constructor_obj->getInstFieldValue<ArrayObject *>(S(parameterTypes), S(array_java_lang_Class));
        constructor = clazz->getConstructor(typesToDescriptor(parameter_types).c_str());
        assert(constructor != nullptr);

        if (!constructor->clazz->inited) {
            // todo java.lang.reflect/Constructor 的 clinit
            constructor_obj->clazz->clinit();
        }

        // todo 可否直接传递 init_args 数组的 data域？？？？？？？？？？？？
        slot_t *args = convert_args(this_obj, constructor, init_args);
        exec_java_func(constructor, args);
        free(args);
    }
}

void sun_reflect_NativeConstructorAccessorImpl_registerNatives()
{
#undef C
#define C "sun/reflect/NativeConstructorAccessorImpl",
    register_native_method(C"newInstance0",
                         "(Ljava/lang/reflect/Constructor;[Ljava/lang/Object;)Ljava/lang/Object;", newInstance0);
}
