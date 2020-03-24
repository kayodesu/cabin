/*
 * Author: kayo
 */

#ifndef KAYO_JNI_INTERNAL_H
#define KAYO_JNI_INTERNAL_H

#include <vector>
#include "jni.h"
#include "../objects/class.h"
#include "../objects/field.h"
#include "../objects/method.h"
#include "../objects/class_loader.h"
#include "../interpreter/interpreter.h"

template <typename T = Object>
static inline T *to_object_ref(jobject jobj)
{
    return (T *) (void *) jobj;
}

static inline jobject to_jobject(Object *o)
{
    return (jobject) (void *) o;
}

static inline jclass to_jclass(Class *c)
{
    return (jclass) (void *) c;
}

static inline Field *to_field(jfieldID fieldID)
{
    return (Field *) (void *) fieldID;
}

static inline jfieldID to_jfieldID(Field *f)
{
    return (jfieldID) (void *) f;
}

static inline Method *to_method(jmethodID methodID)
{
    return (Method *) (void *) methodID;
}

static jobject addJNILocalRef(Object *ref)
{
    // todo
    jvm_abort("not implement.");
}

static void deleteJNILocalRef(Object *ref)
{
    // todo
    jvm_abort("not implement.");
}

struct GlobalRefTable {
    std::vector<jref> table;
 //   VMLock lock;

    void lock()
    {
        // todo
        jvm_abort("not implement.");
    }

    void unlock()
    {
        // todo
        jvm_abort("not implement.");
    }
};

extern GlobalRefTable global_refs;

static jobject addJNIGlobalRef(Object *ref)
{
    // todo
    global_refs.lock();
    global_refs.table.push_back(ref);
    ref->jni_obj_ref_type = JNIGlobalRefType;
    global_refs.unlock();

    jvm_abort("not implement.");
}

static void deleteJNIGlobalRef(Object *ref)
{
    if (ref->jni_obj_ref_type != JNIGlobalRefType) {
        // todo error
    }
    global_refs.lock();
//    global_refs.table.erase(ref); // todo
    global_refs.unlock();

    jvm_abort("not implement.");
}

extern GlobalRefTable weak_global_refs;

static jweak addJNIWeakGlobalRef(Object *ref)
{
    // todo
    weak_global_refs.lock();
    weak_global_refs.table.push_back(ref);
    ref->jni_obj_ref_type = JNIWeakGlobalRefType;
    weak_global_refs.unlock();

    jvm_abort("not implement.");
}

static void deleteJNIWeakGlobalRef(Object *ref)
{
    if (ref->jni_obj_ref_type != JNIWeakGlobalRefType) {
        // todo error
    }
    weak_global_refs.lock();
//    weak_global_refs.table.erase(ref); // todo
    weak_global_refs.unlock();

    jvm_abort("not implement.");
}

/*
 * JNI helper function. The class may be invalid
 * or it may not have been initialised yet
 * */
static Class *checkClassBeforeAllocObject(Class *c)
{
    /* Check the class can be instantiated */
    if (c->isAbstract() || c->isInterface()) {
//        signalException(java_lang_InstantiationException, cb->name); todo
        return nullptr;
    }

    /* Creating an instance of a class is an active use; make sure it is initialised */
    return initClass(c);
}

static jmethodID getMethodID(JNIEnv *env,
                        jclass clazz, const char *name, const char *sig, bool is_static)
{
    // todo
    jvm_abort("not implement.");
}

static jfieldID getFieldID(JNIEnv *env,
                             jclass clazz, const char *name, const char *sig, bool is_static)
{
    // todo
    jvm_abort("not implement.");
}

template <typename T>
static T getInstFieldValue(jobject obj, jfieldID fieldID)
{
    Object *o = to_object_ref(obj);
    Field *f = to_field(fieldID);
    return o->getInstFieldValue<T>(f);
}

#define setInstFieldValue(obj, fieldID, value) \
    do { \
        Object *o = to_object_ref(obj); \
        Field *f = to_field(fieldID); \
        o->setFieldValue(f, value); \
    } while(false)

/*
 * 定义 Call_T_Method 镞函数：
 *
 * T JNICALL Kayo_Call_T_Method(JNIEnv *env,
 *                              jobject obj, jmethodID methodID, ...);
 * T JNICALL Kayo_Call_T_MethodV(JNIEnv *env,
 *                              jobject obj, jmethodID methodID, va_list args);
 * T JNICALL Kayo_Call_T_MethodA(JNIEnv *env,
 *                              jobject obj, jmethodID methodID, const jvalue *args);
 */

#define DEFINE_CALL_T_METHOD_BODY \
    jref _this = to_object_ref(obj); \
    Method *m0 = to_method(methodID); \
    Method *m = _this->clazz->lookupMethod(m0->name, m0->signature); \
    if (m == nullptr) { \
        /* todo error */ \
    } \
    slot_t *ret = execJavaFunc(m, _this, args);

#define DEFINE_CALL_T_METHOD(T, ret_type, ret_value)  \
ret_type JNICALL Kayo_Call##T##Method(JNIEnv *env, jobject obj, jmethodID methodID, ...) \
{ \
    va_list args; \
    va_start(args, methodID); \
    DEFINE_CALL_T_METHOD_BODY \
    va_end(args); \
    return ret_value; \
}

#define DEFINE_CALL_T_METHOD_V(T, ret_type, ret_value) \
ret_type JNICALL Kayo_Call##T##MethodV(JNIEnv *env, jobject obj, jmethodID methodID, va_list args) \
{ \
    DEFINE_CALL_T_METHOD_BODY \
    return ret_value; \
}

#define DEFINE_CALL_T_METHOD_A(T, ret_type, ret_value) \
ret_type JNICALL Kayo_Call##T##MethodA(JNIEnv *env, \
                                       jobject obj, jmethodID methodID, const jvalue *args) \
{ \
    DEFINE_CALL_T_METHOD_BODY \
    return ret_value; \
}

#define DEFINE_3_CALL_T_METHODS(T, ret_type, ret_value) \
    DEFINE_CALL_T_METHOD(T, ret_type, ret_value) \
    DEFINE_CALL_T_METHOD_V(T, ret_type, ret_value) \
    DEFINE_CALL_T_METHOD_A(T, ret_type, ret_value)

/*
 * 定义 CallNonvirtual_T_Method 镞函数：
 *
 * T JNICALL Kayo_CallNonvirtual_T_Method(JNIEnv *env,
 *                              jobject obj, jclass clazz, jmethodID methodID, ...);
 * T JNICALL Kayo_CallNonvirtual_T_MethodV(JNIEnv *env,
 *                              jobject obj, jclass clazz, jmethodID methodID, va_list args);
 * T JNICALL Kayo_CallNonvirtual_T_MethodA(JNIEnv *env,
 *                              jobject obj, jclass clazz, jmethodID methodID, const jvalue *args);
 */

#define DEFINE_CALL_NONVIRTUAL_T_METHOD(T, ret_type, ret_value)  \
ret_type JNICALL Kayo_CallNonvirtual##T##Method(JNIEnv *env, jobject obj, jclass clazz, jmethodID methodID, ...) \
{ \
    va_list args; \
    va_start(args, methodID); \
    slot_t *ret = execJavaFunc( \
                    to_method(methodID), to_object_ref(obj), to_object_ref<Class>(clazz), args); \
    va_end(args); \
    return ret_value; \
}

#define DEFINE_CALL_NONVIRTUAL_T_METHOD_V(T, ret_type, ret_value) \
ret_type JNICALL Kayo_CallNonvirtual##T##MethodV(JNIEnv *env, jobject obj, \
                            jclass clazz, jmethodID methodID, va_list args) \
{ \
    slot_t *ret = execJavaFunc( \
                    to_method(methodID), to_object_ref(obj), to_object_ref<Class>(clazz), args); \
    return ret_value; \
}

#define DEFINE_CALL_NONVIRTUAL_T_METHOD_A(T, ret_type, ret_value) \
ret_type JNICALL Kayo_CallNonvirtual##T##MethodA(JNIEnv *env, jobject obj, \
                            jclass clazz, jmethodID methodID, const jvalue *args) \
{ \
    slot_t *ret = execJavaFunc( \
                    to_method(methodID), to_object_ref(obj), to_object_ref<Class>(clazz), args); \
    return ret_value; \
}

#define DEFINE_3_CALL_NONVIRTUAL_T_METHODS(T, ret_type, ret_value) \
    DEFINE_CALL_NONVIRTUAL_T_METHOD_A(T, ret_type, ret_value) \
    DEFINE_CALL_NONVIRTUAL_T_METHOD_V(T, ret_type, ret_value) \
    DEFINE_CALL_NONVIRTUAL_T_METHOD(T, ret_type, ret_value)

/*
* 定义 CallStatic_T_Method 镞函数：
*
* T JNICALL Kayo_CallStatic_T_Method(JNIEnv *env,
*                              jobject obj, jclass clazz, jmethodID methodID, ...);
* T JNICALL Kayo_CallStatic_T_MethodV(JNIEnv *env,
*                              jobject obj, jclass clazz, jmethodID methodID, va_list args);
* T JNICALL Kayo_CallStatic_T_MethodA(JNIEnv *env,
*                              jobject obj, jclass clazz, jmethodID methodID, const jvalue *args);
*/

#define DEFINE_CALL_STATIC_T_METHOD(T, ret_type, ret_value)  \
ret_type JNICALL Kayo_CallStatic##T##Method(JNIEnv *env, jclass clazz, jmethodID methodID, ...) \
{ \
    va_list args; \
    va_start(args, methodID); \
    slot_t *ret = execJavaFunc(to_method(methodID), \
                        to_object_ref<Class>(clazz), to_object_ref<Class>(clazz), args); \
    va_end(args); \
    return ret_value; \
}

#define DEFINE_CALL_STATIC_T_METHOD_V(T, ret_type, ret_value) \
ret_type JNICALL Kayo_CallStatic##T##MethodV(JNIEnv *env, \
                            jclass clazz, jmethodID methodID, va_list args) \
{ \
    slot_t *ret = execJavaFunc(to_method(methodID), \
                        to_object_ref<Class>(clazz), to_object_ref<Class>(clazz), args); \
    return ret_value; \
}

#define DEFINE_CALL_STATIC_T_METHOD_A(T, ret_type, ret_value) \
ret_type JNICALL Kayo_CallStatic##T##MethodA(JNIEnv *env, \
                            jclass clazz, jmethodID methodID, const jvalue *args) \
{ \
    slot_t *ret = execJavaFunc(to_method(methodID), \
                        to_object_ref<Class>(clazz), to_object_ref<Class>(clazz), args); \
    return ret_value; \
}

#define DEFINE_3_CALL_STATIC_T_METHODS(T, ret_type, ret_value) \
    DEFINE_CALL_STATIC_T_METHOD(T, ret_type, ret_value) \
    DEFINE_CALL_STATIC_T_METHOD_V(T, ret_type, ret_value) \
    DEFINE_CALL_STATIC_T_METHOD_A(T, ret_type, ret_value)

#endif //KAYO_JNI_INTERNAL_H
