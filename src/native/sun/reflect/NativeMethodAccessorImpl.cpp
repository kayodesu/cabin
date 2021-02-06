#include <cassert>
#include "../../jni_internal.h"
#include "../../../cabin.h"
#include "../../../objects/object.h"
#include "../../../objects/array.h"
#include "../../../metadata/class.h"
#include "../../../objects/prims.h"
#include "../../../interpreter/interpreter.h"
#include "../../../metadata/descriptor.h"

using namespace std;
using namespace slot;

// private static native Object invoke0(Method method, Object o, Object[] os);
static jobject invoke0(jobject method, jobject o, jobjectArray os)
{
    assert(method != nullptr);
    // If method is static, o is nullptr.

    // private Class<?>   clazz;
    // private String     name;
    // private Class<?>   returnType;
    // private Class<?>[] parameterTypes;
    Class *c = method->getRefField<ClsObj>(S(clazz), S(sig_java_lang_Class))->jvm_mirror;
    jstrref name = method->getRefField(S(name), S(sig_java_lang_String));
    auto rtype = method->getRefField<ClsObj>(S(returnType), S(sig_java_lang_Class));
    auto ptypes = method->getRefField<Array>(S(parameterTypes), S(array_java_lang_Class));

    string desc = unparseMethodDescriptor(ptypes, rtype);
    Method *m = c->lookupMethod(name->toUtf8(), desc.c_str());
    if (m == nullptr) {
        JVM_PANIC("error"); // todo
    }

    slot_t *result = execJavaFunc(m, o, os);
    switch (m->ret_type) {
    case Method::RET_VOID:
        return voidBox();
    case Method::RET_BYTE:
//        return byteBox(jint2jbyte(ISLOT(result)));
        return byteBox(getByte(result));
    case Method::RET_BOOL:
//        return boolBox(jint2jbool(ISLOT(result)));
        return boolBox(getBool(result));
    case Method::RET_CHAR:
//        return charBox(jint2jchar(ISLOT(result)));
        return charBox(getChar(result));
    case Method::RET_SHORT:
//        return shortBox(jint2jshort(ISLOT(result)));
        return shortBox(getShort(result));
    case Method::RET_INT:
        return intBox(getInt(result));
    case Method::RET_FLOAT:
        return floatBox(getFloat(result));
    case Method::RET_LONG:
        return longBox(getLong(result));
    case Method::RET_DOUBLE:
        return doubleBox(getDouble(result));
    case Method::RET_REFERENCE:
        return getRef(result);
    default:
        JVM_PANIC("never go here\n"); // todo
    }
}

static JNINativeMethod methods[] = {
        JNINativeMethod_registerNatives,
        { "invoke0", "(Ljava/lang/reflect/Method;" OBJ "[" OBJ_ OBJ, (void *) invoke0 },
};

void sun_reflect_NativeMethodAccessorImpl_registerNatives()
{
    registerNatives("sun/reflect/NativeMethodAccessorImpl", methods, ARRAY_LENGTH(methods));
}
