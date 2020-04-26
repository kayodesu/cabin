/*
 * Author: Yo Ka
 */

#include <vector>
#include "jni_inner.h"
#include "../objects/array_object.h"
#include "../interpreter/interpreter.h"
#include "../runtime/thread_info.h"
#include "../runtime/frame.h"

using namespace std;
using namespace utf8;

GlobalRefTable global_refs;
GlobalRefTable weak_global_refs;

jint JNICALL JVM_GetVersion(JNIEnv *env)
{
    assert(env != nullptr);
    // todo
    jvm_abort("not implement.");
}

jclass JNICALL JVM_DefineClass(JNIEnv *env,
                const char *name, jobject loader, const jbyte *buf, jsize len)
{
    assert(env != nullptr && name != nullptr && buf != nullptr && len >= 0);

    Class *c = defineClass(to_object_ref(loader), (u1 *) buf, len);
    if (c != nullptr)
        linkClass(c);
    return (jclass) addJNILocalRef(c); // todo
}

jclass JNICALL JVM_FindClass(JNIEnv *env, const char *name)
{
    assert(env != nullptr && name != nullptr);
    // todo
    jref loader = ((Frame *) env->functions->reserved3)->method->clazz->loader;
    Class *c = loadClass(loader, name);
    return to_jclass(c);
   // jvm_abort("not implement.");
}

jmethodID JNICALL JVM_FromReflectedMethod(JNIEnv *env, jobject method)
{
    assert(env != nullptr && method != nullptr);
    // todo
    jvm_abort("not implement.");
}

jfieldID JNICALL JVM_FromReflectedField(JNIEnv *env, jobject field)
{
    assert(env != nullptr && field != nullptr);
    // todo
    jvm_abort("not implement.");
}

jobject JNICALL JVM_ToReflectedMethod(JNIEnv *env, jclass cls, jmethodID methodID, jboolean isStatic)
{
    // todo
    jvm_abort("not implement.");
}

jclass JNICALL JVM_GetSuperclass(JNIEnv *env, jclass sub)
{
    assert(env != nullptr && sub != nullptr);
    auto c = to_object_ref<Class>(sub);
    return (jclass) addJNILocalRef(c->superClass);
}

jboolean JNICALL JVM_IsAssignableFrom(JNIEnv *env, jclass sub, jclass sup)
{
    // todo
    jvm_abort("not implement.");
}

jobject JNICALL JVM_ToReflectedField(JNIEnv *env, jclass cls, jfieldID fieldID, jboolean isStatic)
{
    // todo
    jvm_abort("not implement.");
}

jint JNICALL JVM_Throw(JNIEnv *env, jthrowable obj)
{
    // todo
    jvm_abort("not implement.");

//    throw Throwable(to_object_ref(obj));
    return JNI_TRUE;
}

jint JNICALL JVM_ThrowNew(JNIEnv *env, jclass clazz, const char *msg)
{
    // todo
    jvm_abort("not implement.");

//    throw Throwable(to_object_ref<Class>(clazz), msg);
    return JNI_TRUE;
}

jthrowable JNICALL JVM_ExceptionOccurred(JNIEnv *env)
{
    // todo
    jvm_abort("not implement.");
}

void JNICALL JVM_ExceptionDescribe(JNIEnv *env)
{
    // todo
    jvm_abort("not implement.");
}

void JNICALL JVM_ExceptionClear(JNIEnv *env)
{
    // todo
    jvm_abort("not implement.");
}

void JNICALL JVM_FatalError(JNIEnv *env, const char *msg)
{
    // todo
    jvm_abort("not implement.");
}

jint JNICALL JVM_PushLocalFrame(JNIEnv *env, jint capacity)
{
    // todo
    jvm_abort("not implement.");
}

jobject JNICALL JVM_PopLocalFrame(JNIEnv *env, jobject result)
{
    // todo
    jvm_abort("not implement.");
}

jobject JNICALL JVM_NewGlobalRef(JNIEnv *env, jobject gref)
{
    assert(env != nullptr && gref != nullptr);
    return addJNIGlobalRef(to_object_ref(gref));
}

void JNICALL JVM_DeleteGlobalRef(JNIEnv *env, jobject gref)
{
    assert(env != nullptr && gref != nullptr);
    auto o = to_object_ref(gref);
    if (o->jni_obj_ref_type == JNIGlobalRefType)
        deleteJNIGlobalRef(o);
}

void JNICALL JVM_DeleteLocalRef(JNIEnv *env, jobject obj)
{
    assert(env != nullptr && obj != nullptr);
    deleteJNILocalRef(to_object_ref(obj));
}

jboolean JNICALL JVM_IsSameObject(JNIEnv *env, jobject obj1, jobject obj2)
{
    // todo
    assert(env != nullptr && obj1 != nullptr && obj2 != nullptr);
    jvm_abort("not implement.");
}

jobject JNICALL JVM_NewLocalRef(JNIEnv *env, jobject obj)
{
    assert(env != nullptr && obj != nullptr);
    return addJNILocalRef(to_object_ref(obj));
}

jint JNICALL JVM_EnsureLocalCapacity(JNIEnv *env, jint capacity)
{
    // todo
    assert(env != nullptr);
    jvm_abort("not implement.");
}

jobject JNICALL JVM_AllocObject(JNIEnv *env, jclass clazz)
{
    assert(env != nullptr && clazz != nullptr);
    Class *c = checkClassBeforeAllocObject(to_object_ref<Class>(clazz));
    if (c == nullptr) {
        // todo
    }

    return addJNILocalRef(newObject(c));
}

jobject JNICALL JVM_NewObject(JNIEnv *env, jclass clazz, jmethodID methodID, ...)
{
    assert(env != nullptr && clazz != nullptr && methodID != nullptr);
    va_list args;
    va_start(args, methodID);
    jobject o = env->NewObjectV(clazz, methodID, args);
    va_end(args);
    return o;
}

jobject JNICALL JVM_NewObjectV(JNIEnv *env, jclass clazz, jmethodID methodID, va_list args)
{
    assert(env != nullptr && clazz != nullptr && methodID != nullptr);
    jobject jo = env->AllocObject(clazz);
    if (jo == nullptr) {
        // todo error
    }

    slot_t *ret = execConstructor(to_method(methodID), to_object_ref(jo), args);
    return to_jobject(RSLOT(ret));
}

jobject JNICALL JVM_NewObjectA(JNIEnv *env, jclass clazz, jmethodID methodID, const jvalue *args)
{
    assert(env != nullptr && clazz != nullptr && methodID != nullptr);
    jobject jo = env->AllocObject(clazz);
    if (jo == nullptr) {
        // todo error
    }

    slot_t *ret = execConstructor(to_method(methodID), to_object_ref(jo), args);
    return to_jobject(RSLOT(ret));
}

jclass JNICALL JVM_GetObjectClass(JNIEnv *env, jobject obj)
{
    assert(env != nullptr && obj != nullptr);
    Object *o = to_object_ref(obj);
    return (jclass) addJNILocalRef(o->clazz);
}

jboolean JNICALL JVM_IsInstanceOf(JNIEnv *env, jobject obj, jclass clazz)
{
    assert(env != nullptr && obj != nullptr && clazz != nullptr);
    auto o = to_object_ref(obj);
    auto c = to_object_ref<Class>(clazz);
    return o->isInstanceOf(c) ? JNI_TRUE : JNI_FALSE;
}

jmethodID JNICALL JVM_GetMethodID(JNIEnv *env, jclass clazz, const char *name, const char *sig)
{
    Class *c = to_object_ref<Class>(clazz);
    Method *m = c->lookupInstMethod(name, sig);
    return to_jmethodID(m);
}


//static jobject getJobjectRetValue(const slot_t *ret) { return addJNILocalRef(RSLOT(ret)); }
//
//template <typename RetType, RetType (*getRetValue)(const slot_t *) = nullptr>
//RetType JNICALL JVM_CallMethodA(JNIEnv *env, jobject obj, jmethodID methodID, const jvalue *args)
//{
//    jref _this = to_object_ref(obj);
//    Method *m0 = to_method(methodID);
//    Method *m = _this->clazz->lookupMethod(m0->name, m0->signature);
//    if (m == nullptr) {
//        /* todo error */
//    }
//    slot_t *ret = execJavaFunc(m, _this, args);
//    if (getRetValue != nullptr)
//        return getRetValue(ret);
//}
//
//template <typename RetType, RetType (*getRetValue)(const slot_t *) = nullptr>
//RetType JNICALL JVM_CallMethodV(JNIEnv *env, jobject obj, jmethodID methodID, va_list args)
//{
//    jvalue values[METHOD_PARAMETERS_MAX_COUNT];
//    parse_args_va_list(to_method(methodID)->signature, args, values);
//    if (getRetValue != nullptr)
//        return JVM_CallMethodA<RetType, getRetValue>(env, obj, methodID, values);
//    else
//        JVM_CallMethodA<RetType, getRetValue>(env, obj, methodID, values);
//}
//
//template <typename RetType, RetType (*getRetValue)(const slot_t *) = nullptr>
//RetType JNICALL JVM_CallMethod(JNIEnv *env, jobject obj, jmethodID methodID, ...)
//{
//    va_list args;
//    va_start(args, methodID);
//    auto o = JVM_CallMethodV<RetType, getRetValue>(env, obj, methodID, args);
//    va_end(args);
//    return o;
//}
//
//void JNICALL JVM_CallMethod(JNIEnv *env, jobject obj, jmethodID methodID, ...)
//{
//    va_list args;
//    va_start(args, methodID);
//    JVM_CallMethodV<void>(env, obj, methodID, args);
//    va_end(args);
//}

DEFINE_3_CALL_T_METHODS(Object, jobject, addJNILocalRef(RSLOT(ret)))
DEFINE_3_CALL_T_METHODS(Boolean, jboolean, jint2jbool(ISLOT(ret)))
DEFINE_3_CALL_T_METHODS(Byte, jbyte, jint2jbyte(ISLOT(ret)))
DEFINE_3_CALL_T_METHODS(Char, jchar, jint2jchar(ISLOT(ret)))
DEFINE_3_CALL_T_METHODS(Short, jshort, jint2jshort(ISLOT(ret)))
DEFINE_3_CALL_T_METHODS(Int, jint, ISLOT(ret))
DEFINE_3_CALL_T_METHODS(Long, jlong, LSLOT(ret))
DEFINE_3_CALL_T_METHODS(Float, jfloat, FSLOT(ret))
DEFINE_3_CALL_T_METHODS(Double, jdouble, DSLOT(ret))
DEFINE_3_CALL_T_METHODS(Void, void, )

DEFINE_3_CALL_NONVIRTUAL_T_METHODS(Object, jobject, addJNILocalRef(RSLOT(ret)))
DEFINE_3_CALL_NONVIRTUAL_T_METHODS(Boolean, jboolean, jint2jbool(ISLOT(ret)))
DEFINE_3_CALL_NONVIRTUAL_T_METHODS(Byte, jbyte, jint2jbyte(ISLOT(ret)))
DEFINE_3_CALL_NONVIRTUAL_T_METHODS(Char, jchar, jint2jchar(ISLOT(ret)))
DEFINE_3_CALL_NONVIRTUAL_T_METHODS(Short, jshort, jint2jshort(ISLOT(ret)))
DEFINE_3_CALL_NONVIRTUAL_T_METHODS(Int, jint, ISLOT(ret))
DEFINE_3_CALL_NONVIRTUAL_T_METHODS(Long, jlong, LSLOT(ret))
DEFINE_3_CALL_NONVIRTUAL_T_METHODS(Float, jfloat, FSLOT(ret))
DEFINE_3_CALL_NONVIRTUAL_T_METHODS(Double, jdouble, DSLOT(ret))
DEFINE_3_CALL_NONVIRTUAL_T_METHODS(Void, void, )


jfieldID JNICALL JVM_GetFieldID(JNIEnv *env, jclass clazz, const char *name, const char *sig)
{
    assert(env != nullptr && clazz != nullptr || name != nullptr || sig != nullptr);

    auto c = to_object_ref<Class>(clazz);
    initClass(c);
    Field *f = c->lookupField(name, sig);
    if (f == nullptr) {
        // todo java_lang_NoSuchFieldError
    }
    return to_jfieldID(f);
}

jobject JNICALL JVM_GetObjectField(JNIEnv *env, jobject obj, jfieldID fieldID)
{
    Object *o = to_object_ref(obj);
    Field *f = to_field(fieldID);
    return addJNILocalRef(o->getRefField(f));
}

jboolean JNICALL JVM_GetBooleanField(JNIEnv *env, jobject obj, jfieldID fieldID)
{
    return to_object_ref(obj)->getBoolField(to_field(fieldID));
}

jbyte JNICALL JVM_GetByteField(JNIEnv *env, jobject obj, jfieldID fieldID)
{
    return to_object_ref(obj)->getByteField(to_field(fieldID));
}

jchar JNICALL JVM_GetCharField(JNIEnv *env, jobject obj, jfieldID fieldID)
{
    return to_object_ref(obj)->getCharField(to_field(fieldID));
}

jshort JNICALL JVM_GetShortField(JNIEnv *env, jobject obj, jfieldID fieldID)
{
    return to_object_ref(obj)->getShortField(to_field(fieldID));
}

jint JNICALL JVM_GetIntField(JNIEnv *env, jobject obj, jfieldID fieldID)
{
    return to_object_ref(obj)->getIntField(to_field(fieldID));
}

jlong JNICALL JVM_GetLongField(JNIEnv *env, jobject obj, jfieldID fieldID)
{
    return to_object_ref(obj)->getLongField(to_field(fieldID));
}

jfloat JNICALL JVM_GetFloatField(JNIEnv *env, jobject obj, jfieldID fieldID)
{
    return to_object_ref(obj)->getFloatField(to_field(fieldID));
}

jdouble JNICALL JVM_GetDoubleField(JNIEnv *env, jobject obj, jfieldID fieldID)
{
    return to_object_ref(obj)->getDoubleField(to_field(fieldID));
}

void JNICALL JVM_SetObjectField(JNIEnv *env, jobject obj, jfieldID fieldID, jobject val)
{
    Object *o = to_object_ref(obj);
    Field *f = to_field(fieldID);
    o->setRefField(f, to_object_ref(val));
//    setInstFieldValue(obj, fieldID, to_object_ref(val));
}

void JNICALL JVM_SetBooleanField(JNIEnv *env, jobject obj, jfieldID fieldID, jboolean val)
{
    Object *o = to_object_ref(obj);
    Field *f = to_field(fieldID);
    o->setBoolField(f, val);
}

void JNICALL JVM_SetByteField(JNIEnv *env, jobject obj, jfieldID fieldID, jbyte val)
{
    Object *o = to_object_ref(obj);
    Field *f = to_field(fieldID);
    o->setByteField(f, val);
}

void JNICALL JVM_SetCharField(JNIEnv *env, jobject obj, jfieldID fieldID, jchar val)
{
    Object *o = to_object_ref(obj);
    Field *f = to_field(fieldID);
    o->setCharField(f, val);
}

void JNICALL JVM_SetShortField(JNIEnv *env, jobject obj, jfieldID fieldID, jshort val)
{
    Object *o = to_object_ref(obj);
    Field *f = to_field(fieldID);
    o->setShortField(f, val);
}

void JNICALL JVM_SetIntField(JNIEnv *env, jobject obj, jfieldID fieldID, jint val)
{
    Object *o = to_object_ref(obj);
    Field *f = to_field(fieldID);
    o->setIntField(f, val);
}

void JNICALL JVM_SetLongField(JNIEnv *env, jobject obj, jfieldID fieldID, jlong val)
{
    Object *o = to_object_ref(obj);
    Field *f = to_field(fieldID);
    o->setLongField(f, val);
}

void JNICALL JVM_SetFloatField(JNIEnv *env, jobject obj, jfieldID fieldID, jfloat val)
{
    Object *o = to_object_ref(obj);
    Field *f = to_field(fieldID);
    o->setFloatField(f, val);
}

void JNICALL JVM_SetDoubleField(JNIEnv *env, jobject obj, jfieldID fieldID, jdouble val)
{
    Object *o = to_object_ref(obj);
    Field *f = to_field(fieldID);
    o->setDoubleField(f, val);
}

jmethodID JNICALL JVM_GetStaticMethodID(JNIEnv *env, jclass clazz, const char *name, const char *sig)
{
    assert(env != nullptr && clazz != nullptr && name != nullptr && sig != nullptr);

    Class *c = to_object_ref<Class>(clazz);
    Method *m = c->lookupStaticMethod(name, sig);
    return to_jmethodID(m);
}

DEFINE_3_CALL_STATIC_T_METHODS(Object, jobject, addJNILocalRef(RSLOT(ret)))
DEFINE_3_CALL_STATIC_T_METHODS(Boolean, jboolean, jint2jbool(ISLOT(ret)))
DEFINE_3_CALL_STATIC_T_METHODS(Byte, jbyte, jint2jbyte(ISLOT(ret)))
DEFINE_3_CALL_STATIC_T_METHODS(Char, jchar, jint2jchar(ISLOT(ret)))
DEFINE_3_CALL_STATIC_T_METHODS(Short, jshort, jint2jshort(ISLOT(ret)))
DEFINE_3_CALL_STATIC_T_METHODS(Int, jint, ISLOT(ret))
DEFINE_3_CALL_STATIC_T_METHODS(Long, jlong, LSLOT(ret))
DEFINE_3_CALL_STATIC_T_METHODS(Float, jfloat, FSLOT(ret))
DEFINE_3_CALL_STATIC_T_METHODS(Double, jdouble, DSLOT(ret))
DEFINE_3_CALL_STATIC_T_METHODS(Void, void, )

jfieldID JNICALL JVM_GetStaticFieldID(JNIEnv *env, jclass clazz, const char *name, const char *sig)
{
    assert(env != nullptr && clazz != nullptr && name != nullptr && sig != nullptr);
    return getFieldID(env, clazz, name, sig, true);
}

jobject JNICALL JVM_GetStaticObjectField(JNIEnv *env, jclass clazz, jfieldID fieldID)
{
    assert(env != nullptr && clazz != nullptr && fieldID != nullptr);
    return addJNILocalRef(to_field(fieldID)->staticValue.r);
}

jboolean JNICALL JVM_GetStaticBooleanField(JNIEnv *env, jclass clazz, jfieldID fieldID)
{
    assert(env != nullptr && clazz != nullptr && fieldID != nullptr);
    return to_field(fieldID)->staticValue.z;
}

jbyte JNICALL JVM_GetStaticByteField(JNIEnv *env, jclass clazz, jfieldID fieldID)
{
    assert(env != nullptr && clazz != nullptr && fieldID != nullptr);
    return to_field(fieldID)->staticValue.b;
}

jchar JNICALL JVM_GetStaticCharField(JNIEnv *env, jclass clazz, jfieldID fieldID)
{
    assert(env != nullptr && clazz != nullptr && fieldID != nullptr);
    return to_field(fieldID)->staticValue.c;
}

jshort JNICALL JVM_GetStaticShortField(JNIEnv *env, jclass clazz, jfieldID fieldID)
{
    assert(env != nullptr && clazz != nullptr && fieldID != nullptr);
    return to_field(fieldID)->staticValue.s;
}

jint JNICALL JVM_GetStaticIntField(JNIEnv *env, jclass clazz, jfieldID fieldID)
{
    assert(env != nullptr && clazz != nullptr && fieldID != nullptr);
    return to_field(fieldID)->staticValue.i;
}

jlong JNICALL JVM_GetStaticLongField(JNIEnv *env, jclass clazz, jfieldID fieldID)
{
    assert(env != nullptr && clazz != nullptr && fieldID != nullptr);
    return to_field(fieldID)->staticValue.j;
}

jfloat JNICALL JVM_GetStaticFloatField(JNIEnv *env, jclass clazz, jfieldID fieldID)
{
    assert(env != nullptr && clazz != nullptr && fieldID != nullptr);
    return to_field(fieldID)->staticValue.f;
}

jdouble JNICALL JVM_GetStaticDoubleField(JNIEnv *env, jclass clazz, jfieldID fieldID)
{
    assert(env != nullptr && clazz != nullptr && fieldID != nullptr);
    return to_field(fieldID)->staticValue.d;
}

void JNICALL JVM_SetStaticObjectField(JNIEnv *env, jclass clazz, jfieldID fieldID, jobject value)
{
    assert(env != nullptr && clazz != nullptr && fieldID != nullptr);
    to_field(fieldID)->staticValue.r = to_object_ref(value);
}

void JNICALL JVM_SetStaticBooleanField(JNIEnv *env, jclass clazz, jfieldID fieldID, jboolean value)
{
    assert(env != nullptr && clazz != nullptr && fieldID != nullptr);
    to_field(fieldID)->staticValue.z = value;
}

void JNICALL JVM_SetStaticByteField(JNIEnv *env, jclass clazz, jfieldID fieldID, jbyte value)
{
    assert(env != nullptr && clazz != nullptr && fieldID != nullptr);
    to_field(fieldID)->staticValue.b = value;
}

void JNICALL JVM_SetStaticCharField(JNIEnv *env, jclass clazz, jfieldID fieldID, jchar value)
{
    assert(env != nullptr && clazz != nullptr && fieldID != nullptr);
    to_field(fieldID)->staticValue.c = value;
}

void JNICALL JVM_SetStaticShortField(JNIEnv *env, jclass clazz, jfieldID fieldID, jshort value)
{
    assert(env != nullptr && clazz != nullptr && fieldID != nullptr);
    to_field(fieldID)->staticValue.s = value;
}

void JNICALL JVM_SetStaticIntField(JNIEnv *env, jclass clazz, jfieldID fieldID, jint value)
{
    assert(env != nullptr && clazz != nullptr && fieldID != nullptr);
    to_field(fieldID)->staticValue.i = value;
}

void JNICALL JVM_SetStaticLongField(JNIEnv *env, jclass clazz, jfieldID fieldID, jlong value)
{
    assert(env != nullptr && clazz != nullptr && fieldID != nullptr);
    to_field(fieldID)->staticValue.j = value;
}

void JNICALL JVM_SetStaticFloatField(JNIEnv *env, jclass clazz, jfieldID fieldID, jfloat value)
{
    assert(env != nullptr && clazz != nullptr && fieldID != nullptr);
    to_field(fieldID)->staticValue.f = value;
}

void JNICALL JVM_SetStaticDoubleField(JNIEnv *env, jclass clazz, jfieldID fieldID, jdouble value)
{
    assert(env != nullptr && clazz != nullptr && fieldID != nullptr);
    to_field(fieldID)->staticValue.d = value;
}

jstring JNICALL JVM_NewString(JNIEnv *env, const jchar *unicode, jsize len)
{
    assert(env != nullptr && unicode != nullptr && len >= 0);
    jstrref str = newString(unicode, len);
    return (jstring) addJNILocalRef(str);
}

jsize JNICALL JVM_GetStringLength(JNIEnv *env, jstring str)
{
    assert(env != nullptr && str != nullptr);
    return strObjGetLength(to_object_ref(str));
}

const jchar *JNICALL JVM_GetStringChars(JNIEnv *env, jstring str, jboolean *isCopy)
{
    assert(env != nullptr && str != nullptr);

    jstrref so = to_object_ref(str);
    if (g_jdk_version_9_and_upper) {
        // byte[] value;
        auto value = so->getRefField<Array>(S(value), S(array_B));
        if (isCopy != nullptr)
            *isCopy = JNI_TRUE;
        return utf8::toUnicode((utf8_t *) value->data, value->len);
    } else {
        // char[] value;
        auto value = so->getRefField<Array>(S(value), S(array_C));
        addJNIGlobalRef(so); /* Pin the reference */
        if (isCopy != nullptr)
            *isCopy = JNI_FALSE;
        return (jchar *) value->data;
    }
}

void JNICALL JVM_ReleaseStringChars(JNIEnv *env, jstring str, const jchar *chars)
{
    assert(env != nullptr && str != nullptr && chars != nullptr);

    if (g_jdk_version_9_and_upper) {
        delete[] chars;
    } else {
        deleteJNIGlobalRef(to_object_ref(str)); /* Unpin the reference */
    }
}

jstring JNICALL JVM_NewStringUTF(JNIEnv *env, const char *utf)
{
    jstrref str = newString(utf);
    return (jstring) addJNILocalRef(str);
}

jsize JNICALL JVM_GetStringUTFLength(JNIEnv *env, jstring str)
{
    assert(env != nullptr && str != nullptr);
    return strObjGetUTFLength(to_object_ref(str));
}

const char* JNICALL JVM_GetStringUTFChars(JNIEnv *env, jstring str, jboolean *isCopy)
{
    assert(env != nullptr && str != nullptr);

    jstrref so = to_object_ref(str);
    if (g_jdk_version_9_and_upper) {
        // byte[] value;
        auto value = so->getRefField<Array>(S(value), S(array_B));
        addJNIGlobalRef(so); /* Pin the reference */
        if (isCopy != nullptr)
            *isCopy = JNI_FALSE;
        return (char *) value->data;
    } else {
        // char[] value;
        auto value = so->getRefField<Array>(S(value), S(array_C));
        if (isCopy != nullptr)
            *isCopy = JNI_TRUE;
        return unicode::toUtf8((unicode_t *) value->data, value->len);
    }
}

void JNICALL JVM_ReleaseStringUTFChars(JNIEnv *env, jstring str, const char *chars)
{
    assert(env != nullptr && str != nullptr && chars != nullptr);

    if (g_jdk_version_9_and_upper) {
        deleteJNIGlobalRef(to_object_ref(str)); /* Unpin the reference */
    } else {
        delete[] chars;
    }
}

jsize JNICALL JVM_GetArrayLength(JNIEnv *env, jarray array)
{
    assert(env != nullptr && array != nullptr);
    auto arr = to_object_ref<Array>(array);
    return arr->len;
}

jobjectArray JNICALL JVM_NewObjectArray(JNIEnv *env, jsize len, jclass clazz, jobject init)
{
    // todo
    assert(env != nullptr && clazz != nullptr);
    if (len < 0) {
        // todo java_lang_NegativeArraySizeException
    }

    Array *arr = newArray(to_object_ref<Class>(clazz), len);
    if (init != nullptr) {
        jref o = to_object_ref(init);
        for (int i = 0; i < len; ++i) {
            arr->set(i, o);
        }
    }

    return (jobjectArray) addJNILocalRef(arr);
}

jobject JNICALL JVM_GetObjectArrayElement(JNIEnv *env, jobjectArray array, jsize index)
{
    assert(env != nullptr && array != nullptr);

    auto ao = to_object_ref<Array>(array);
    if (index <= 0 || index >= ao->len) {
        // todo error
    }

    return to_jobject(ao->get<jref>(index));
}

void JNICALL JVM_SetObjectArrayElement(JNIEnv *env, jobjectArray array, jsize index, jobject val)
{
    assert(env != nullptr && array != nullptr);

    auto ao = to_object_ref<Array>(array);
    if (index <= 0 || index >= ao->len) {
        // todo error
    }

    ao->set(index, to_object_ref(val));
}

template <typename jtypeArray, ArrayType arr_type>
jtypeArray JNICALL JVM_NewTypeArray(JNIEnv *env, jsize len)
{
    assert(env != nullptr);
    if (len < 0) {
        /* todo java_lang_NegativeArraySizeException */
    }

    Array *arr = newTypeArray(arr_type, len);
    return (jtypeArray) addJNILocalRef(arr);
}

template <typename jtypeArray, typename raw_type>
raw_type* JNICALL JVM_GetTypeArrayElements(JNIEnv *env, jtypeArray array, jboolean *isCopy)
{
    assert(env != nullptr && array != nullptr);
    auto arr = to_object_ref<Array>(array);
    if (isCopy != nullptr)
        *isCopy = JNI_FALSE;
    addJNIGlobalRef(arr);
    return (raw_type *) arr->data;
}

template <typename jtypeArray, typename raw_type>
void JNICALL JVM_ReleaseTypeArrayElements(JNIEnv *env, jtypeArray array, raw_type *elems, jint mode)
{
    assert(env != nullptr);
    deleteJNIGlobalRef(to_object_ref(array));
}

template <typename jtypeArray, typename raw_type>
void JNICALL JVM_GetTypeArrayRegion(JNIEnv *env, jtypeArray array, jsize start, jsize len, raw_type *buf)
{
    assert(env != nullptr && array != nullptr && buf != nullptr);
    auto arr = to_object_ref<Array>(array);
    assert(start + len <= arr->len);
    assert(arr->clazz->getEleSize() == sizeof(raw_type));
    memcpy(buf, arr->index(start), len*sizeof(raw_type));
}

template <typename jtypeArray, typename raw_type>
void JNICALL JVM_SetTypeArrayRegion(JNIEnv *env, jtypeArray array, jsize start, jsize len, const raw_type *buf)
{
    assert(env != nullptr && array != nullptr && buf != nullptr);
    auto arr = to_object_ref<Array>(array);
    assert(start + len <= arr->len);
    assert(arr->clazz->getEleSize() == sizeof(raw_type));
    memcpy(arr->index(start), buf, len*sizeof(raw_type));
}

jint JNICALL JVM_RegisterNatives(JNIEnv *env, jclass clazz, const JNINativeMethod *methods, jint nMethods)
{
    assert(env != nullptr && clazz != nullptr);

    auto c = to_object_ref<Class>(clazz);
    for (int i = 0; i < nMethods; i++) {
        Method *m = c->lookupMethod(methods[i].name, methods[i].signature);
        if (m == nullptr or !m->isNative()) {
            // todo java_lang_NoSuchMethodError
            return JNI_ERR;
        }

        // todo
        jvm_abort("not implement.");
    }
    return JNI_OK;
}

jint JNICALL JVM_UnregisterNatives(JNIEnv *env, jclass clazz)
{
    assert(env != nullptr && clazz != nullptr);
    // todo
    jvm_abort("not implement.");
}

jint JNICALL JVM_MonitorEnter(JNIEnv *env, jobject obj)
{
    assert(env != nullptr && obj != nullptr);
    // todo
//    jref o = to_object_ref(obj);
//    o->lock();
// return JNI_OK;
    jvm_abort("not implement.");
}

jint JNICALL JVM_MonitorExit(JNIEnv *env, jobject obj)
{
    assert(env != nullptr && obj != nullptr);
    // todo
//    jref o = to_object_ref(obj);
//    o->unlock();
// return JNI_OK;
    jvm_abort("not implement.");
}

static JavaVM java_vm;

jint JNICALL JVM_GetJavaVM(JNIEnv *env, JavaVM **vm)
{
    *vm = &java_vm;
    return JNI_OK;
}

void JNICALL JVM_GetStringRegion(JNIEnv *env, jstring str, jsize start, jsize len, jchar *buf)
{
    assert(env != nullptr && str != nullptr && buf != nullptr);
    auto so = to_object_ref(str);
    jvm_abort("not implement."); // todo
}

void JNICALL JVM_GetStringUTFRegion(JNIEnv *env, jstring str, jsize start, jsize len, char *buf)
{
    assert(env != nullptr && str != nullptr && buf != nullptr);
    auto so = to_object_ref(str);
    jvm_abort("not implement."); // todo
}

void* JNICALL JVM_GetPrimitiveArrayCritical(JNIEnv *env, jarray array, jboolean *isCopy)
{
    assert(env != nullptr && array != nullptr);
    auto arr = to_object_ref<Array>(array);

    if(isCopy != nullptr)
        *isCopy = JNI_FALSE;

    /* Pin the array */ //  todo
    addJNIGlobalRef(arr);
    return arr->data;
}

void JNICALL JVM_ReleasePrimitiveArrayCritical(JNIEnv *env, jarray array, void *carray, jint mode)
{
    assert(env != nullptr && array != nullptr);
    deleteJNIGlobalRef(to_object_ref<Array>(array));
}

const jchar* JNICALL JVM_GetStringCritical(JNIEnv *env, jstring string, jboolean *isCopy)
{
    return JVM_GetStringChars(env, string, isCopy);
}

void JNICALL JVM_ReleaseStringCritical(JNIEnv *env, jstring string, const jchar *cstring)
{
    JVM_ReleaseStringChars(env, string, cstring);
}

jweak JNICALL JVM_NewWeakGlobalRef(JNIEnv *env, jobject obj)
{
    assert(env != nullptr && obj != nullptr);
    return addJNIWeakGlobalRef(to_object_ref(obj));
}

void JNICALL JVM_DeleteWeakGlobalRef(JNIEnv *env, jweak ref)
{
    assert(env != nullptr && ref != nullptr);
    deleteJNIWeakGlobalRef(to_object_ref(ref));
}

jboolean JNICALL JVM_ExceptionCheck(JNIEnv *env)
{
    // todo
    jvm_abort("not implement.");
}

jobject JNICALL JVM_NewDirectByteBuffer(JNIEnv *env, void *address, jlong capacity)
{
    // todo
    jvm_abort("not implement.");
}

void* JNICALL JVM_GetDirectBufferAddress(JNIEnv *env, jobject buf)
{
    // todo
    jvm_abort("not implement.");
}

jlong JNICALL JVM_GetDirectBufferCapacity(JNIEnv *env, jobject buf)
{
    // todo
    jvm_abort("not implement.");
}

jobjectRefType JNICALL JVM_GetObjectRefType(JNIEnv* env, jobject obj)
{
    jref o = to_object_ref(obj);
    return o != nullptr ? o->jni_obj_ref_type : JNIInvalidRefType;
}

jobject JNICALL JVM_GetModule(JNIEnv* env, jclass clazz)
{
    // todo
    jvm_abort("not implement.");
}


static struct JNINativeInterface_ JVM_JNINativeInterface = {
    .reserved0 = nullptr,
    .reserved1 = nullptr,
    .reserved2 = nullptr,
    .reserved3 = nullptr,

    .GetVersion = JVM_GetVersion,

    .DefineClass = JVM_DefineClass,
    .FindClass = JVM_FindClass,

    .FromReflectedMethod = JVM_FromReflectedMethod,
    .FromReflectedField = JVM_FromReflectedField,

    .ToReflectedMethod = JVM_ToReflectedMethod,

    .GetSuperclass = JVM_GetSuperclass,
    .IsAssignableFrom = JVM_IsAssignableFrom,

    .ToReflectedField = JVM_ToReflectedField,

    .Throw = JVM_Throw,
    .ThrowNew = JVM_ThrowNew,
    .ExceptionOccurred = JVM_ExceptionOccurred,
    .ExceptionDescribe = JVM_ExceptionDescribe,
    .ExceptionClear = JVM_ExceptionClear,
    .FatalError = JVM_FatalError,

    .PushLocalFrame = JVM_PushLocalFrame,
    .PopLocalFrame = JVM_PopLocalFrame,

    .NewGlobalRef = JVM_NewGlobalRef,
    .DeleteGlobalRef = JVM_DeleteGlobalRef,
    .DeleteLocalRef = JVM_DeleteLocalRef,
    .IsSameObject = JVM_IsSameObject,
    .NewLocalRef = JVM_NewLocalRef,
    .EnsureLocalCapacity = JVM_EnsureLocalCapacity,

    .AllocObject = JVM_AllocObject,
    .NewObject = JVM_NewObject,
    .NewObjectV = JVM_NewObjectV,
    .NewObjectA = JVM_NewObjectA,

    .GetObjectClass = JVM_GetObjectClass,
    .IsInstanceOf = JVM_IsInstanceOf,

    .GetMethodID = JVM_GetMethodID,

//    .CallObjectMethod = JVM_CallMethod<jobject, getJobjectRetValue>,
//    .CallObjectMethodV = JVM_CallMethodV<jobject, getJobjectRetValue>,
//    .CallObjectMethodA = JVM_CallMethodA<jobject, getJobjectRetValue>,
    .CallObjectMethod = JVM_CallObjectMethod,
    .CallObjectMethodV = JVM_CallObjectMethodV,
    .CallObjectMethodA = JVM_CallObjectMethodA,

    .CallBooleanMethod = JVM_CallBooleanMethod,
    .CallBooleanMethodV = JVM_CallBooleanMethodV,
    .CallBooleanMethodA = JVM_CallBooleanMethodA,

    .CallByteMethod = JVM_CallByteMethod,
    .CallByteMethodV = JVM_CallByteMethodV,
    .CallByteMethodA = JVM_CallByteMethodA,

    .CallCharMethod = JVM_CallCharMethod,
    .CallCharMethodV = JVM_CallCharMethodV,
    .CallCharMethodA = JVM_CallCharMethodA,

    .CallShortMethod = JVM_CallShortMethod,
    .CallShortMethodV = JVM_CallShortMethodV,
    .CallShortMethodA = JVM_CallShortMethodA,

    .CallIntMethod = JVM_CallIntMethod,
    .CallIntMethodV = JVM_CallIntMethodV,
    .CallIntMethodA = JVM_CallIntMethodA,

    .CallLongMethod = JVM_CallLongMethod,
    .CallLongMethodV = JVM_CallLongMethodV,
    .CallLongMethodA = JVM_CallLongMethodA,

    .CallFloatMethod = JVM_CallFloatMethod,
    .CallFloatMethodV = JVM_CallFloatMethodV,
    .CallFloatMethodA = JVM_CallFloatMethodA,

    .CallDoubleMethod = JVM_CallDoubleMethod,
    .CallDoubleMethodV = JVM_CallDoubleMethodV,
    .CallDoubleMethodA = JVM_CallDoubleMethodA,

    .CallVoidMethod = JVM_CallVoidMethod,
    .CallVoidMethodV = JVM_CallVoidMethodV,
    .CallVoidMethodA = JVM_CallVoidMethodA,

    .CallNonvirtualObjectMethod = JVM_CallNonvirtualObjectMethod,
    .CallNonvirtualObjectMethodV = JVM_CallNonvirtualObjectMethodV,
    .CallNonvirtualObjectMethodA = JVM_CallNonvirtualObjectMethodA,

    .CallNonvirtualBooleanMethod = JVM_CallNonvirtualBooleanMethod,
    .CallNonvirtualBooleanMethodV = JVM_CallNonvirtualBooleanMethodV,
    .CallNonvirtualBooleanMethodA = JVM_CallNonvirtualBooleanMethodA,

    .CallNonvirtualByteMethod = JVM_CallNonvirtualByteMethod,
    .CallNonvirtualByteMethodV = JVM_CallNonvirtualByteMethodV,
    .CallNonvirtualByteMethodA = JVM_CallNonvirtualByteMethodA,

    .CallNonvirtualCharMethod = JVM_CallNonvirtualCharMethod,
    .CallNonvirtualCharMethodV = JVM_CallNonvirtualCharMethodV,
    .CallNonvirtualCharMethodA = JVM_CallNonvirtualCharMethodA,

    .CallNonvirtualShortMethod = JVM_CallNonvirtualShortMethod,
    .CallNonvirtualShortMethodV = JVM_CallNonvirtualShortMethodV,
    .CallNonvirtualShortMethodA = JVM_CallNonvirtualShortMethodA,

    .CallNonvirtualIntMethod = JVM_CallNonvirtualIntMethod,
    .CallNonvirtualIntMethodV = JVM_CallNonvirtualIntMethodV,
    .CallNonvirtualIntMethodA = JVM_CallNonvirtualIntMethodA,

    .CallNonvirtualLongMethod = JVM_CallNonvirtualLongMethod,
    .CallNonvirtualLongMethodV = JVM_CallNonvirtualLongMethodV,
    .CallNonvirtualLongMethodA = JVM_CallNonvirtualLongMethodA,

    .CallNonvirtualFloatMethod = JVM_CallNonvirtualFloatMethod,
    .CallNonvirtualFloatMethodV = JVM_CallNonvirtualFloatMethodV,
    .CallNonvirtualFloatMethodA = JVM_CallNonvirtualFloatMethodA,

    .CallNonvirtualDoubleMethod = JVM_CallNonvirtualDoubleMethod,
    .CallNonvirtualDoubleMethodV = JVM_CallNonvirtualDoubleMethodV,
    .CallNonvirtualDoubleMethodA = JVM_CallNonvirtualDoubleMethodA,

    .CallNonvirtualVoidMethod = JVM_CallNonvirtualVoidMethod,
    .CallNonvirtualVoidMethodV = JVM_CallNonvirtualVoidMethodV,
    .CallNonvirtualVoidMethodA = JVM_CallNonvirtualVoidMethodA,

    .GetFieldID = JVM_GetFieldID,

    .GetObjectField = JVM_GetObjectField,
    .GetBooleanField = JVM_GetBooleanField,
    .GetByteField = JVM_GetByteField,
    .GetCharField = JVM_GetCharField,
    .GetShortField = JVM_GetShortField,
    .GetIntField = JVM_GetIntField,
    .GetLongField = JVM_GetLongField,
    .GetFloatField = JVM_GetFloatField,
    .GetDoubleField = JVM_GetDoubleField,

    .SetObjectField = JVM_SetObjectField,
    .SetBooleanField = JVM_SetBooleanField,
    .SetByteField = JVM_SetByteField,
    .SetCharField = JVM_SetCharField,
    .SetShortField = JVM_SetShortField,
    .SetIntField = JVM_SetIntField,
    .SetLongField = JVM_SetLongField,
    .SetFloatField = JVM_SetFloatField,
    .SetDoubleField = JVM_SetDoubleField,

    .GetStaticMethodID = JVM_GetStaticMethodID,

    .CallStaticObjectMethod = JVM_CallStaticObjectMethod,
    .CallStaticObjectMethodV = JVM_CallStaticObjectMethodV,
    .CallStaticObjectMethodA = JVM_CallStaticObjectMethodA,

    .CallStaticBooleanMethod = JVM_CallStaticBooleanMethod,
    .CallStaticBooleanMethodV = JVM_CallStaticBooleanMethodV,
    .CallStaticBooleanMethodA = JVM_CallStaticBooleanMethodA,

    .CallStaticByteMethod = JVM_CallStaticByteMethod,
    .CallStaticByteMethodV = JVM_CallStaticByteMethodV,
    .CallStaticByteMethodA = JVM_CallStaticByteMethodA,

    .CallStaticCharMethod = JVM_CallStaticCharMethod,
    .CallStaticCharMethodV = JVM_CallStaticCharMethodV,
    .CallStaticCharMethodA = JVM_CallStaticCharMethodA,

    .CallStaticShortMethod = JVM_CallStaticShortMethod,
    .CallStaticShortMethodV = JVM_CallStaticShortMethodV,
    .CallStaticShortMethodA = JVM_CallStaticShortMethodA,

    .CallStaticIntMethod = JVM_CallStaticIntMethod,
    .CallStaticIntMethodV = JVM_CallStaticIntMethodV,
    .CallStaticIntMethodA = JVM_CallStaticIntMethodA,

    .CallStaticLongMethod = JVM_CallStaticLongMethod,
    .CallStaticLongMethodV = JVM_CallStaticLongMethodV,
    .CallStaticLongMethodA = JVM_CallStaticLongMethodA,

    .CallStaticFloatMethod = JVM_CallStaticFloatMethod,
    .CallStaticFloatMethodV = JVM_CallStaticFloatMethodV,
    .CallStaticFloatMethodA = JVM_CallStaticFloatMethodA,

    .CallStaticDoubleMethod = JVM_CallStaticDoubleMethod,
    .CallStaticDoubleMethodV = JVM_CallStaticDoubleMethodV,
    .CallStaticDoubleMethodA = JVM_CallStaticDoubleMethodA,

    .CallStaticVoidMethod = JVM_CallStaticVoidMethod,
    .CallStaticVoidMethodV = JVM_CallStaticVoidMethodV,
    .CallStaticVoidMethodA = JVM_CallStaticVoidMethodA,

    .GetStaticFieldID = JVM_GetStaticFieldID,
    .GetStaticObjectField = JVM_GetStaticObjectField,
    .GetStaticBooleanField = JVM_GetStaticBooleanField,
    .GetStaticByteField = JVM_GetStaticByteField,
    .GetStaticCharField = JVM_GetStaticCharField,
    .GetStaticShortField = JVM_GetStaticShortField,
    .GetStaticIntField = JVM_GetStaticIntField,
    .GetStaticLongField = JVM_GetStaticLongField,
    .GetStaticFloatField = JVM_GetStaticFloatField,
    .GetStaticDoubleField = JVM_GetStaticDoubleField,

    .SetStaticObjectField = JVM_SetStaticObjectField,
    .SetStaticBooleanField = JVM_SetStaticBooleanField,
    .SetStaticByteField = JVM_SetStaticByteField,
    .SetStaticCharField = JVM_SetStaticCharField,
    .SetStaticShortField = JVM_SetStaticShortField,
    .SetStaticIntField = JVM_SetStaticIntField,
    .SetStaticLongField = JVM_SetStaticLongField,
    .SetStaticFloatField = JVM_SetStaticFloatField,
    .SetStaticDoubleField = JVM_SetStaticDoubleField,

    .NewString = JVM_NewString,
    .GetStringLength = JVM_GetStringLength,
    .GetStringChars = JVM_GetStringChars,
    .ReleaseStringChars = JVM_ReleaseStringChars,

    .NewStringUTF = JVM_NewStringUTF,
    .GetStringUTFLength = JVM_GetStringUTFLength,
    .GetStringUTFChars = JVM_GetStringUTFChars,
    .ReleaseStringUTFChars = JVM_ReleaseStringUTFChars,

    .GetArrayLength = JVM_GetArrayLength,

    .NewObjectArray = JVM_NewObjectArray,
    .GetObjectArrayElement = JVM_GetObjectArrayElement,
    .SetObjectArrayElement = JVM_SetObjectArrayElement,
    .NewBooleanArray = JVM_NewTypeArray<jbooleanArray, JVM_AT_BOOLEAN>,
    .NewByteArray = JVM_NewTypeArray<jbyteArray, JVM_AT_BYTE>,
    .NewCharArray = JVM_NewTypeArray<jcharArray, JVM_AT_CHAR>,
    .NewShortArray = JVM_NewTypeArray<jshortArray, JVM_AT_SHORT>,
    .NewIntArray = JVM_NewTypeArray<jintArray, JVM_AT_INT>,
    .NewLongArray = JVM_NewTypeArray<jlongArray, JVM_AT_LONG>,
    .NewFloatArray = JVM_NewTypeArray<jfloatArray, JVM_AT_FLOAT>,
    .NewDoubleArray = JVM_NewTypeArray<jdoubleArray, JVM_AT_DOUBLE>,

    .GetBooleanArrayElements = JVM_GetTypeArrayElements<jbooleanArray, jboolean>,
    .GetByteArrayElements = JVM_GetTypeArrayElements<jbyteArray, jbyte>,
    .GetCharArrayElements = JVM_GetTypeArrayElements<jcharArray, jchar>,
    .GetShortArrayElements = JVM_GetTypeArrayElements<jshortArray, jshort>,
    .GetIntArrayElements = JVM_GetTypeArrayElements<jintArray, jint>,
    .GetLongArrayElements = JVM_GetTypeArrayElements<jlongArray, jlong>,
    .GetFloatArrayElements = JVM_GetTypeArrayElements<jfloatArray, jfloat>,
    .GetDoubleArrayElements = JVM_GetTypeArrayElements<jdoubleArray, jdouble>,

    .ReleaseBooleanArrayElements = JVM_ReleaseTypeArrayElements<jbooleanArray, jboolean>,
    .ReleaseByteArrayElements = JVM_ReleaseTypeArrayElements<jbyteArray, jbyte>,
    .ReleaseCharArrayElements = JVM_ReleaseTypeArrayElements<jcharArray, jchar>,
    .ReleaseShortArrayElements = JVM_ReleaseTypeArrayElements<jshortArray, jshort>,
    .ReleaseIntArrayElements = JVM_ReleaseTypeArrayElements<jintArray, jint>,
    .ReleaseLongArrayElements = JVM_ReleaseTypeArrayElements<jlongArray, jlong>,
    .ReleaseFloatArrayElements = JVM_ReleaseTypeArrayElements<jfloatArray, jfloat>,
    .ReleaseDoubleArrayElements = JVM_ReleaseTypeArrayElements<jdoubleArray, jdouble>,

    .GetBooleanArrayRegion = JVM_GetTypeArrayRegion<jbooleanArray, jboolean>,
    .GetByteArrayRegion = JVM_GetTypeArrayRegion<jbyteArray, jbyte>,
    .GetCharArrayRegion = JVM_GetTypeArrayRegion<jcharArray, jchar>,
    .GetShortArrayRegion = JVM_GetTypeArrayRegion<jshortArray, jshort>,
    .GetIntArrayRegion = JVM_GetTypeArrayRegion<jintArray, jint>,
    .GetLongArrayRegion = JVM_GetTypeArrayRegion<jlongArray, jlong>,
    .GetFloatArrayRegion = JVM_GetTypeArrayRegion<jfloatArray, jfloat>,
    .GetDoubleArrayRegion = JVM_GetTypeArrayRegion<jdoubleArray, jdouble>,

    .SetBooleanArrayRegion = JVM_SetTypeArrayRegion<jbooleanArray, jboolean>,
    .SetByteArrayRegion = JVM_SetTypeArrayRegion<jbyteArray, jbyte>,
    .SetCharArrayRegion = JVM_SetTypeArrayRegion<jcharArray, jchar>,
    .SetShortArrayRegion = JVM_SetTypeArrayRegion<jshortArray, jshort>,
    .SetIntArrayRegion = JVM_SetTypeArrayRegion<jintArray, jint>,
    .SetLongArrayRegion = JVM_SetTypeArrayRegion<jlongArray, jlong>,
    .SetFloatArrayRegion = JVM_SetTypeArrayRegion<jfloatArray, jfloat>,
    .SetDoubleArrayRegion = JVM_SetTypeArrayRegion<jdoubleArray, jdouble>,

    .RegisterNatives = JVM_RegisterNatives,
    .UnregisterNatives = JVM_UnregisterNatives,

    .MonitorEnter = JVM_MonitorEnter,
    .MonitorExit = JVM_MonitorExit,

    .GetJavaVM = JVM_GetJavaVM,

    .GetStringRegion = JVM_GetStringRegion,
    .GetStringUTFRegion = JVM_GetStringUTFRegion,

    .GetPrimitiveArrayCritical = JVM_GetPrimitiveArrayCritical,
    .ReleasePrimitiveArrayCritical = JVM_ReleasePrimitiveArrayCritical,

    .GetStringCritical = JVM_GetStringCritical,
    .ReleaseStringCritical = JVM_ReleaseStringCritical,

    .NewWeakGlobalRef = JVM_NewWeakGlobalRef,
    .DeleteWeakGlobalRef = JVM_DeleteWeakGlobalRef,

    .ExceptionCheck = JVM_ExceptionCheck,

    .NewDirectByteBuffer = JVM_NewDirectByteBuffer,
    .GetDirectBufferAddress = JVM_GetDirectBufferAddress,
    .GetDirectBufferCapacity = JVM_GetDirectBufferCapacity,

    .GetObjectRefType = JVM_GetObjectRefType,

    .GetModule = JVM_GetModule,
};

////////////////////////////////////////////////////////////////////////////////////////////////////

jint JNICALL JVM_DestroyJavaVM(JavaVM *vm)
{
    jvm_abort("not implement.");  //  todo
    return JNI_OK;
}

jint JNICALL JVM_AttachCurrentThread(JavaVM *vm, void **penv, void *args)
{
    jvm_abort("not implement.");  //  todo
    return JNI_OK;
}

jint JNICALL JVM_DetachCurrentThread(JavaVM *vm)
{
    jvm_abort("not implement.");  //  todo
    return JNI_OK;
}

static JNIEnv jni_env;

jint JNICALL JVM_GetEnv(JavaVM *vm, void **penv, jint version)
{
    assert(penv != nullptr);

    if (getCurrentThread() == nullptr) {
        *penv = nullptr;
        return JNI_EDETACHED;
    }

    *penv = &jni_env;
    return JNI_OK;
}

jint JNICALL JVM_AttachCurrentThreadAsDaemon(JavaVM *vm, void **penv, void *args)
{
    jvm_abort("not implement.");  //  todo
    return JNI_OK;
}

const static struct JNIInvokeInterface_ JVM_JNIInvokeInterface = {
    .reserved0 = nullptr,
    .reserved1 = nullptr,
    .reserved2 = nullptr,

    .DestroyJavaVM = JVM_DestroyJavaVM,
    .AttachCurrentThread = JVM_AttachCurrentThread,
    .DetachCurrentThread = JVM_DetachCurrentThread,
    .GetEnv = JVM_GetEnv,
    .AttachCurrentThreadAsDaemon = JVM_AttachCurrentThreadAsDaemon,
};

////////////////////////////////////////////////////////////////////////////////////////////////////

extern "C" {
    void java_lang_Float_registerNatives();
    void java_lang_Double_registerNatives();
    void java_lang_Object_registerNatives();
    void java_lang_ClassLoader$NativeLibrary_registerNatives();

    void java_io_FileDescriptor_registerNatives();
    void java_io_RandomAccessFile_registerNatives();

    void java_lang_reflect_Executable_registerNatives();
    void java_lang_reflect_Field_registerNatives();
    void java_lang_reflect_Proxy_registerNatives();

    void java_nio_Bits_registerNatives();

    void sun_misc_Signal_registerNatives();
    void sun_misc_URLClassPath_registerNatives();
    void sun_misc_Version_registerNatives();
    void sun_misc_VM_registerNatives();

    void sun_reflect_NativeMethodAccessorImpl_registerNatives();

    void jdk_internal_misc_VM_registerNatives();
}

void initJNI()
{
    jni_env.functions = &JVM_JNINativeInterface;
    java_vm.functions = &JVM_JNIInvokeInterface;

#ifdef R
#undef R
#endif
#define R(method_name) void method_name(); /* declare */ method_name() /* invoke */

    // register all native methods // todo 不要一次全注册，需要时再注册
    R(java_lang_Class_registerNatives);

    java_lang_Float_registerNatives();
    java_lang_Double_registerNatives();    
    java_lang_Object_registerNatives();    
    R(java_lang_System_registerNatives);
    R(java_lang_String_registerNatives);
    R(java_lang_Package_registerNatives);
    R(java_lang_Throwable_registerNatives);
    R(java_lang_Thread_registerNatives);
    R(java_lang_Runtime_registerNatives);
    R(java_lang_ClassLoader_registerNatives);
    java_lang_ClassLoader$NativeLibrary_registerNatives();

    java_lang_reflect_Field_registerNatives();
    java_lang_reflect_Executable_registerNatives();
    R(java_lang_reflect_Array_registerNatives);
    java_lang_reflect_Proxy_registerNatives();

    R(java_lang_invoke_MethodHandle_registerNatives);
    R(java_lang_invoke_MethodHandleNatives_registerNatives);

    java_io_FileDescriptor_registerNatives();
    R(java_io_FileInputStream_registerNatives);
    R(java_io_FileOutputStream_registerNatives);
    R(java_io_WinNTFileSystem_registerNatives);
    java_io_RandomAccessFile_registerNatives();

    java_nio_Bits_registerNatives();

    sun_misc_VM_registerNatives();
    R(sun_misc_Unsafe_registerNatives);
    sun_misc_Signal_registerNatives();
    sun_misc_Version_registerNatives();
    sun_misc_URLClassPath_registerNatives();
    R(sun_misc_Perf_registerNatives);

    R(sun_io_Win32ErrorMode_registerNatives);

    R(sun_reflect_Reflection_registerNatives);
    R(sun_reflect_NativeConstructorAccessorImpl_registerNatives);
    sun_reflect_NativeMethodAccessorImpl_registerNatives();
    R(sun_reflect_ConstantPool_registerNatives);

    R(sun_management_VMManagementImpl_registerNatives);
    R(sun_management_ThreadImpl_registerNatives);

    R(java_security_AccessController_registerNatives);

    R(java_util_concurrent_atomic_AtomicLong_registerNatives);
    R(java_util_zip_ZipFile_registerNatives);

    jdk_internal_misc_VM_registerNatives();
}

static vector<tuple<const char * /* class name */, JNINativeMethod *, int /* method count */>> native_methods;

extern "C" void registerNatives(const char *class_name, JNINativeMethod *methods, int method_count)
{
    assert(class_name != nullptr && methods != nullptr && method_count > 0);
    native_methods.emplace_back(class_name, methods, method_count);
}

void *findNativeMethod(const char *class_name, const char *method_name, const char *method_type)
{
    assert(class_name != nullptr && method_name != nullptr && method_type != nullptr);
    for (auto &t: native_methods) {
        if (equals(get<0>(t), class_name)) {
            JNINativeMethod *methods = get<1>(t);
            for (int i = 0; i < get<2>(t); i++) {
                if (equals(methods[i].name, method_name) && equals(methods[i].signature, method_type)) {
                    return methods[i].fnPtr;
                }
            }
            return nullptr; // not find
        }
    }
}

// extern "C" jclass obj_get_class(jobject o)
// {
//     jref r = to_object_ref(o);
//     return to_jclass(r->clazz);
// }

extern "C" jobject obj_clone(jobject o)
{
    jref r = to_object_ref(o)->clone();
    return to_jobject(r);
}

extern "C" int is_subclass_of(jclass sub, jclass base)
{
    Class *s = to_object_ref<Class>(sub);
    Class *b = to_object_ref<Class>(base);
    return s->isSubclassOf(b) ? 1 : 0;
}

extern "C" void ci_initClass(jclass clazz)
{
    Class *c = to_object_ref<Class>(clazz);
    initClass(c);
}