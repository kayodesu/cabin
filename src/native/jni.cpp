/*
 * Author: kayo
 */

#include "jni_internal.h"
#include "../objects/array_object.h"
#include "../interpreter/interpreter.h"

GlobalRefTable global_refs;
GlobalRefTable weak_global_refs;

jint JNICALL Kayo_GetVersion(JNIEnv *env)
{
    assert(env != nullptr);
    // todo
    jvm_abort("not implement.");
}

jclass JNICALL Kayo_DefineClass(JNIEnv *env,
                const char *name, jobject loader, const jbyte *buf, jsize len)
{
    assert(env != nullptr && name != nullptr && buf != nullptr && len >= 0);

    Class *c = defineClass(to_object_ref(loader), (u1 *) buf, len);
    if (c != nullptr)
        linkClass(c);
    return (jclass) addJNILocalRef(c); // todo
}

jclass JNICALL Kayo_FindClass(JNIEnv *env, const char *name)
{
    assert(env != nullptr && name != nullptr);
    // todo
    jvm_abort("not implement.");
}

jmethodID JNICALL Kayo_FromReflectedMethod(JNIEnv *env, jobject method)
{
    assert(env != nullptr && method != nullptr);
    // todo
    jvm_abort("not implement.");
}

jfieldID JNICALL Kayo_FromReflectedField(JNIEnv *env, jobject field)
{
    assert(env != nullptr && field != nullptr);
    // todo
    jvm_abort("not implement.");
}

jobject JNICALL Kayo_ToReflectedMethod(JNIEnv *env, jclass cls, jmethodID methodID, jboolean isStatic)
{
    // todo
    jvm_abort("not implement.");
}

jclass JNICALL Kayo_GetSuperclass(JNIEnv *env, jclass sub)
{
    auto c = to_object_ref<Class>(sub);
    return (jclass) addJNILocalRef(c->superClass);
}

jboolean JNICALL Kayo_IsAssignableFrom(JNIEnv *env, jclass sub, jclass sup)
{
    // todo
    jvm_abort("not implement.");
}

jobject JNICALL Kayo_ToReflectedField(JNIEnv *env, jclass cls, jfieldID fieldID, jboolean isStatic)
{
    // todo
    jvm_abort("not implement.");
}

jint JNICALL Kayo_Throw(JNIEnv *env, jthrowable obj)
{
    // todo
    jvm_abort("not implement.");

//    throw Throwable(to_object_ref(obj));
    return JNI_TRUE;
}

jint JNICALL Kayo_ThrowNew(JNIEnv *env, jclass clazz, const char *msg)
{
    // todo
    jvm_abort("not implement.");

//    throw Throwable(to_object_ref<Class>(clazz), msg);
    return JNI_TRUE;
}

jthrowable JNICALL Kayo_ExceptionOccurred(JNIEnv *env)
{
    // todo
    jvm_abort("not implement.");
}

void JNICALL Kayo_ExceptionDescribe(JNIEnv *env)
{
    // todo
    jvm_abort("not implement.");
}

void JNICALL Kayo_ExceptionClear(JNIEnv *env)
{
    // todo
    jvm_abort("not implement.");
}

void JNICALL Kayo_FatalError(JNIEnv *env, const char *msg)
{
    // todo
    jvm_abort("not implement.");
}

jint JNICALL Kayo_PushLocalFrame(JNIEnv *env, jint capacity)
{
    // todo
    jvm_abort("not implement.");
}

jobject JNICALL Kayo_PopLocalFrame(JNIEnv *env, jobject result)
{
    // todo
    jvm_abort("not implement.");
}

jobject JNICALL Kayo_NewGlobalRef(JNIEnv *env, jobject lobj)
{
    // todo
    return addJNIGlobalRef(to_object_ref(lobj));
}

void JNICALL Kayo_DeleteGlobalRef(JNIEnv *env, jobject gref)
{
    auto o = to_object_ref(gref);
    if (o->jni_obj_ref_type == JNIGlobalRefType)
        deleteJNIGlobalRef(o);
}

void JNICALL Kayo_DeleteLocalRef(JNIEnv *env, jobject obj)
{
    deleteJNILocalRef(to_object_ref(obj));
}

jboolean JNICALL Kayo_IsSameObject(JNIEnv *env, jobject obj1, jobject obj2)
{
    // todo
    jvm_abort("not implement.");
}

jobject JNICALL Kayo_NewLocalRef(JNIEnv *env, jobject obj)
{
    return addJNILocalRef(to_object_ref(obj));
}

jint JNICALL Kayo_EnsureLocalCapacity(JNIEnv *env, jint capacity)
{
    // todo
    jvm_abort("not implement.");
}

jobject JNICALL Kayo_AllocObject(JNIEnv *env, jclass clazz)
{
    // todo
    Class *c = checkClassBeforeAllocObject(to_object_ref<Class>(clazz));
    if (c == nullptr) {
        // todo
    }

    return addJNILocalRef(newObject(c));
}

jobject JNICALL Kayo_NewObject(JNIEnv *env, jclass clazz, jmethodID methodID, ...)
{
    // todo
    va_list args;
    va_start(args, methodID);
    jobject o = env->NewObjectV(clazz, methodID, args);
    va_end(args);
    return o;
}

jobject JNICALL Kayo_NewObjectV(JNIEnv *env, jclass clazz, jmethodID methodID, va_list args)
{
    // todo
    jobject jo = env->AllocObject(clazz);
    if (jo == nullptr) {
        // todo error
    }

    slot_t *ret = execConstructor(to_method(methodID), to_object_ref(jo), args);
    return to_jobject(RSLOT(ret));
}

jobject JNICALL Kayo_NewObjectA(JNIEnv *env, jclass clazz, jmethodID methodID, const jvalue *args)
{
    // todo
    jobject jo = env->AllocObject(clazz);
    if (jo == nullptr) {
        // todo error
    }

    slot_t *ret = execConstructor(to_method(methodID), to_object_ref(jo), args);
    return to_jobject(RSLOT(ret));
}

jclass JNICALL Kayo_GetObjectClass(JNIEnv *env, jobject obj)
{
    Object *o = to_object_ref(obj);
    return (jclass) addJNILocalRef(o->clazz);
}

jboolean JNICALL Kayo_IsInstanceOf(JNIEnv *env, jobject obj, jclass clazz)
{
    auto o = to_object_ref(obj);
    auto c = to_object_ref<Class>(clazz);
    return o->isInstanceOf(c) ? JNI_TRUE : JNI_FALSE;
}

jmethodID JNICALL Kayo_GetMethodID(JNIEnv *env, jclass clazz, const char *name, const char *sig)
{
    return getMethodID(env, clazz, name, sig, false);
}


//static jobject getJobjectRetValue(const slot_t *ret) { return addJNILocalRef(RSLOT(ret)); }
//
//template <typename RetType, RetType (*getRetValue)(const slot_t *) = nullptr>
//RetType JNICALL Kayo_CallMethodA(JNIEnv *env, jobject obj, jmethodID methodID, const jvalue *args)
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
//RetType JNICALL Kayo_CallMethodV(JNIEnv *env, jobject obj, jmethodID methodID, va_list args)
//{
//    jvalue values[METHOD_PARAMETERS_MAX_COUNT];
//    parse_args_va_list(to_method(methodID)->signature, args, values);
//    if (getRetValue != nullptr)
//        return Kayo_CallMethodA<RetType, getRetValue>(env, obj, methodID, values);
//    else
//        Kayo_CallMethodA<RetType, getRetValue>(env, obj, methodID, values);
//}
//
//template <typename RetType, RetType (*getRetValue)(const slot_t *) = nullptr>
//RetType JNICALL Kayo_CallMethod(JNIEnv *env, jobject obj, jmethodID methodID, ...)
//{
//    va_list args;
//    va_start(args, methodID);
//    auto o = Kayo_CallMethodV<RetType, getRetValue>(env, obj, methodID, args);
//    va_end(args);
//    return o;
//}
//
//void JNICALL Kayo_CallMethod(JNIEnv *env, jobject obj, jmethodID methodID, ...)
//{
//    va_list args;
//    va_start(args, methodID);
//    Kayo_CallMethodV<void>(env, obj, methodID, args);
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


jfieldID JNICALL Kayo_GetFieldID(JNIEnv *env, jclass clazz, const char *name, const char *sig)
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

jobject JNICALL Kayo_GetObjectField(JNIEnv *env, jobject obj, jfieldID fieldID)
{
    Object *o = to_object_ref(obj);
    Field *f = to_field(fieldID);
    return addJNILocalRef(o->getInstFieldValue<jref>(f));
}

jboolean JNICALL Kayo_GetBooleanField(JNIEnv *env, jobject obj, jfieldID fieldID)
{
    return getInstFieldValue<jboolean>(obj, fieldID);
}

jbyte JNICALL Kayo_GetByteField(JNIEnv *env, jobject obj, jfieldID fieldID)
{
    return getInstFieldValue<jbyte>(obj, fieldID);
}

jchar JNICALL Kayo_GetCharField(JNIEnv *env, jobject obj, jfieldID fieldID)
{
    return getInstFieldValue<jchar>(obj, fieldID);
}

jshort JNICALL Kayo_GetShortField(JNIEnv *env, jobject obj, jfieldID fieldID)
{
    return getInstFieldValue<jshort>(obj, fieldID);
}

jint JNICALL Kayo_GetIntField(JNIEnv *env, jobject obj, jfieldID fieldID)
{
    return getInstFieldValue<jint>(obj, fieldID);
}

jlong JNICALL Kayo_GetLongField(JNIEnv *env, jobject obj, jfieldID fieldID)
{
    return getInstFieldValue<jlong>(obj, fieldID);
}

jfloat JNICALL Kayo_GetFloatField(JNIEnv *env, jobject obj, jfieldID fieldID)
{
    return getInstFieldValue<jfloat>(obj, fieldID);
}

jdouble JNICALL Kayo_GetDoubleField(JNIEnv *env, jobject obj, jfieldID fieldID)
{
    return getInstFieldValue<jdouble>(obj, fieldID);
}

void JNICALL Kayo_SetObjectField(JNIEnv *env, jobject obj, jfieldID fieldID, jobject val)
{
    setInstFieldValue(obj, fieldID, to_object_ref(val));
}

void JNICALL Kayo_SetBooleanField(JNIEnv *env, jobject obj, jfieldID fieldID, jboolean val)
{
    setInstFieldValue(obj, fieldID, val);
}

void JNICALL Kayo_SetByteField(JNIEnv *env, jobject obj, jfieldID fieldID, jbyte val)
{
    setInstFieldValue(obj, fieldID, val);
}

void JNICALL Kayo_SetCharField(JNIEnv *env, jobject obj, jfieldID fieldID, jchar val)
{
    setInstFieldValue(obj, fieldID, val);
}

void JNICALL Kayo_SetShortField(JNIEnv *env, jobject obj, jfieldID fieldID, jshort val)
{
    setInstFieldValue(obj, fieldID, val);
}

void JNICALL Kayo_SetIntField(JNIEnv *env, jobject obj, jfieldID fieldID, jint val)
{
    setInstFieldValue(obj, fieldID, val);
}

void JNICALL Kayo_SetLongField(JNIEnv *env, jobject obj, jfieldID fieldID, jlong val)
{
    setInstFieldValue(obj, fieldID, val);
}

void JNICALL Kayo_SetFloatField(JNIEnv *env, jobject obj, jfieldID fieldID, jfloat val)
{
    setInstFieldValue(obj, fieldID, val);
}

void JNICALL Kayo_SetDoubleField(JNIEnv *env, jobject obj, jfieldID fieldID, jdouble val)
{
    setInstFieldValue(obj, fieldID, val);
}

jmethodID JNICALL Kayo_GetStaticMethodID(JNIEnv *env, jclass clazz, const char *name, const char *sig)
{
    assert(env != nullptr && clazz != nullptr && name != nullptr && sig != nullptr);
    return getMethodID(env, clazz, name, sig, true);
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

jfieldID JNICALL Kayo_GetStaticFieldID(JNIEnv *env, jclass clazz, const char *name, const char *sig)
{
    assert(env != nullptr && clazz != nullptr && name != nullptr && sig != nullptr);
    return getFieldID(env, clazz, name, sig, true);
}

jobject JNICALL Kayo_GetStaticObjectField(JNIEnv *env, jclass clazz, jfieldID fieldID)
{
    assert(env != nullptr && clazz != nullptr && fieldID != nullptr);
    return addJNILocalRef(to_field(fieldID)->staticValue.r);
}

jboolean JNICALL Kayo_GetStaticBooleanField(JNIEnv *env, jclass clazz, jfieldID fieldID)
{
    assert(env != nullptr && clazz != nullptr && fieldID != nullptr);
    return to_field(fieldID)->staticValue.z;
}

jbyte JNICALL Kayo_GetStaticByteField(JNIEnv *env, jclass clazz, jfieldID fieldID)
{
    assert(env != nullptr && clazz != nullptr && fieldID != nullptr);
    return to_field(fieldID)->staticValue.b;
}

jchar JNICALL Kayo_GetStaticCharField(JNIEnv *env, jclass clazz, jfieldID fieldID)
{
    assert(env != nullptr && clazz != nullptr && fieldID != nullptr);
    return to_field(fieldID)->staticValue.c;
}

jshort JNICALL Kayo_GetStaticShortField(JNIEnv *env, jclass clazz, jfieldID fieldID)
{
    assert(env != nullptr && clazz != nullptr && fieldID != nullptr);
    return to_field(fieldID)->staticValue.s;
}

jint JNICALL Kayo_GetStaticIntField(JNIEnv *env, jclass clazz, jfieldID fieldID)
{
    assert(env != nullptr && clazz != nullptr && fieldID != nullptr);
    return to_field(fieldID)->staticValue.i;
}

jlong JNICALL Kayo_GetStaticLongField(JNIEnv *env, jclass clazz, jfieldID fieldID)
{
    assert(env != nullptr && clazz != nullptr && fieldID != nullptr);
    return to_field(fieldID)->staticValue.j;
}

jfloat JNICALL Kayo_GetStaticFloatField(JNIEnv *env, jclass clazz, jfieldID fieldID)
{
    assert(env != nullptr && clazz != nullptr && fieldID != nullptr);
    return to_field(fieldID)->staticValue.f;
}

jdouble JNICALL Kayo_GetStaticDoubleField(JNIEnv *env, jclass clazz, jfieldID fieldID)
{
    assert(env != nullptr && clazz != nullptr && fieldID != nullptr);
    return to_field(fieldID)->staticValue.d;
}

void JNICALL Kayo_SetStaticObjectField(JNIEnv *env, jclass clazz, jfieldID fieldID, jobject value)
{
    assert(env != nullptr && clazz != nullptr && fieldID != nullptr);
    to_field(fieldID)->staticValue.r = to_object_ref(value);
}

void JNICALL Kayo_SetStaticBooleanField(JNIEnv *env, jclass clazz, jfieldID fieldID, jboolean value)
{
    assert(env != nullptr && clazz != nullptr && fieldID != nullptr);
    to_field(fieldID)->staticValue.z = value;
}

void JNICALL Kayo_SetStaticByteField(JNIEnv *env, jclass clazz, jfieldID fieldID, jbyte value)
{
    assert(env != nullptr && clazz != nullptr && fieldID != nullptr);
    to_field(fieldID)->staticValue.b = value;
}

void JNICALL Kayo_SetStaticCharField(JNIEnv *env, jclass clazz, jfieldID fieldID, jchar value)
{
    assert(env != nullptr && clazz != nullptr && fieldID != nullptr);
    to_field(fieldID)->staticValue.c = value;
}

void JNICALL Kayo_SetStaticShortField(JNIEnv *env, jclass clazz, jfieldID fieldID, jshort value)
{
    assert(env != nullptr && clazz != nullptr && fieldID != nullptr);
    to_field(fieldID)->staticValue.s = value;
}

void JNICALL Kayo_SetStaticIntField(JNIEnv *env, jclass clazz, jfieldID fieldID, jint value)
{
    assert(env != nullptr && clazz != nullptr && fieldID != nullptr);
    to_field(fieldID)->staticValue.i = value;
}

void JNICALL Kayo_SetStaticLongField(JNIEnv *env, jclass clazz, jfieldID fieldID, jlong value)
{
    assert(env != nullptr && clazz != nullptr && fieldID != nullptr);
    to_field(fieldID)->staticValue.j = value;
}

void JNICALL Kayo_SetStaticFloatField(JNIEnv *env, jclass clazz, jfieldID fieldID, jfloat value)
{
    assert(env != nullptr && clazz != nullptr && fieldID != nullptr);
    to_field(fieldID)->staticValue.f = value;
}

void JNICALL Kayo_SetStaticDoubleField(JNIEnv *env, jclass clazz, jfieldID fieldID, jdouble value)
{
    assert(env != nullptr && clazz != nullptr && fieldID != nullptr);
    to_field(fieldID)->staticValue.d = value;
}

jstring JNICALL Kayo_NewString(JNIEnv *env, const jchar *unicode, jsize len)
{
    assert(env != nullptr && unicode != nullptr && len >= 0);
    jstrref str = newStringUnicode(unicode, len);
    return (jstring) addJNILocalRef(str);
}

jsize JNICALL Kayo_GetStringLength(JNIEnv *env, jstring str)
{
    assert(env != nullptr && str != nullptr);
//    jstrref s = to_object_ref(str);
    // todo
    jvm_abort("not implement.");
}

const jchar *JNICALL Kayo_GetStringChars(JNIEnv *env, jstring str, jboolean *isCopy)
{
    assert(env != nullptr && str != nullptr);
    // todo
    jvm_abort("not implement.");
}

void JNICALL Kayo_ReleaseStringChars(JNIEnv *env, jstring str, const jchar *chars)
{
    // todo
    jvm_abort("not implement.");
}

jstring JNICALL Kayo_NewStringUTF(JNIEnv *env, const char *utf)
{
    jstrref str = newString(utf);
    return (jstring) addJNILocalRef(str);
}

jsize JNICALL Kayo_GetStringUTFLength(JNIEnv *env, jstring str)
{
    // todo
    jvm_abort("not implement.");
}

const char* JNICALL Kayo_GetStringUTFChars(JNIEnv *env, jstring str, jboolean *isCopy)
{
    // todo
    jvm_abort("not implement.");
}

void JNICALL Kayo_ReleaseStringUTFChars(JNIEnv *env, jstring str, const char* chars)
{
    // todo
    jvm_abort("not implement.");
}

jsize JNICALL Kayo_GetArrayLength(JNIEnv *env, jarray array)
{
    assert(env != nullptr && array != nullptr);
    auto arr = to_object_ref<Array>(array);
    return arr->len;
}

jobjectArray JNICALL Kayo_NewObjectArray(JNIEnv *env, jsize len, jclass clazz, jobject init)
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

jobject JNICALL Kayo_GetObjectArrayElement(JNIEnv *env, jobjectArray array, jsize index)
{
    // todo
    jvm_abort("not implement.");
}

void JNICALL Kayo_SetObjectArrayElement(JNIEnv *env, jobjectArray array, jsize index, jobject val)
{
    // todo
    jvm_abort("not implement.");
}

template <typename jtypeArray, ArrayType arr_type>
jtypeArray JNICALL Kayo_NewTypeArray(JNIEnv *env, jsize len)
{
    assert(env != nullptr);
    if (len < 0) {
        /* todo java_lang_NegativeArraySizeException */
    }

    Array *arr = newTypeArray(arr_type, len);
    return (jtypeArray) addJNILocalRef(arr);
}

template <typename jtypeArray, typename raw_type>
raw_type* JNICALL Kayo_GetTypeArrayElements(JNIEnv *env, jtypeArray array, jboolean *isCopy)
{
    assert(env != nullptr && array != nullptr);
    auto arr = to_object_ref<Array>(array);
    if (isCopy != nullptr)
        *isCopy = JNI_FALSE;
    addJNIGlobalRef(arr);
    return (raw_type *) arr->data;
}

template <typename jtypeArray, typename raw_type>
void JNICALL Kayo_ReleaseTypeArrayElements(JNIEnv *env, jtypeArray array, raw_type *elems, jint mode)
{
    assert(env != nullptr);
    deleteJNIGlobalRef(to_object_ref(array));
}

template <typename jtypeArray, typename raw_type>
void JNICALL Kayo_GetTypeArrayRegion(JNIEnv *env, jtypeArray array, jsize start, jsize len, raw_type *buf)
{
    assert(env != nullptr && array != nullptr && buf != nullptr);
    auto arr = to_object_ref<Array>(array);
    assert(start + len <= arr->len);
    assert(arr->clazz->getEleSize() == sizeof(raw_type));
    memcpy(buf, arr->index(start), len*sizeof(raw_type));
}

template <typename jtypeArray, typename raw_type>
void JNICALL Kayo_SetTypeArrayRegion(JNIEnv *env, jtypeArray array, jsize start, jsize len, const raw_type *buf)
{
    assert(env != nullptr && array != nullptr && buf != nullptr);
    auto arr = to_object_ref<Array>(array);
    assert(start + len <= arr->len);
    assert(arr->clazz->getEleSize() == sizeof(raw_type));
    memcpy(arr->index(start), buf, len*sizeof(raw_type));
}

jint JNICALL Kayo_RegisterNatives(JNIEnv *env, jclass clazz, const JNINativeMethod *methods, jint nMethods)
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

jint JNICALL Kayo_UnregisterNatives(JNIEnv *env, jclass clazz)
{
    assert(env != nullptr && clazz != nullptr);
    // todo
    jvm_abort("not implement.");
}

jint JNICALL Kayo_MonitorEnter(JNIEnv *env, jobject obj)
{
    assert(env != nullptr && obj != nullptr);
    // todo
//    jref o = to_object_ref(obj);
//    o->lock();
// return JNI_OK;
    jvm_abort("not implement.");
}

jint JNICALL Kayo_MonitorExit(JNIEnv *env, jobject obj)
{
    assert(env != nullptr && obj != nullptr);
    // todo
//    jref o = to_object_ref(obj);
//    o->unlock();
// return JNI_OK;
    jvm_abort("not implement.");
}

jint JNICALL Kayo_GetJavaVM(JNIEnv *env, JavaVM **vm)
{
    // todo
    jvm_abort("not implement.");
}

void JNICALL Kayo_GetStringRegion(JNIEnv *env, jstring str, jsize start, jsize len, jchar *buf)
{
    // todo
    jvm_abort("not implement.");
}

void JNICALL Kayo_GetStringUTFRegion(JNIEnv *env, jstring str, jsize start, jsize len, char *buf)
{
    // todo
    jvm_abort("not implement.");
}

void* JNICALL Kayo_GetPrimitiveArrayCritical(JNIEnv *env, jarray array, jboolean *isCopy)
{
    assert(env != nullptr && array != nullptr);
    auto arr = to_object_ref<Array>(array);

    if(isCopy != nullptr)
        *isCopy = JNI_FALSE;

    /* Pin the array */ //  todo
    addJNIGlobalRef(arr);
    return arr->data;
}

void JNICALL Kayo_ReleasePrimitiveArrayCritical(JNIEnv *env, jarray array, void *carray, jint mode)
{
    assert(env != nullptr && array != nullptr);
    deleteJNIGlobalRef(to_object_ref<Array>(array));
}

const jchar * JNICALL Kayo_GetStringCritical(JNIEnv *env, jstring string, jboolean *isCopy)
{
    // todo
    jvm_abort("not implement.");
}

void JNICALL Kayo_ReleaseStringCritical(JNIEnv *env, jstring string, const jchar *cstring)
{
    // todo
    jvm_abort("not implement.");
}

jweak JNICALL Kayo_NewWeakGlobalRef(JNIEnv *env, jobject obj)
{
    assert(env != nullptr && obj != nullptr);
    return addJNIWeakGlobalRef(to_object_ref(obj));
}

void JNICALL Kayo_DeleteWeakGlobalRef(JNIEnv *env, jweak ref)
{
    assert(env != nullptr && ref != nullptr);
    deleteJNIWeakGlobalRef(to_object_ref(ref));
}

jboolean JNICALL Kayo_ExceptionCheck(JNIEnv *env)
{
    // todo
    jvm_abort("not implement.");
}

jobject JNICALL Kayo_NewDirectByteBuffer(JNIEnv *env, void *address, jlong capacity)
{
    // todo
    jvm_abort("not implement.");
}

void* JNICALL Kayo_GetDirectBufferAddress(JNIEnv *env, jobject buf)
{
    // todo
    jvm_abort("not implement.");
}

jlong JNICALL Kayo_GetDirectBufferCapacity(JNIEnv *env, jobject buf)
{
    // todo
    jvm_abort("not implement.");
}

jobjectRefType JNICALL Kayo_GetObjectRefType(JNIEnv* env, jobject obj)
{
    jref o = to_object_ref(obj);
    return o != nullptr ? o->jni_obj_ref_type : JNIInvalidRefType;
}

jobject JNICALL Kayo_GetModule(JNIEnv* env, jclass clazz)
{
    // todo
    jvm_abort("not implement.");
}


static struct JNINativeInterface_ Kayo_JNINativeInterface = {
    .reserved0 = nullptr,
    .reserved1 = nullptr,
    .reserved2 = nullptr,
    .reserved3 = nullptr,

    .GetVersion = Kayo_GetVersion,

    .DefineClass = Kayo_DefineClass,
    .FindClass = Kayo_FindClass,

    .FromReflectedMethod = Kayo_FromReflectedMethod,
    .FromReflectedField = Kayo_FromReflectedField,

    .ToReflectedMethod = Kayo_ToReflectedMethod,

    .GetSuperclass = Kayo_GetSuperclass,
    .IsAssignableFrom = Kayo_IsAssignableFrom,

    .ToReflectedField = Kayo_ToReflectedField,

    .Throw = Kayo_Throw,
    .ThrowNew = Kayo_ThrowNew,
    .ExceptionOccurred = Kayo_ExceptionOccurred,
    .ExceptionDescribe = Kayo_ExceptionDescribe,
    .ExceptionClear = Kayo_ExceptionClear,
    .FatalError = Kayo_FatalError,

    .PushLocalFrame = Kayo_PushLocalFrame,
    .PopLocalFrame = Kayo_PopLocalFrame,

    .NewGlobalRef = Kayo_NewGlobalRef,
    .DeleteGlobalRef = Kayo_DeleteGlobalRef,
    .DeleteLocalRef = Kayo_DeleteLocalRef,
    .IsSameObject = Kayo_IsSameObject,
    .NewLocalRef = Kayo_NewLocalRef,
    .EnsureLocalCapacity = Kayo_EnsureLocalCapacity,

    .AllocObject = Kayo_AllocObject,
    .NewObject = Kayo_NewObject,
    .NewObjectV = Kayo_NewObjectV,
    .NewObjectA = Kayo_NewObjectA,

    .GetObjectClass = Kayo_GetObjectClass,
    .IsInstanceOf = Kayo_IsInstanceOf,

    .GetMethodID = Kayo_GetMethodID,

//    .CallObjectMethod = Kayo_CallMethod<jobject, getJobjectRetValue>,
//    .CallObjectMethodV = Kayo_CallMethodV<jobject, getJobjectRetValue>,
//    .CallObjectMethodA = Kayo_CallMethodA<jobject, getJobjectRetValue>,
    .CallObjectMethod = Kayo_CallObjectMethod,
    .CallObjectMethodV = Kayo_CallObjectMethodV,
    .CallObjectMethodA = Kayo_CallObjectMethodA,

    .CallBooleanMethod = Kayo_CallBooleanMethod,
    .CallBooleanMethodV = Kayo_CallBooleanMethodV,
    .CallBooleanMethodA = Kayo_CallBooleanMethodA,

    .CallByteMethod = Kayo_CallByteMethod,
    .CallByteMethodV = Kayo_CallByteMethodV,
    .CallByteMethodA = Kayo_CallByteMethodA,

    .CallCharMethod = Kayo_CallCharMethod,
    .CallCharMethodV = Kayo_CallCharMethodV,
    .CallCharMethodA = Kayo_CallCharMethodA,

    .CallShortMethod = Kayo_CallShortMethod,
    .CallShortMethodV = Kayo_CallShortMethodV,
    .CallShortMethodA = Kayo_CallShortMethodA,

    .CallIntMethod = Kayo_CallIntMethod,
    .CallIntMethodV = Kayo_CallIntMethodV,
    .CallIntMethodA = Kayo_CallIntMethodA,

    .CallLongMethod = Kayo_CallLongMethod,
    .CallLongMethodV = Kayo_CallLongMethodV,
    .CallLongMethodA = Kayo_CallLongMethodA,

    .CallFloatMethod = Kayo_CallFloatMethod,
    .CallFloatMethodV = Kayo_CallFloatMethodV,
    .CallFloatMethodA = Kayo_CallFloatMethodA,

    .CallDoubleMethod = Kayo_CallDoubleMethod,
    .CallDoubleMethodV = Kayo_CallDoubleMethodV,
    .CallDoubleMethodA = Kayo_CallDoubleMethodA,

    .CallVoidMethod = Kayo_CallVoidMethod,
    .CallVoidMethodV = Kayo_CallVoidMethodV,
    .CallVoidMethodA = Kayo_CallVoidMethodA,

    .CallNonvirtualObjectMethod = Kayo_CallNonvirtualObjectMethod,
    .CallNonvirtualObjectMethodV = Kayo_CallNonvirtualObjectMethodV,
    .CallNonvirtualObjectMethodA = Kayo_CallNonvirtualObjectMethodA,

    .CallNonvirtualBooleanMethod = Kayo_CallNonvirtualBooleanMethod,
    .CallNonvirtualBooleanMethodV = Kayo_CallNonvirtualBooleanMethodV,
    .CallNonvirtualBooleanMethodA = Kayo_CallNonvirtualBooleanMethodA,

    .CallNonvirtualByteMethod = Kayo_CallNonvirtualByteMethod,
    .CallNonvirtualByteMethodV = Kayo_CallNonvirtualByteMethodV,
    .CallNonvirtualByteMethodA = Kayo_CallNonvirtualByteMethodA,

    .CallNonvirtualCharMethod = Kayo_CallNonvirtualCharMethod,
    .CallNonvirtualCharMethodV = Kayo_CallNonvirtualCharMethodV,
    .CallNonvirtualCharMethodA = Kayo_CallNonvirtualCharMethodA,

    .CallNonvirtualShortMethod = Kayo_CallNonvirtualShortMethod,
    .CallNonvirtualShortMethodV = Kayo_CallNonvirtualShortMethodV,
    .CallNonvirtualShortMethodA = Kayo_CallNonvirtualShortMethodA,

    .CallNonvirtualIntMethod = Kayo_CallNonvirtualIntMethod,
    .CallNonvirtualIntMethodV = Kayo_CallNonvirtualIntMethodV,
    .CallNonvirtualIntMethodA = Kayo_CallNonvirtualIntMethodA,

    .CallNonvirtualLongMethod = Kayo_CallNonvirtualLongMethod,
    .CallNonvirtualLongMethodV = Kayo_CallNonvirtualLongMethodV,
    .CallNonvirtualLongMethodA = Kayo_CallNonvirtualLongMethodA,

    .CallNonvirtualFloatMethod = Kayo_CallNonvirtualFloatMethod,
    .CallNonvirtualFloatMethodV = Kayo_CallNonvirtualFloatMethodV,
    .CallNonvirtualFloatMethodA = Kayo_CallNonvirtualFloatMethodA,

    .CallNonvirtualDoubleMethod = Kayo_CallNonvirtualDoubleMethod,
    .CallNonvirtualDoubleMethodV = Kayo_CallNonvirtualDoubleMethodV,
    .CallNonvirtualDoubleMethodA = Kayo_CallNonvirtualDoubleMethodA,

    .CallNonvirtualVoidMethod = Kayo_CallNonvirtualVoidMethod,
    .CallNonvirtualVoidMethodV = Kayo_CallNonvirtualVoidMethodV,
    .CallNonvirtualVoidMethodA = Kayo_CallNonvirtualVoidMethodA,

    .GetFieldID = Kayo_GetFieldID,

    .GetObjectField = Kayo_GetObjectField,
    .GetBooleanField = Kayo_GetBooleanField,
    .GetByteField = Kayo_GetByteField,
    .GetCharField = Kayo_GetCharField,
    .GetShortField = Kayo_GetShortField,
    .GetIntField = Kayo_GetIntField,
    .GetLongField = Kayo_GetLongField,
    .GetFloatField = Kayo_GetFloatField,
    .GetDoubleField = Kayo_GetDoubleField,

    .SetObjectField = Kayo_SetObjectField,
    .SetBooleanField = Kayo_SetBooleanField,
    .SetByteField = Kayo_SetByteField,
    .SetCharField = Kayo_SetCharField,
    .SetShortField = Kayo_SetShortField,
    .SetIntField = Kayo_SetIntField,
    .SetLongField = Kayo_SetLongField,
    .SetFloatField = Kayo_SetFloatField,
    .SetDoubleField = Kayo_SetDoubleField,

    .GetStaticMethodID = Kayo_GetStaticMethodID,

    .CallStaticObjectMethod = Kayo_CallStaticObjectMethod,
    .CallStaticObjectMethodV = Kayo_CallStaticObjectMethodV,
    .CallStaticObjectMethodA = Kayo_CallStaticObjectMethodA,

    .CallStaticBooleanMethod = Kayo_CallStaticBooleanMethod,
    .CallStaticBooleanMethodV = Kayo_CallStaticBooleanMethodV,
    .CallStaticBooleanMethodA = Kayo_CallStaticBooleanMethodA,

    .CallStaticByteMethod = Kayo_CallStaticByteMethod,
    .CallStaticByteMethodV = Kayo_CallStaticByteMethodV,
    .CallStaticByteMethodA = Kayo_CallStaticByteMethodA,

    .CallStaticCharMethod = Kayo_CallStaticCharMethod,
    .CallStaticCharMethodV = Kayo_CallStaticCharMethodV,
    .CallStaticCharMethodA = Kayo_CallStaticCharMethodA,

    .CallStaticShortMethod = Kayo_CallStaticShortMethod,
    .CallStaticShortMethodV = Kayo_CallStaticShortMethodV,
    .CallStaticShortMethodA = Kayo_CallStaticShortMethodA,

    .CallStaticIntMethod = Kayo_CallStaticIntMethod,
    .CallStaticIntMethodV = Kayo_CallStaticIntMethodV,
    .CallStaticIntMethodA = Kayo_CallStaticIntMethodA,

    .CallStaticLongMethod = Kayo_CallStaticLongMethod,
    .CallStaticLongMethodV = Kayo_CallStaticLongMethodV,
    .CallStaticLongMethodA = Kayo_CallStaticLongMethodA,

    .CallStaticFloatMethod = Kayo_CallStaticFloatMethod,
    .CallStaticFloatMethodV = Kayo_CallStaticFloatMethodV,
    .CallStaticFloatMethodA = Kayo_CallStaticFloatMethodA,

    .CallStaticDoubleMethod = Kayo_CallStaticDoubleMethod,
    .CallStaticDoubleMethodV = Kayo_CallStaticDoubleMethodV,
    .CallStaticDoubleMethodA = Kayo_CallStaticDoubleMethodA,

    .CallStaticVoidMethod = Kayo_CallStaticVoidMethod,
    .CallStaticVoidMethodV = Kayo_CallStaticVoidMethodV,
    .CallStaticVoidMethodA = Kayo_CallStaticVoidMethodA,

    .GetStaticFieldID = Kayo_GetStaticFieldID,
    .GetStaticObjectField = Kayo_GetStaticObjectField,
    .GetStaticBooleanField = Kayo_GetStaticBooleanField,
    .GetStaticByteField = Kayo_GetStaticByteField,
    .GetStaticCharField = Kayo_GetStaticCharField,
    .GetStaticShortField = Kayo_GetStaticShortField,
    .GetStaticIntField = Kayo_GetStaticIntField,
    .GetStaticLongField = Kayo_GetStaticLongField,
    .GetStaticFloatField = Kayo_GetStaticFloatField,
    .GetStaticDoubleField = Kayo_GetStaticDoubleField,

    .SetStaticObjectField = Kayo_SetStaticObjectField,
    .SetStaticBooleanField = Kayo_SetStaticBooleanField,
    .SetStaticByteField = Kayo_SetStaticByteField,
    .SetStaticCharField = Kayo_SetStaticCharField,
    .SetStaticShortField = Kayo_SetStaticShortField,
    .SetStaticIntField = Kayo_SetStaticIntField,
    .SetStaticLongField = Kayo_SetStaticLongField,
    .SetStaticFloatField = Kayo_SetStaticFloatField,
    .SetStaticDoubleField = Kayo_SetStaticDoubleField,

    .NewString = Kayo_NewString,
    .GetStringLength = Kayo_GetStringLength,
    .GetStringChars = Kayo_GetStringChars,
    .ReleaseStringChars = Kayo_ReleaseStringChars,

    .NewStringUTF = Kayo_NewStringUTF,
    .GetStringUTFLength = Kayo_GetStringUTFLength,
    .GetStringUTFChars = Kayo_GetStringUTFChars,
    .ReleaseStringUTFChars = Kayo_ReleaseStringUTFChars,

    .GetArrayLength = Kayo_GetArrayLength,

    .NewObjectArray = Kayo_NewObjectArray,
    .GetObjectArrayElement = Kayo_GetObjectArrayElement,
    .SetObjectArrayElement = Kayo_SetObjectArrayElement,
    .NewBooleanArray = Kayo_NewTypeArray<jbooleanArray, JVM_AT_BOOLEAN>,
    .NewByteArray = Kayo_NewTypeArray<jbyteArray, JVM_AT_BYTE>,
    .NewCharArray = Kayo_NewTypeArray<jcharArray, JVM_AT_CHAR>,
    .NewShortArray = Kayo_NewTypeArray<jshortArray, JVM_AT_SHORT>,
    .NewIntArray = Kayo_NewTypeArray<jintArray, JVM_AT_INT>,
    .NewLongArray = Kayo_NewTypeArray<jlongArray, JVM_AT_LONG>,
    .NewFloatArray = Kayo_NewTypeArray<jfloatArray, JVM_AT_FLOAT>,
    .NewDoubleArray = Kayo_NewTypeArray<jdoubleArray, JVM_AT_DOUBLE>,

    .GetBooleanArrayElements = Kayo_GetTypeArrayElements<jbooleanArray, jboolean>,
    .GetByteArrayElements = Kayo_GetTypeArrayElements<jbyteArray, jbyte>,
    .GetCharArrayElements = Kayo_GetTypeArrayElements<jcharArray, jchar>,
    .GetShortArrayElements = Kayo_GetTypeArrayElements<jshortArray, jshort>,
    .GetIntArrayElements = Kayo_GetTypeArrayElements<jintArray, jint>,
    .GetLongArrayElements = Kayo_GetTypeArrayElements<jlongArray, jlong>,
    .GetFloatArrayElements = Kayo_GetTypeArrayElements<jfloatArray, jfloat>,
    .GetDoubleArrayElements = Kayo_GetTypeArrayElements<jdoubleArray, jdouble>,

    .ReleaseBooleanArrayElements = Kayo_ReleaseTypeArrayElements<jbooleanArray, jboolean>,
    .ReleaseByteArrayElements = Kayo_ReleaseTypeArrayElements<jbyteArray, jbyte>,
    .ReleaseCharArrayElements = Kayo_ReleaseTypeArrayElements<jcharArray, jchar>,
    .ReleaseShortArrayElements = Kayo_ReleaseTypeArrayElements<jshortArray, jshort>,
    .ReleaseIntArrayElements = Kayo_ReleaseTypeArrayElements<jintArray, jint>,
    .ReleaseLongArrayElements = Kayo_ReleaseTypeArrayElements<jlongArray, jlong>,
    .ReleaseFloatArrayElements = Kayo_ReleaseTypeArrayElements<jfloatArray, jfloat>,
    .ReleaseDoubleArrayElements = Kayo_ReleaseTypeArrayElements<jdoubleArray, jdouble>,

    .GetBooleanArrayRegion = Kayo_GetTypeArrayRegion<jbooleanArray, jboolean>,
    .GetByteArrayRegion = Kayo_GetTypeArrayRegion<jbyteArray, jbyte>,
    .GetCharArrayRegion = Kayo_GetTypeArrayRegion<jcharArray, jchar>,
    .GetShortArrayRegion = Kayo_GetTypeArrayRegion<jshortArray, jshort>,
    .GetIntArrayRegion = Kayo_GetTypeArrayRegion<jintArray, jint>,
    .GetLongArrayRegion = Kayo_GetTypeArrayRegion<jlongArray, jlong>,
    .GetFloatArrayRegion = Kayo_GetTypeArrayRegion<jfloatArray, jfloat>,
    .GetDoubleArrayRegion = Kayo_GetTypeArrayRegion<jdoubleArray, jdouble>,

    .SetBooleanArrayRegion = Kayo_SetTypeArrayRegion<jbooleanArray, jboolean>,
    .SetByteArrayRegion = Kayo_SetTypeArrayRegion<jbyteArray, jbyte>,
    .SetCharArrayRegion = Kayo_SetTypeArrayRegion<jcharArray, jchar>,
    .SetShortArrayRegion = Kayo_SetTypeArrayRegion<jshortArray, jshort>,
    .SetIntArrayRegion = Kayo_SetTypeArrayRegion<jintArray, jint>,
    .SetLongArrayRegion = Kayo_SetTypeArrayRegion<jlongArray, jlong>,
    .SetFloatArrayRegion = Kayo_SetTypeArrayRegion<jfloatArray, jfloat>,
    .SetDoubleArrayRegion = Kayo_SetTypeArrayRegion<jdoubleArray, jdouble>,

    .RegisterNatives = Kayo_RegisterNatives,
    .UnregisterNatives = Kayo_UnregisterNatives,

    .MonitorEnter = Kayo_MonitorEnter,
    .MonitorExit = Kayo_MonitorExit,

    .GetJavaVM = Kayo_GetJavaVM,

    .GetStringRegion = Kayo_GetStringRegion,
    .GetStringUTFRegion = Kayo_GetStringUTFRegion,

    .GetPrimitiveArrayCritical = Kayo_GetPrimitiveArrayCritical,
    .ReleasePrimitiveArrayCritical = Kayo_ReleasePrimitiveArrayCritical,

    .GetStringCritical = Kayo_GetStringCritical,
    .ReleaseStringCritical = Kayo_ReleaseStringCritical,

    .NewWeakGlobalRef = Kayo_NewWeakGlobalRef,
    .DeleteWeakGlobalRef = Kayo_DeleteWeakGlobalRef,

    .ExceptionCheck = Kayo_ExceptionCheck,

    .NewDirectByteBuffer = Kayo_NewDirectByteBuffer,
    .GetDirectBufferAddress = Kayo_GetDirectBufferAddress,
    .GetDirectBufferCapacity = Kayo_GetDirectBufferCapacity,

    .GetObjectRefType = Kayo_GetObjectRefType,

    .GetModule = Kayo_GetModule,
};

JNIEnv_::JNIEnv_(const struct JNINativeInterface_ *functions) noexcept
{
    this->functions = functions;
}

JNIEnv g_jni_env(&Kayo_JNINativeInterface);
