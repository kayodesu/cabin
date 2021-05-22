#include "cabin.h"
#include "jni.h"


#define JVM_MIRROR(_jclass) ((jclsRef) _jclass)->jvm_mirror

static jobject addJNILocalRef(Object *ref)
{
    // todo 
    return (jobject) ref;
}

static jobject addJNIGlobalRef(Object *ref)
{
    // todo 
    return (jobject) ref;
}

static void deleteJNIGlobalRef(Object *ref)
{
    // todo
}

static jweak addJNIWeakGlobalRef(Object *ref)
{
    JVM_PANIC("not implement.");
}

static void deleteJNIWeakGlobalRef(Object *ref)
{
    JVM_PANIC("not implement.");
}

static slot_t *execJavaV(Method *m, jref this, va_list args)
{
    // Class[]
    jarrRef types = get_parameter_types(m);
    jsize args_count = types->arr_len;
    
    // 因为有 category two 的存在，result 的长度最大为 types_len * 2 + this_obj
    slot_t *real_args = vm_malloc(sizeof(slot_t)*(2*args_count + 1));
    int k = 0;
    if (this != NULL) {
        assert(!IS_STATIC(m));
        slot_set_ref(real_args, this);
        k++;
    }

    for (int i = 0; i < args_count; i++, k++) {
        Class *c = array_get(jclsRef, types, i)->jvm_mirror;

        // 可变长参数列表误区与陷阱——va_arg不可接受的类型：
        // https://www.cnblogs.com/shiweihappy/p/4246442.html
        if (is_boolean_class(c)) {
            slot_set_bool(real_args + k, va_arg(args, jint));
        } else if (is_byte_class(c)) {
            slot_set_byte(real_args + k, va_arg(args, jint));
        } else if (is_char_class(c)) {
            slot_set_char(real_args + k, va_arg(args, jint));
        } else if (is_short_class(c)) {
            slot_set_short(real_args + k, va_arg(args, jint));
        } else if (is_int_class(c)) {
            slot_set_int(real_args + k, va_arg(args, jint));
        } else if (is_float_class(c)) {
            slot_set_float(real_args + k, va_arg(args, jdouble));
        } else if (is_long_class(c)) { // category_two 
            slot_set_long(real_args + k++, va_arg(args, jlong));
        } else if (is_double_class(c)) { // category_two
            slot_set_double(real_args + k++, va_arg(args, jdouble));
        } else {
            slot_set_ref(real_args + k, va_arg(args, jref));
        }
    }

    return exec_java(m, real_args);
}

static slot_t *execJavaA(Method *m, jref this, const jvalue *args)
{
    // Class[]
    jarrRef types = get_parameter_types(m);
    jsize args_count = types->arr_len;
    
    // 因为有 category two 的存在，result 的长度最大为 types_len * 2 + this_obj
    slot_t *real_args = vm_malloc(sizeof(slot_t)*(2*args_count + 1));
    int k = 0;
    if (this != NULL) {
        assert(!IS_STATIC(m));
        slot_set_ref(real_args, this);
        k++;
    }

    for (int i = 0; i < args_count; i++, k++) {
        Class *c = array_get(jclsRef, types, i)->jvm_mirror;

        if (is_boolean_class(c)) {
            slot_set_bool(real_args + k, args[i].z);
        } else if (is_byte_class(c)) {
            slot_set_byte(real_args + k, args[i].b);
        } else if (is_char_class(c)) {
            slot_set_char(real_args + k, args[i].c);
        } else if (is_short_class(c)) {
            slot_set_short(real_args + k, args[i].s);
        } else if (is_int_class(c)) {
            slot_set_int(real_args + k, args[i].i);
        } else if (is_float_class(c)) {
            slot_set_float(real_args + k, args[i].f);
        } else if (is_long_class(c)) { // category_two 
            slot_set_long(real_args + k++, args[i].j);
        } else if (is_double_class(c)) { // category_two
            slot_set_double(real_args + k++, args[i].d);
        } else {
            slot_set_ref(real_args + k, (jref) args[i].l);
        }
    }

    return exec_java(m, real_args);
}

// GlobalRefTable global_refs;
// GlobalRefTable weak_global_refs;

jint JNICALL Cabin_GetVersion(JNIEnv *env)
{
    TRACE("Cabin_GetVersion(env=%p)", env);
    
    // todo
    JVM_PANIC("not implement.");
}

jclass JNICALL Cabin_DefineClass(JNIEnv *env,
                const char *name, jobject loader, const jbyte *buf, jsize len)
{
    TRACE("Cabin_DefineClass(env=%p, name=%s, loader=%p, buf=%p, len=%d)", env, name, loader, buf, len);

    assert(name != NULL && buf != NULL && len >= 0);

    Class *c = define_class((jref) loader, (u1 *) buf, len);
    if (c != NULL)
        link_class(c);
    return (jclass) c->java_mirror;
    // return (jclass) addJNILocalRef(c); // todo
}

jclass JNICALL Cabin_FindClass(JNIEnv *env, const char *name)
{
    // assert(env != NULL && name != NULL);
    // // todo
    // jref loader = ((Frame *) (*env)->functions->reserved3)->method->clazz->loader;
    // Class *c = loadClass(loader, name);
    // return to_jclass(c);

    // todo
    Class *c = load_class(NULL, name); // todo clasloader
    return (jclass) c->java_mirror;

    //    JVM_PANIC("not implement.");  // todo
}

jmethodID JNICALL Cabin_FromReflectedMethod(JNIEnv *env, jobject method)
{
    // todo
    JVM_PANIC("not implement.");
}

jfieldID JNICALL Cabin_FromReflectedField(JNIEnv *env, jobject field)
{
    // todo
    JVM_PANIC("not implement.");
}

jobject JNICALL Cabin_ToReflectedMethod(JNIEnv *env, jclass cls, jmethodID methodID, jboolean isStatic)
{
    // todo
    JVM_PANIC("not implement.");
}

/**
 * Returns the Class representing the superclass of the entity
 * (class, interface, primitive type or void) represented by this
 * Class.  If this Class represents either the Object class, 
 * an interface, a primitive type, or void, then null is returned.  
 * If this object represents an array class then the
 * Class object representing the Object class is returned.
 */
jclass JNICALL Cabin_GetSuperclass(JNIEnv *env, jclass sub)
{
    // jclsRef c = (jclsRef) sub;
    // return c->jvm_mirror->super_class;

    Class *c = ((jclsRef) sub)->jvm_mirror;
    if (IS_INTERFACE(c) || is_prim_class(c) || is_void_class(c))
        return NULL;
    if (c->super_class == NULL)
        return NULL;
    return (jclass) c->super_class->java_mirror;

    // return (jclass) addJNILocalRef(c->jvm_mirror->super_class);
}

// 查看 static jboolean Class::isAssignableFrom(JNIEnv *env, jclsRef this, jclsRef cls);
jboolean JNICALL Cabin_IsAssignableFrom(JNIEnv *env, jclass sub, jclass sup)
{
    // todo
    return is_subclass_of(JVM_MIRROR(sub), JVM_MIRROR(sup));
}

jobject JNICALL Cabin_ToReflectedField(JNIEnv *env, jclass cls, jfieldID fieldID, jboolean isStatic)
{
    // todo
    JVM_PANIC("not implement.");
}

jint JNICALL Cabin_Throw(JNIEnv *env, jthrowable obj)
{
    set_exception(obj);
    return JNI_TRUE;
}

jint JNICALL Cabin_ThrowNew(JNIEnv *env, jclass clazz, const char *msg)
{
    Class *c = JVM_MIRROR(clazz);
    raise_exception(c->class_name, msg);
    return JNI_TRUE;
}

jthrowable JNICALL Cabin_ExceptionOccurred(JNIEnv *env)
{
    return exception_occurred();
}

void JNICALL Cabin_ExceptionDescribe(JNIEnv *env)
{
    // todo
    JVM_PANIC("not implement.");
}

void JNICALL Cabin_ExceptionClear(JNIEnv *env)
{
    clear_exception();
}

void JNICALL Cabin_FatalError(JNIEnv *env, const char *msg)
{
    // todo
    JVM_PANIC("not implement.");
}

jint JNICALL Cabin_PushLocalFrame(JNIEnv *env, jint capacity)
{
    // todo
    JVM_PANIC("not implement.");
}

jobject JNICALL Cabin_PopLocalFrame(JNIEnv *env, jobject result)
{
    // todo
    JVM_PANIC("not implement.");
}

jobject JNICALL Cabin_NewGlobalRef(JNIEnv *env, jobject gref)
{
    // todo
    // JVM_PANIC("not implement.");
    // assert(env != NULL && gref != NULL);
    return addJNIGlobalRef(gref); // todo
}

void JNICALL Cabin_DeleteGlobalRef(JNIEnv *env, jobject gref)
{
    // todo
    JVM_PANIC("not implement.");
    // assert(env != NULL && gref != NULL);
    // auto o = to_object_ref(gref);
    // if (o->jni_obj_ref_type == JNIGlobalRefType)
    //     deleteJNIGlobalRef(o);
}

void JNICALL Cabin_DeleteLocalRef(JNIEnv *env, jobject obj)
{
    // assert(env != NULL && obj != NULL);
    // deleteJNILocalRef(to_object_ref(obj));

    // todo

    // JVM_PANIC("not implement.");
}

jboolean JNICALL Cabin_IsSameObject(JNIEnv *env, jobject obj1, jobject obj2)
{
    // todo
    assert(env != NULL && obj1 != NULL && obj2 != NULL);
    JVM_PANIC("not implement.");
}

jobject JNICALL Cabin_NewLocalRef(JNIEnv *env, jobject obj)
{
    assert(env != NULL && obj != NULL);
    JVM_PANIC("not implement.");
    // return addJNILocalRef(to_object_ref(obj));
}

jint JNICALL Cabin_EnsureLocalCapacity(JNIEnv *env, jint capacity)
{
    // todo
    assert(env != NULL);

    // todo 此函数干啥的？？？
    return capacity; 
    // JVM_PANIC("not implement.");

}

jobject JNICALL Cabin_AllocObject(JNIEnv *env, jclass clazz)
{
    JVM_PANIC("not implement.");
    // assert(env != NULL && clazz != NULL);
    // Class *c = checkClassBeforeAllocObject(to_object_ref<Class>(clazz));
    // if (c == NULL) {
    //     // todo
    // }

    // return addJNILocalRef(newObject(c));
}

jobject JNICALL Cabin_NewObject(JNIEnv *env, jclass clazz, jmethodID methodID, ...)
{
    va_list args;
    va_start(args, methodID);
    jobject o = (*env)->NewObjectV(env, clazz, methodID, args);
    va_end(args);
    return o;
}

jobject JNICALL Cabin_NewObjectV(JNIEnv *env, jclass clazz, jmethodID methodID, va_list args)
{
    jobject o = (*env)->AllocObject(env, clazz);
    if (o == NULL) {
        // todo error
    }

    slot_t *ret = execJavaV((Method *) methodID, (jref) o, args); 
    return (jobject) RSLOT(ret);
}

jobject JNICALL Cabin_NewObjectA(JNIEnv *env, jclass clazz, jmethodID methodID, const jvalue *args)
{
    jobject o = (*env)->AllocObject(env, clazz);
    if (o == NULL) {
        // todo error
    }

    slot_t *ret = execJavaA((Method *) methodID, (jref) o, args);
    return (jobject) RSLOT(ret);
}

jclass JNICALL Cabin_GetObjectClass(JNIEnv *env, jobject obj)
{
    // assert(env != NULL && obj != NULL);
    // Object *o = to_object_ref(obj);
    // return (jclass) addJNILocalRef(o->clazz);
    return (jclass) ((jref)obj)->clazz->java_mirror;
}

jboolean JNICALL Cabin_IsInstanceOf(JNIEnv *env, jobject obj, jclass clazz)
{
    // assert(env != NULL && obj != NULL && clazz != NULL);
    // auto o = to_object_ref(obj);
    // auto c = to_object_ref<Class>(clazz);
    // return o->isInstanceOf(c) ? JNI_TRUE : JNI_FALSE;
    return is_instance_of((jref) obj, JVM_MIRROR(clazz));
}

jmethodID JNICALL Cabin_GetMethodID(JNIEnv *env, jclass clazz, const char *name, const char *sig)
{
    Class *c = JVM_MIRROR(clazz);
    Method *m = lookup_inst_method(c, name, sig);
    return (jmethodID) m;
}

/*
 * 定义 Call_T_Method 镞函数：
 *
 * T JNICALL JVM_Call_T_Method(JNIEnv *env, jobject obj, jmethodID methodID, ...);
 * T JNICALL JVM_Call_T_MethodV(JNIEnv *env, jobject obj, jmethodID methodID, va_list args);
 * T JNICALL JVM_Call_T_MethodA(JNIEnv *env, jobject obj, jmethodID methodID, const jvalue *args);
 */

#define DEFINE_CALL_T_METHOD_BODY \
    jref _this = (jref)(obj); \
    Method *_m0 = (Method *)(methodID); \
    Method *_m = lookup_method(_this->clazz, _m0->name, _m0->descriptor); \
    if (_m == NULL) { \
        /* todo error */ \
    }

#define DEFINE_CALL_T_METHOD(T, ret_type, ret_value)  \
ret_type JNICALL Cabin_Call##T##Method(JNIEnv *env, jobject obj, jmethodID methodID, ...) \
{ \
    va_list args; \
    va_start(args, methodID); \
    DEFINE_CALL_T_METHOD_BODY \
    slot_t *ret = execJavaV(_m, _this, args); \
    va_end(args); \
    return ret_value; \
}

#define DEFINE_CALL_T_METHOD_V(T, ret_type, ret_value) \
ret_type JNICALL Cabin_Call##T##MethodV(JNIEnv *env, jobject obj, jmethodID methodID, va_list args) \
{ \
    DEFINE_CALL_T_METHOD_BODY \
    slot_t *ret = execJavaV(_m, _this, args); \
    return ret_value; \
}

#define DEFINE_CALL_T_METHOD_A(T, ret_type, ret_value) \
ret_type JNICALL Cabin_Call##T##MethodA(JNIEnv *env, \
                                       jobject obj, jmethodID methodID, const jvalue *args) \
{ \
    DEFINE_CALL_T_METHOD_BODY \
    slot_t *ret = execJavaA(_m, _this, args); \
    return ret_value; \
}

#define DEFINE_3_CALL_T_METHODS(T, ret_type, ret_value) \
    DEFINE_CALL_T_METHOD(T, ret_type, ret_value) \
    DEFINE_CALL_T_METHOD_V(T, ret_type, ret_value) \
    DEFINE_CALL_T_METHOD_A(T, ret_type, ret_value)


DEFINE_3_CALL_T_METHODS(Object, jobject, addJNILocalRef(RSLOT(ret)))
DEFINE_3_CALL_T_METHODS(Boolean, jboolean, JINT_TO_JBOOL(ISLOT(ret)))
DEFINE_3_CALL_T_METHODS(Byte, jbyte, JINT_TO_JBYTE(ISLOT(ret)))
DEFINE_3_CALL_T_METHODS(Char, jchar, JINT_TO_JCHAR(ISLOT(ret)))
DEFINE_3_CALL_T_METHODS(Short, jshort, JINT_TO_JSHORT(ISLOT(ret)))
DEFINE_3_CALL_T_METHODS(Int, jint, ISLOT(ret))
DEFINE_3_CALL_T_METHODS(Long, jlong, LSLOT(ret))
DEFINE_3_CALL_T_METHODS(Float, jfloat, FSLOT(ret))
DEFINE_3_CALL_T_METHODS(Double, jdouble, DSLOT(ret))
DEFINE_3_CALL_T_METHODS(Void, void, )

/*
 * 定义 CallNonvirtual_T_Method 镞函数：
 *
 * T JNICALL JVM_CallNonvirtual_T_Method(JNIEnv *env, jobject obj, jclass clazz, jmethodID methodID, ...);
 * T JNICALL JVM_CallNonvirtual_T_MethodV(JNIEnv *env, jobject obj, jclass clazz, jmethodID methodID, va_list args);
 * T JNICALL JVM_CallNonvirtual_T_MethodA(JNIEnv *env, jobject obj, jclass clazz, jmethodID methodID, const jvalue *args);
 */

#define DEFINE_CALL_NONVIRTUAL_T_METHOD(T, ret_type, ret_value)  \
ret_type JNICALL Cabin_CallNonvirtual##T##Method(JNIEnv *env, jobject obj, jclass clazz, jmethodID methodID, ...) \
{ \
    va_list args; \
    va_start(args, methodID); \
    slot_t *ret = execJavaV((Method *) methodID, (jref) obj, args); \
    va_end(args); \
    return ret_value; \
}

#define DEFINE_CALL_NONVIRTUAL_T_METHOD_V(T, ret_type, ret_value) \
ret_type JNICALL Cabin_CallNonvirtual##T##MethodV(JNIEnv *env, jobject obj, \
                            jclass clazz, jmethodID methodID, va_list args) \
{ \
    slot_t *ret = execJavaV((Method *) methodID, (jref) obj, args); \
    return ret_value; \
}

#define DEFINE_CALL_NONVIRTUAL_T_METHOD_A(T, ret_type, ret_value) \
ret_type JNICALL Cabin_CallNonvirtual##T##MethodA(JNIEnv *env, jobject obj, \
                            jclass clazz, jmethodID methodID, const jvalue *args) \
{ \
    slot_t *ret = execJavaA((Method *) methodID, (jref) obj, args); \
    return ret_value; \
}

#define DEFINE_3_CALL_NONVIRTUAL_T_METHODS(T, ret_type, ret_value) \
    DEFINE_CALL_NONVIRTUAL_T_METHOD_A(T, ret_type, ret_value) \
    DEFINE_CALL_NONVIRTUAL_T_METHOD_V(T, ret_type, ret_value) \
    DEFINE_CALL_NONVIRTUAL_T_METHOD(T, ret_type, ret_value)


DEFINE_3_CALL_NONVIRTUAL_T_METHODS(Object, jobject, addJNILocalRef(RSLOT(ret)))
DEFINE_3_CALL_NONVIRTUAL_T_METHODS(Boolean, jboolean, JINT_TO_JBOOL(ISLOT(ret)))
DEFINE_3_CALL_NONVIRTUAL_T_METHODS(Byte, jbyte, JINT_TO_JBYTE(ISLOT(ret)))
DEFINE_3_CALL_NONVIRTUAL_T_METHODS(Char, jchar, JINT_TO_JCHAR(ISLOT(ret)))
DEFINE_3_CALL_NONVIRTUAL_T_METHODS(Short, jshort, JINT_TO_JSHORT(ISLOT(ret)))
DEFINE_3_CALL_NONVIRTUAL_T_METHODS(Int, jint, ISLOT(ret))
DEFINE_3_CALL_NONVIRTUAL_T_METHODS(Long, jlong, LSLOT(ret))
DEFINE_3_CALL_NONVIRTUAL_T_METHODS(Float, jfloat, FSLOT(ret))
DEFINE_3_CALL_NONVIRTUAL_T_METHODS(Double, jdouble, DSLOT(ret))
DEFINE_3_CALL_NONVIRTUAL_T_METHODS(Void, void, )

/*
* 定义 CallStatic_T_Method 镞函数：
*
* T JNICALL JVM_CallStatic_T_Method(JNIEnv *env, jobject obj, jclass clazz, jmethodID methodID, ...);
* T JNICALL JVM_CallStatic_T_MethodV(JNIEnv *env, jobject obj, jclass clazz, jmethodID methodID, va_list args);
* T JNICALL JVM_CallStatic_T_MethodA(JNIEnv *env, jobject obj, jclass clazz, jmethodID methodID, const jvalue *args);
*/

#define DEFINE_CALL_STATIC_T_METHOD(T, ret_type, ret_value)  \
ret_type JNICALL Cabin_CallStatic##T##Method(JNIEnv *env, jclass clazz, jmethodID methodID, ...) \
{ \
    va_list args; \
    va_start(args, methodID); \
    slot_t *ret = execJavaV((Method *) methodID, NULL, args); \
    va_end(args); \
    return ret_value; \
}

#define DEFINE_CALL_STATIC_T_METHOD_V(T, ret_type, ret_value) \
ret_type JNICALL Cabin_CallStatic##T##MethodV(JNIEnv *env, \
                            jclass clazz, jmethodID methodID, va_list args) \
{ \
    slot_t *ret = execJavaV((Method *) methodID, NULL, args); \
    return ret_value; \
}

#define DEFINE_CALL_STATIC_T_METHOD_A(T, ret_type, ret_value) \
ret_type JNICALL Cabin_CallStatic##T##MethodA(JNIEnv *env, \
                            jclass clazz, jmethodID methodID, const jvalue *args) \
{ \
    slot_t *ret = execJavaA((Method *) methodID, NULL, args); \
    return ret_value; \
}

#define DEFINE_3_CALL_STATIC_T_METHODS(T, ret_type, ret_value) \
    DEFINE_CALL_STATIC_T_METHOD(T, ret_type, ret_value) \
    DEFINE_CALL_STATIC_T_METHOD_V(T, ret_type, ret_value) \
    DEFINE_CALL_STATIC_T_METHOD_A(T, ret_type, ret_value)

DEFINE_3_CALL_STATIC_T_METHODS(Object, jobject, addJNILocalRef(RSLOT(ret)))
DEFINE_3_CALL_STATIC_T_METHODS(Boolean, jboolean, JINT_TO_JBOOL(ISLOT(ret)))
DEFINE_3_CALL_STATIC_T_METHODS(Byte, jbyte, JINT_TO_JBYTE(ISLOT(ret)))
DEFINE_3_CALL_STATIC_T_METHODS(Char, jchar, JINT_TO_JCHAR(ISLOT(ret)))
DEFINE_3_CALL_STATIC_T_METHODS(Short, jshort, JINT_TO_JSHORT(ISLOT(ret)))
DEFINE_3_CALL_STATIC_T_METHODS(Int, jint, ISLOT(ret))
DEFINE_3_CALL_STATIC_T_METHODS(Long, jlong, LSLOT(ret))
DEFINE_3_CALL_STATIC_T_METHODS(Float, jfloat, FSLOT(ret))
DEFINE_3_CALL_STATIC_T_METHODS(Double, jdouble, DSLOT(ret))
DEFINE_3_CALL_STATIC_T_METHODS(Void, void, )

jfieldID JNICALL Cabin_GetFieldID(JNIEnv *env, jclass clazz, const char *name, const char *sig)
{
    Class *c = JVM_MIRROR(clazz);
    init_class(c);

    Field *f = lookup_field(c, name, sig);
    if (f == NULL) {
        // todo java_lang_NoSuchFieldError
    }
    return (jfieldID) f;
}

jobject JNICALL Cabin_GetObjectField(JNIEnv *env, jobject obj, jfieldID fieldID)
{
    // return addJNILocalRef(o->getRefField(f));

    return (jobject) get_ref_field0((jref) obj, (Field *) fieldID);
}

#define GET_AND_SET_FIELD(Type, jtype, type, t) \
jtype JNICALL Cabin_Get##Type##Field(JNIEnv *env, jobject obj, jfieldID fieldID) \
{ \
    return get_##type##_field0((jref) obj, (Field *) fieldID); \
} \
\
void JNICALL Cabin_Set##Type##Field(JNIEnv *env, jobject obj, jfieldID fieldID, jtype val) \
{ \
    set_##type##_field0((jref) obj, (Field *) fieldID, val); \
} \
\
jtype JNICALL Cabin_GetStatic##Type##Field(JNIEnv *env, jclass clazz, jfieldID fieldID) \
{ \
    return ((Field *) fieldID)->static_value.t; \
} \
\
void JNICALL Cabin_SetStatic##Type##Field(JNIEnv *env, jclass clazz, jfieldID fieldID, jtype value) \
{ \
    ((Field *) fieldID)->static_value.t = value; \
}

GET_AND_SET_FIELD(Boolean, jboolean, bool, z)
GET_AND_SET_FIELD(Byte, jbyte, byte, b)
GET_AND_SET_FIELD(Char, jchar, char, c)
GET_AND_SET_FIELD(Short, jshort, short, s)
GET_AND_SET_FIELD(Int, jint, int, i)
GET_AND_SET_FIELD(Long, jlong, long, j)
GET_AND_SET_FIELD(Float, jfloat, float, f)
GET_AND_SET_FIELD(Double, jdouble, double, d)

#undef GET_AND_SET_FIELD

void JNICALL Cabin_SetObjectField(JNIEnv *env, jobject obj, jfieldID fieldID, jobject val)
{
    set_ref_field0((jref) obj, (Field *) fieldID, (jref) val);
}

jmethodID JNICALL Cabin_GetStaticMethodID(JNIEnv *env, jclass clazz, const char *name, const char *sig)
{
    return (jmethodID) lookup_static_method(JVM_MIRROR(clazz), name, sig);
}

jfieldID JNICALL Cabin_GetStaticFieldID(JNIEnv *env, jclass clazz, const char *name, const char *sig)
{
    return (jfieldID) lookup_static_field(JVM_MIRROR(clazz), name, sig);
}

jobject JNICALL Cabin_GetStaticObjectField(JNIEnv *env, jclass clazz, jfieldID fieldID)
{
    // assert(env != NULL && clazz != NULL && fieldID != NULL);
    // return addJNILocalRef(to_field(fieldID)->staticValue.r);

    return (jobject) (((Field *) fieldID)->static_value.r);
}

void JNICALL Cabin_SetStaticObjectField(JNIEnv *env, jclass clazz, jfieldID fieldID, jobject value)
{
    ((Field *) fieldID)->static_value.r = (jref) value;
}

jstring JNICALL Cabin_NewString(JNIEnv *env, const jchar *unicode, jsize len)
{
    jstrRef str = alloc_string0(unicode, len);
    return addJNILocalRef(str);
}

jsize JNICALL Cabin_GetStringLength(JNIEnv *env, jstring str)
{
    return get_string_length((jstrRef) str);
}

const jchar *JNICALL Cabin_GetStringChars(JNIEnv *env, jstring str, jboolean *isCopy)
{
    // // addJNIGlobalRef(so); /* Pin the reference */
    if (isCopy != NULL)
        *isCopy = JNI_TRUE;

    unicode_t *u = string_to_unicode((jstrRef)str); 
    return u;
}

void JNICALL Cabin_ReleaseStringChars(JNIEnv *env, jstring str, const jchar *chars)
{
    // if (g_jdk_version_9_and_upper) {
    //     delete[] chars;
    // } else {
    //     deleteJNIGlobalRef(to_object_ref(str)); /* Unpin the reference */
    // }

    // todo

    // JVM_PANIC("not implement."); // todo
}

jstring JNICALL Cabin_NewStringUTF(JNIEnv *env, const char *utf)
{
    jstrRef str = alloc_string(utf);
    return addJNILocalRef(str);
}

jsize JNICALL Cabin_GetStringUTFLength(JNIEnv *env, jstring str)
{
    return get_string_uft_length((jstrRef) str);
}

const char* JNICALL Cabin_GetStringUTFChars(JNIEnv *env, jstring str, jboolean *isCopy)
{
//    // addJNIGlobalRef(so); /* Pin the reference */
    if (isCopy != NULL)
        *isCopy = JNI_TRUE;

    utf8_t *u = string_to_utf8((jstrRef)str);
    return u;
}

void JNICALL Cabin_ReleaseStringUTFChars(JNIEnv *env, jstring str, const char *chars)
{
    // if (g_jdk_version_9_and_upper) {
    //     deleteJNIGlobalRef(to_object_ref(str)); /* Unpin the reference */
    // } else {
    //     delete[] chars;
    // }

    // todo

    // JVM_PANIC("not implement."); // todo
}

jsize JNICALL Cabin_GetArrayLength(JNIEnv *env, jarray arr)
{
    jarrRef array = (jarrRef) arr;
    return array->arr_len;
}

jobjectArray JNICALL Cabin_NewObjectArray(JNIEnv *env, jsize len, jclass elementClass, jobject init)
{
    if (len < 0) {
        JNI_THROW_NegativeArraySizeException(env, NULL);
        return NULL;
    }
    
    jarrRef arr = alloc_array(array_class(JVM_MIRROR(elementClass)), len);
    if (init != NULL) {
        for (int i = 0; i < len; ++i) {
            array_set_ref(arr, i, (jref) init);
        }
    }

    return (jobjectArray) addJNILocalRef(arr);
}

jobject JNICALL Cabin_GetObjectArrayElement(JNIEnv *env, jobjectArray array, jsize index)
{
    jarrRef ao = (jarrRef) array;
    if (index <= 0 || index >= ao->arr_len) {
        // todo error
    }

    return (jobject) array_get(jref, ao, index);
}

void JNICALL Cabin_SetObjectArrayElement(JNIEnv *env, jobjectArray array, jsize index, jobject val)
{
    jarrRef ao = (jarrRef) array;
    if (index < 0 || index >= ao->arr_len) {
        JNI_THROW_ArrayIndexOutOfBoundsException(env, NULL); // todo msg
        return;
    }

    array_set_ref(ao, index, (jref) val);
}

#define NewTypeArray(Type, class_name) \
jarray JNICALL Cabin_New##Type##Array(JNIEnv *env, jsize len) \
{ \
    if (len < 0) { \
        JNI_THROW_NegativeArraySizeException(env, NULL); \
        return NULL; \
    } \
 \
    jarrRef arr = alloc_array0(BOOT_CLASS_LOADER, class_name, len); \
    return addJNILocalRef(arr); \
}

NewTypeArray(Byte, "[B")
NewTypeArray(Boolean, "[Z")
NewTypeArray(Char, "[C")
NewTypeArray(Short, "[S")
NewTypeArray(Int, "[I")
NewTypeArray(Long, "[J")
NewTypeArray(Float, "[F")
NewTypeArray(Double, "[D")

#define getTypeArrayElements(Type, raw_type) \
raw_type* JNICALL Cabin_Get##Type##ArrayElements(JNIEnv *env, jarray array, jboolean *isCopy) \
{ \
    jarrRef arr = (jarrRef) array; \
    if (isCopy != NULL) \
        *isCopy = JNI_FALSE; \
    addJNIGlobalRef(arr); \
    return (raw_type *) arr->data; \
}

getTypeArrayElements(Byte, jbyte)
getTypeArrayElements(Boolean, jboolean)
getTypeArrayElements(Char, jchar)
getTypeArrayElements(Short, jshort)
getTypeArrayElements(Int, jint)
getTypeArrayElements(Long, jlong)
getTypeArrayElements(Float, jfloat)
getTypeArrayElements(Double, jdouble)


#define releaseTypeArrayElements(Type, raw_type) \
void JNICALL Cabin_Release##Type##ArrayElements(JNIEnv *env, jarray array, raw_type *elems, jint mode) \
{ \
    deleteJNIGlobalRef(array); /* todo mode */\
}

releaseTypeArrayElements(Byte, jbyte)
releaseTypeArrayElements(Boolean, jboolean)
releaseTypeArrayElements(Char, jchar)
releaseTypeArrayElements(Short, jshort)
releaseTypeArrayElements(Int, jint)
releaseTypeArrayElements(Long, jlong)
releaseTypeArrayElements(Float, jfloat)
releaseTypeArrayElements(Double, jdouble)

#define GET_AND_SET_TYPE_ARRAY_REGION(Type, raw_type) \
void JNICALL Cabin_Get##Type##ArrayRegion(JNIEnv *env, jarray array, jsize start, jsize len, raw_type *buf) \
{ \
    jarrRef arr = (jarrRef) array; \
    assert(start + len <= arr->arr_len); \
    assert(get_ele_size(arr->clazz) == sizeof(raw_type)); \
    memcpy(buf, array_index(arr, start), len*sizeof(raw_type)); \
} \
\
void JNICALL Cabin_Set##Type##ArrayRegion(JNIEnv *env, jarray array, jsize start, jsize len, const raw_type *buf) \
{ \
    jarrRef arr = (jarrRef) array; \
    assert(start + len <= arr->arr_len); \
    assert(get_ele_size(arr->clazz) == sizeof(raw_type)); \
    memcpy(array_index(arr, start), buf, len*sizeof(raw_type)); \
}

GET_AND_SET_TYPE_ARRAY_REGION(Byte, jbyte)
GET_AND_SET_TYPE_ARRAY_REGION(Boolean, jboolean)
GET_AND_SET_TYPE_ARRAY_REGION(Char, jchar)
GET_AND_SET_TYPE_ARRAY_REGION(Short, jshort)
GET_AND_SET_TYPE_ARRAY_REGION(Int, jint)
GET_AND_SET_TYPE_ARRAY_REGION(Long, jlong)
GET_AND_SET_TYPE_ARRAY_REGION(Float, jfloat)
GET_AND_SET_TYPE_ARRAY_REGION(Double, jdouble)

#undef GET_AND_SET_TYPE_ARRAY_REGION

jint JNICALL Cabin_RegisterNatives(JNIEnv *env, jclass clazz, const JNINativeMethod *methods, jint methods_count)
{
    Class *c = JVM_MIRROR(clazz);
    for (jint i = 0; i < methods_count; i++) {
        Method *m = get_declared_method_noexcept(c, methods[i].name, methods[i].signature);
        if (m == NULL || !IS_NATIVE(m)) {
            SHOULD_NEVER_REACH_HERE("%s, %s, %d",
                    methods[i].name, methods[i].signature, m != NULL ? IS_NATIVE(m) : -1);
            return JNI_ERR;
        }
        m->native_method = methods[i].fnPtr;
    }

    return JNI_OK;
}

jint JNICALL Cabin_UnregisterNatives(JNIEnv *env, jclass clazz)
{
    Class *c = JVM_MIRROR(clazz);
    for (u2 i = 0; i < c->methods_count; i++) {
        c->methods[i].native_method = NULL;
    }
    return JNI_OK;
}

jint JNICALL Cabin_MonitorEnter(JNIEnv *env, jobject obj)
{
    // todo
//    jref o = to_object_ref(obj);
//    o->lock();
// return JNI_OK;
    JVM_PANIC("not implement.");
}

jint JNICALL Cabin_MonitorExit(JNIEnv *env, jobject obj)
{
    // todo
//    jref o = to_object_ref(obj);
//    o->unlock();
// return JNI_OK;
    JVM_PANIC("not implement.");
}

static JavaVM java_vm;

jint JNICALL Cabin_GetJavaVM(JNIEnv *env, JavaVM **vm)
{
    *vm = &java_vm;
    return JNI_OK;
}

void JNICALL Cabin_GetStringRegion(JNIEnv *env, jstring str, jsize start, jsize len, jchar *buf)
{
    unicode_t *u = string_to_unicode((jstrRef) str);
    memcpy(buf, u + start, len * sizeof(unicode_t));
    buf[len] = 0;
}

void JNICALL Cabin_GetStringUTFRegion(JNIEnv *env, jstring str, jsize start, jsize len, char *buf)
{   
    utf8_t *u = string_to_utf8((jstrRef) str);
    strncpy(buf, u + start, len);
    buf[len] = 0;
}

void* JNICALL Cabin_GetPrimitiveArrayCritical(JNIEnv *env, jarray array, jboolean *isCopy)
{
    jarrRef arr = (jarrRef) array;

    if(isCopy != NULL)
        *isCopy = JNI_FALSE;

    /* Pin the array */ //  todo
    addJNIGlobalRef(arr);
    return arr->data;
}

void JNICALL Cabin_ReleasePrimitiveArrayCritical(JNIEnv *env, jarray array, void *carray, jint mode)
{ 
    // todo 'mode'
    deleteJNIGlobalRef(array);
}

const jchar* JNICALL Cabin_GetStringCritical(JNIEnv *env, jstring string, jboolean *isCopy)
{
    return Cabin_GetStringChars(env, string, isCopy);
}

void JNICALL Cabin_ReleaseStringCritical(JNIEnv *env, jstring string, const jchar *cstring)
{
    Cabin_ReleaseStringChars(env, string, cstring);
}

jweak JNICALL Cabin_NewWeakGlobalRef(JNIEnv *env, jobject obj)
{
    return addJNIWeakGlobalRef(obj);
}

void JNICALL Cabin_DeleteWeakGlobalRef(JNIEnv *env, jweak ref)
{
    deleteJNIWeakGlobalRef(ref);
}

jboolean JNICALL Cabin_ExceptionCheck(JNIEnv *env)
{
    // todo
    JVM_PANIC("not implement.");
}

jobject JNICALL Cabin_NewDirectByteBuffer(JNIEnv *env, void *address, jlong capacity)
{
    // todo
    JVM_PANIC("not implement.");
}

void* JNICALL Cabin_GetDirectBufferAddress(JNIEnv *env, jobject buf)
{
    // todo
    JVM_PANIC("not implement.");
}

jlong JNICALL Cabin_GetDirectBufferCapacity(JNIEnv *env, jobject buf)
{    
    JVM_PANIC("not implement."); // todo
}

jobjectRefType JNICALL Cabin_GetObjectRefType(JNIEnv* env, jobject obj)
{
    // jref o = to_object_ref(obj);
    // return o != NULL ? o->jni_obj_ref_type : JNIInvalidRefType;
    JVM_PANIC("not implement."); // todo
}

jobject JNICALL Cabin_GetModule(JNIEnv* env, jclass clazz)
{
    // todo
    JVM_PANIC("not implement.");
}


static struct JNINativeInterface_ Cabin_JNINativeInterface = {
    .reserved0 = NULL,
    .reserved1 = NULL,
    .reserved2 = NULL,
    .reserved3 = NULL,

    .GetVersion = Cabin_GetVersion,

    .DefineClass = Cabin_DefineClass,
    .FindClass = Cabin_FindClass,

    .FromReflectedMethod = Cabin_FromReflectedMethod,
    .FromReflectedField = Cabin_FromReflectedField,

    .ToReflectedMethod = Cabin_ToReflectedMethod,

    .GetSuperclass = Cabin_GetSuperclass,
    .IsAssignableFrom = Cabin_IsAssignableFrom,

    .ToReflectedField = Cabin_ToReflectedField,

    .Throw = Cabin_Throw,
    .ThrowNew = Cabin_ThrowNew,
    .ExceptionOccurred = Cabin_ExceptionOccurred,
    .ExceptionDescribe = Cabin_ExceptionDescribe,
    .ExceptionClear = Cabin_ExceptionClear,
    .FatalError = Cabin_FatalError,

    .PushLocalFrame = Cabin_PushLocalFrame,
    .PopLocalFrame = Cabin_PopLocalFrame,

    .NewGlobalRef = Cabin_NewGlobalRef,
    .DeleteGlobalRef = Cabin_DeleteGlobalRef,
    .DeleteLocalRef = Cabin_DeleteLocalRef,
    .IsSameObject = Cabin_IsSameObject,
    .NewLocalRef = Cabin_NewLocalRef,
    .EnsureLocalCapacity = Cabin_EnsureLocalCapacity,

    .AllocObject = Cabin_AllocObject,
    .NewObject = Cabin_NewObject,
    .NewObjectV = Cabin_NewObjectV,
    .NewObjectA = Cabin_NewObjectA,

    .GetObjectClass = Cabin_GetObjectClass,
    .IsInstanceOf = Cabin_IsInstanceOf,

    .GetMethodID = Cabin_GetMethodID,

    .CallObjectMethod = Cabin_CallObjectMethod,
    .CallObjectMethodV = Cabin_CallObjectMethodV,
    .CallObjectMethodA = Cabin_CallObjectMethodA,

    .CallBooleanMethod = Cabin_CallBooleanMethod,
    .CallBooleanMethodV = Cabin_CallBooleanMethodV,
    .CallBooleanMethodA = Cabin_CallBooleanMethodA,

    .CallByteMethod = Cabin_CallByteMethod,
    .CallByteMethodV = Cabin_CallByteMethodV,
    .CallByteMethodA = Cabin_CallByteMethodA,

    .CallCharMethod = Cabin_CallCharMethod,
    .CallCharMethodV = Cabin_CallCharMethodV,
    .CallCharMethodA = Cabin_CallCharMethodA,

    .CallShortMethod = Cabin_CallShortMethod,
    .CallShortMethodV = Cabin_CallShortMethodV,
    .CallShortMethodA = Cabin_CallShortMethodA,

    .CallIntMethod = Cabin_CallIntMethod,
    .CallIntMethodV = Cabin_CallIntMethodV,
    .CallIntMethodA = Cabin_CallIntMethodA,

    .CallLongMethod = Cabin_CallLongMethod,
    .CallLongMethodV = Cabin_CallLongMethodV,
    .CallLongMethodA = Cabin_CallLongMethodA,

    .CallFloatMethod = Cabin_CallFloatMethod,
    .CallFloatMethodV = Cabin_CallFloatMethodV,
    .CallFloatMethodA = Cabin_CallFloatMethodA,

    .CallDoubleMethod = Cabin_CallDoubleMethod,
    .CallDoubleMethodV = Cabin_CallDoubleMethodV,
    .CallDoubleMethodA = Cabin_CallDoubleMethodA,

    .CallVoidMethod = Cabin_CallVoidMethod,
    .CallVoidMethodV = Cabin_CallVoidMethodV,
    .CallVoidMethodA = Cabin_CallVoidMethodA,

    .CallNonvirtualObjectMethod = Cabin_CallNonvirtualObjectMethod,
    .CallNonvirtualObjectMethodV = Cabin_CallNonvirtualObjectMethodV,
    .CallNonvirtualObjectMethodA = Cabin_CallNonvirtualObjectMethodA,

    .CallNonvirtualBooleanMethod = Cabin_CallNonvirtualBooleanMethod,
    .CallNonvirtualBooleanMethodV = Cabin_CallNonvirtualBooleanMethodV,
    .CallNonvirtualBooleanMethodA = Cabin_CallNonvirtualBooleanMethodA,

    .CallNonvirtualByteMethod = Cabin_CallNonvirtualByteMethod,
    .CallNonvirtualByteMethodV = Cabin_CallNonvirtualByteMethodV,
    .CallNonvirtualByteMethodA = Cabin_CallNonvirtualByteMethodA,

    .CallNonvirtualCharMethod = Cabin_CallNonvirtualCharMethod,
    .CallNonvirtualCharMethodV = Cabin_CallNonvirtualCharMethodV,
    .CallNonvirtualCharMethodA = Cabin_CallNonvirtualCharMethodA,

    .CallNonvirtualShortMethod = Cabin_CallNonvirtualShortMethod,
    .CallNonvirtualShortMethodV = Cabin_CallNonvirtualShortMethodV,
    .CallNonvirtualShortMethodA = Cabin_CallNonvirtualShortMethodA,

    .CallNonvirtualIntMethod = Cabin_CallNonvirtualIntMethod,
    .CallNonvirtualIntMethodV = Cabin_CallNonvirtualIntMethodV,
    .CallNonvirtualIntMethodA = Cabin_CallNonvirtualIntMethodA,

    .CallNonvirtualLongMethod = Cabin_CallNonvirtualLongMethod,
    .CallNonvirtualLongMethodV = Cabin_CallNonvirtualLongMethodV,
    .CallNonvirtualLongMethodA = Cabin_CallNonvirtualLongMethodA,

    .CallNonvirtualFloatMethod = Cabin_CallNonvirtualFloatMethod,
    .CallNonvirtualFloatMethodV = Cabin_CallNonvirtualFloatMethodV,
    .CallNonvirtualFloatMethodA = Cabin_CallNonvirtualFloatMethodA,

    .CallNonvirtualDoubleMethod = Cabin_CallNonvirtualDoubleMethod,
    .CallNonvirtualDoubleMethodV = Cabin_CallNonvirtualDoubleMethodV,
    .CallNonvirtualDoubleMethodA = Cabin_CallNonvirtualDoubleMethodA,

    .CallNonvirtualVoidMethod = Cabin_CallNonvirtualVoidMethod,
    .CallNonvirtualVoidMethodV = Cabin_CallNonvirtualVoidMethodV,
    .CallNonvirtualVoidMethodA = Cabin_CallNonvirtualVoidMethodA,

    .GetFieldID = Cabin_GetFieldID,

    .GetObjectField = Cabin_GetObjectField,
    .GetBooleanField = Cabin_GetBooleanField,
    .GetByteField = Cabin_GetByteField,
    .GetCharField = Cabin_GetCharField,
    .GetShortField = Cabin_GetShortField,
    .GetIntField = Cabin_GetIntField,
    .GetLongField = Cabin_GetLongField,
    .GetFloatField = Cabin_GetFloatField,
    .GetDoubleField = Cabin_GetDoubleField,

    .SetObjectField = Cabin_SetObjectField,
    .SetBooleanField = Cabin_SetBooleanField,
    .SetByteField = Cabin_SetByteField,
    .SetCharField = Cabin_SetCharField,
    .SetShortField = Cabin_SetShortField,
    .SetIntField = Cabin_SetIntField,
    .SetLongField = Cabin_SetLongField,
    .SetFloatField = Cabin_SetFloatField,
    .SetDoubleField = Cabin_SetDoubleField,

    .GetStaticMethodID = Cabin_GetStaticMethodID,

    .CallStaticObjectMethod = Cabin_CallStaticObjectMethod,
    .CallStaticObjectMethodV = Cabin_CallStaticObjectMethodV,
    .CallStaticObjectMethodA = Cabin_CallStaticObjectMethodA,

    .CallStaticBooleanMethod = Cabin_CallStaticBooleanMethod,
    .CallStaticBooleanMethodV = Cabin_CallStaticBooleanMethodV,
    .CallStaticBooleanMethodA = Cabin_CallStaticBooleanMethodA,

    .CallStaticByteMethod = Cabin_CallStaticByteMethod,
    .CallStaticByteMethodV = Cabin_CallStaticByteMethodV,
    .CallStaticByteMethodA = Cabin_CallStaticByteMethodA,

    .CallStaticCharMethod = Cabin_CallStaticCharMethod,
    .CallStaticCharMethodV = Cabin_CallStaticCharMethodV,
    .CallStaticCharMethodA = Cabin_CallStaticCharMethodA,

    .CallStaticShortMethod = Cabin_CallStaticShortMethod,
    .CallStaticShortMethodV = Cabin_CallStaticShortMethodV,
    .CallStaticShortMethodA = Cabin_CallStaticShortMethodA,

    .CallStaticIntMethod = Cabin_CallStaticIntMethod,
    .CallStaticIntMethodV = Cabin_CallStaticIntMethodV,
    .CallStaticIntMethodA = Cabin_CallStaticIntMethodA,

    .CallStaticLongMethod = Cabin_CallStaticLongMethod,
    .CallStaticLongMethodV = Cabin_CallStaticLongMethodV,
    .CallStaticLongMethodA = Cabin_CallStaticLongMethodA,

    .CallStaticFloatMethod = Cabin_CallStaticFloatMethod,
    .CallStaticFloatMethodV = Cabin_CallStaticFloatMethodV,
    .CallStaticFloatMethodA = Cabin_CallStaticFloatMethodA,

    .CallStaticDoubleMethod = Cabin_CallStaticDoubleMethod,
    .CallStaticDoubleMethodV = Cabin_CallStaticDoubleMethodV,
    .CallStaticDoubleMethodA = Cabin_CallStaticDoubleMethodA,

    .CallStaticVoidMethod = Cabin_CallStaticVoidMethod,
    .CallStaticVoidMethodV = Cabin_CallStaticVoidMethodV,
    .CallStaticVoidMethodA = Cabin_CallStaticVoidMethodA,

    .GetStaticFieldID = Cabin_GetStaticFieldID,
    .GetStaticObjectField = Cabin_GetStaticObjectField,
    .GetStaticBooleanField = Cabin_GetStaticBooleanField,
    .GetStaticByteField = Cabin_GetStaticByteField,
    .GetStaticCharField = Cabin_GetStaticCharField,
    .GetStaticShortField = Cabin_GetStaticShortField,
    .GetStaticIntField = Cabin_GetStaticIntField,
    .GetStaticLongField = Cabin_GetStaticLongField,
    .GetStaticFloatField = Cabin_GetStaticFloatField,
    .GetStaticDoubleField = Cabin_GetStaticDoubleField,

    .SetStaticObjectField = Cabin_SetStaticObjectField,
    .SetStaticBooleanField = Cabin_SetStaticBooleanField,
    .SetStaticByteField = Cabin_SetStaticByteField,
    .SetStaticCharField = Cabin_SetStaticCharField,
    .SetStaticShortField = Cabin_SetStaticShortField,
    .SetStaticIntField = Cabin_SetStaticIntField,
    .SetStaticLongField = Cabin_SetStaticLongField,
    .SetStaticFloatField = Cabin_SetStaticFloatField,
    .SetStaticDoubleField = Cabin_SetStaticDoubleField,

    .NewString = Cabin_NewString,
    .GetStringLength = Cabin_GetStringLength,
    .GetStringChars = Cabin_GetStringChars,
    .ReleaseStringChars = Cabin_ReleaseStringChars,

    .NewStringUTF = Cabin_NewStringUTF,
    .GetStringUTFLength = Cabin_GetStringUTFLength,
    .GetStringUTFChars = Cabin_GetStringUTFChars,
    .ReleaseStringUTFChars = Cabin_ReleaseStringUTFChars,

    .GetArrayLength = Cabin_GetArrayLength,

    .NewObjectArray = Cabin_NewObjectArray,
    .GetObjectArrayElement = Cabin_GetObjectArrayElement,
    .SetObjectArrayElement = Cabin_SetObjectArrayElement,
    .NewBooleanArray = Cabin_NewBooleanArray,
    .NewByteArray = Cabin_NewByteArray,
    .NewCharArray = Cabin_NewCharArray,
    .NewShortArray = Cabin_NewShortArray,
    .NewIntArray = Cabin_NewIntArray,
    .NewLongArray = Cabin_NewLongArray,
    .NewFloatArray = Cabin_NewFloatArray,
    .NewDoubleArray = Cabin_NewDoubleArray,

    .GetBooleanArrayElements = Cabin_GetBooleanArrayElements,
    .GetByteArrayElements = Cabin_GetByteArrayElements,
    .GetCharArrayElements = Cabin_GetCharArrayElements,
    .GetShortArrayElements = Cabin_GetShortArrayElements,
    .GetIntArrayElements = Cabin_GetIntArrayElements,
    .GetLongArrayElements = Cabin_GetLongArrayElements,
    .GetFloatArrayElements = Cabin_GetFloatArrayElements,
    .GetDoubleArrayElements = Cabin_GetDoubleArrayElements,

    .ReleaseBooleanArrayElements = Cabin_ReleaseBooleanArrayElements,
    .ReleaseByteArrayElements = Cabin_ReleaseByteArrayElements,
    .ReleaseCharArrayElements = Cabin_ReleaseCharArrayElements,
    .ReleaseShortArrayElements = Cabin_ReleaseShortArrayElements,
    .ReleaseIntArrayElements = Cabin_ReleaseIntArrayElements,
    .ReleaseLongArrayElements = Cabin_ReleaseLongArrayElements,
    .ReleaseFloatArrayElements = Cabin_ReleaseFloatArrayElements,
    .ReleaseDoubleArrayElements = Cabin_ReleaseDoubleArrayElements,

    .GetBooleanArrayRegion = Cabin_GetBooleanArrayRegion,
    .GetByteArrayRegion = Cabin_GetByteArrayRegion,
    .GetCharArrayRegion = Cabin_GetCharArrayRegion,
    .GetShortArrayRegion = Cabin_GetShortArrayRegion,
    .GetIntArrayRegion = Cabin_GetIntArrayRegion,
    .GetLongArrayRegion = Cabin_GetLongArrayRegion,
    .GetFloatArrayRegion = Cabin_GetFloatArrayRegion,
    .GetDoubleArrayRegion = Cabin_GetDoubleArrayRegion,

    .SetBooleanArrayRegion = Cabin_SetBooleanArrayRegion,
    .SetByteArrayRegion = Cabin_SetByteArrayRegion,
    .SetCharArrayRegion = Cabin_SetCharArrayRegion,
    .SetShortArrayRegion = Cabin_SetShortArrayRegion,
    .SetIntArrayRegion = Cabin_SetIntArrayRegion,
    .SetLongArrayRegion = Cabin_SetLongArrayRegion,
    .SetFloatArrayRegion = Cabin_SetFloatArrayRegion,
    .SetDoubleArrayRegion = Cabin_SetDoubleArrayRegion,

    .RegisterNatives = Cabin_RegisterNatives,
    .UnregisterNatives = Cabin_UnregisterNatives,

    .MonitorEnter = Cabin_MonitorEnter,
    .MonitorExit = Cabin_MonitorExit,

    .GetJavaVM = Cabin_GetJavaVM,

    .GetStringRegion = Cabin_GetStringRegion,
    .GetStringUTFRegion = Cabin_GetStringUTFRegion,

    .GetPrimitiveArrayCritical = Cabin_GetPrimitiveArrayCritical,
    .ReleasePrimitiveArrayCritical = Cabin_ReleasePrimitiveArrayCritical,

    .GetStringCritical = Cabin_GetStringCritical,
    .ReleaseStringCritical = Cabin_ReleaseStringCritical,

    .NewWeakGlobalRef = Cabin_NewWeakGlobalRef,
    .DeleteWeakGlobalRef = Cabin_DeleteWeakGlobalRef,

    .ExceptionCheck = Cabin_ExceptionCheck,

    .NewDirectByteBuffer = Cabin_NewDirectByteBuffer,
    .GetDirectBufferAddress = Cabin_GetDirectBufferAddress,
    .GetDirectBufferCapacity = Cabin_GetDirectBufferCapacity,

    .GetObjectRefType = Cabin_GetObjectRefType,

    .GetModule = Cabin_GetModule,
};

////////////////////////////////////////////////////////////////////////////////////////////////////

jint JNICALL Cabin_DestroyJavaVM(JavaVM *vm)
{
    JVM_PANIC("not implement.");  //  todo
    return JNI_OK;
}

jint JNICALL Cabin_AttachCurrentThread(JavaVM *vm, void **penv, void *args)
{
    JVM_PANIC("not implement.");  //  todo
    return JNI_OK;
}

jint JNICALL Cabin_DetachCurrentThread(JavaVM *vm)
{
    JVM_PANIC("not implement.");  //  todo
    return JNI_OK;
}

static JNIEnv jni_env;

jint JNICALL Cabin_GetEnv(JavaVM *vm, void **penv, jint version)
{
    assert(penv != NULL);

    if (get_current_thread() == NULL) {
        *penv = NULL;
        return JNI_EDETACHED;
    }

    *penv = &jni_env;
    return JNI_OK;
}

jint JNICALL Cabin_AttachCurrentThreadAsDaemon(JavaVM *vm, void **penv, void *args)
{
    JVM_PANIC("not implement.");  //  todo
    return JNI_OK;
}

const static struct _JNIInvokeInterface Cabin_JNIInvokeInterface = {
    .reserved0 = NULL,
    .reserved1 = NULL,
    .reserved2 = NULL,

    .DestroyJavaVM = Cabin_DestroyJavaVM,
    .AttachCurrentThread = Cabin_AttachCurrentThread,
    .DetachCurrentThread = Cabin_DetachCurrentThread,
    .GetEnv = Cabin_GetEnv,
    .AttachCurrentThreadAsDaemon = Cabin_AttachCurrentThreadAsDaemon,
};

////////////////////////////////////////////////////////////////////////////////////////////////////

void init_jvm(JavaVMInitArgs *);

jint JNICALL JNI_CreateJavaVM(JavaVM **pvm, void **penv, void *args) 
{
    java_vm = &Cabin_JNIInvokeInterface;
    jni_env = &Cabin_JNINativeInterface;

    *pvm = &java_vm;
    *penv = &jni_env;

    JavaVMInitArgs *vm_init_args = (JavaVMInitArgs *) args;
    InitArgs init_args;

    init_jvm(vm_init_args);
    return JNI_OK;
}

jint JNICALL JNI_GetDefaultJavaVMInitArgs(void *args) 
{
    JavaVMInitArgs *vm_init_args = (JavaVMInitArgs*) args;

    // if(!isSupportedJNIVersion(vm_args->version))
    //     return JNI_EVERSION;

    return JNI_OK;
}

jint JNICALL JNI_GetCreatedJavaVMs(JavaVM **buff, jsize buff_len, jsize *num)
 {
    if(buff_len > 0) {
        *buff = &java_vm;
        *num = 1;
        return JNI_OK;
    }
    return JNI_ERR;
}

// jint JNICALL JNI_OnLoad(JavaVM *vm, void *reserved)
// {

// }

// void JNICALL JNI_OnUnload(JavaVM *vm, void *reserved)
// {

// }

JNIEnv *get_jni_env()
{
    return &jni_env;
}