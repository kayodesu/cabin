/*
 * Author: Jia Yang
 */

#include <sstream>
#include "../../registry.h"
#include "../../../runtime/Frame.h"
#include "../../../objects/Object.h"
#include "../../../objects/Method.h"
#include "../../../objects/Array.h"
#include "../../../interpreter/interpreter.h"
#include "../../../symbol.h"

using namespace std;

//static slot_t* convert_args(jref this_obj, Method *m, Array *args)
//{
//    Array *types = m->getParameterTypes();
//    int types_len = types->len;
//    assert(types_len == args->len);
//
//    // 因为有 category two 的存在，result 的长度最大为 types_len * 2 + this_obj
//    auto result = new slot_t[2 * types_len + 1];
//    int k = 0;
//    if (this_obj != nullptr) {
//        result[k++] = (slot_t) this_obj;
//    }
//
//    for (int i = 0; i < types_len; i++) {
//        auto clsobj = types->get<Class *>(i);
//        assert(clsobj != nullptr);
//        auto o = args->get<jref>(i);
//
//        if (clsobj->isPrimClass()) {
//            const slot_t *unbox = o->unbox();
//            result[k++] = *unbox;
//            if (strcmp(o->clazz->className, "long") == 0 || strcmp(o->clazz->className, "double") == 0) // category_two
//                result[k++] = *++unbox;
//        } else {
//            RSLOT(result + k) = o;
//            k++;
//        }
//    }
//
//    return result;
//}

//static string typesToDescriptor(const Array *types)
//{
//    assert(types != nullptr);
//    assert(types->isArrayObject());
//
//    ostringstream os;
//
//    for (int t = 0; t < types->len; t++) {
//        auto type = types->get<Class *>(t);
//        char d = primClassName2descriptor(type->className);
//        if (d != 0) { // primitive type
//            os << d;
//        } else if (type->isArrayObject()) { // 数组
//            os << type->className;
//        } else { // 普通类
//            os << 'L' << type->className << ';';
//        }
//    }
//
//    return os.str();
//}

/*
 * private static native Object newInstance0(Constructor<?> c, Object[] os)
 * throws InstantiationException, IllegalArgumentException, InvocationTargetException;
 */
static void newInstance0(Frame *frame)
{
    jref co = frame->getLocalAsRef(0);
    /*
     * args array of objects to be passed as arguments to
     * the constructor call; values of primitive types are wrapped in
     * a wrapper Object of the appropriate type
     */
    auto args = frame->getLocalAsRef<Array>(1); // may be NULL

    // which class this constructor belongs to.
    auto ac = co->getInstFieldValue<Class *>(S(clazz), S(sig_java_lang_Class));
    initClass(co->clazz);
    Object *obj = newObject(ac);

    if (args == nullptr) { // 构造函数没有参数
        Method * constructor = ac->getConstructor("()V");
        assert(constructor->arg_slot_count == 1); // this
        execJavaFunc(constructor, obj);
    } else {
        // parameter types of this constructor
        auto parameterTypes = co->getInstFieldValue<Array *>(S(parameterTypes), S(array_java_lang_Class));
        Method * constructor = ac->getConstructor(parameterTypes);
        execConstructor(constructor, obj, args);
    }

    frame->pushr(obj);
}

void sun_reflect_NativeConstructorAccessorImpl_registerNatives()
{
#undef C
#define C "sun/reflect/NativeConstructorAccessorImpl"
    registerNative(C, "newInstance0",
                   "(Ljava/lang/reflect/Constructor;[Ljava/lang/Object;)Ljava/lang/Object;", newInstance0);
}
