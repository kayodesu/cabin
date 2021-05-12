#include "../../../objects/object.h"
#include "../../../metadata/class.h"
#include "../../../metadata/field.h"
#include "../../../interpreter/interpreter.h"
#include "../../../runtime/frame.h"
#include "../../../runtime/vm_thread.h"
#include "../../jni_internal.h"
#include "../../../exception.h"
#include "../../../jvm.h"


/*
 * Called after security check for system loader access checks have been made.
 *
 * private static native Class<?> forName0
 *  (String name, boolean initialize, ClassLoader loader, Class<?> caller) throws ClassNotFoundException;
 */
static jclsref forName0(JNIEnv *env, jclass cls, jstrref name, jboolean initialize, jref loader, jclsref caller)
{
    const utf8_t *utf8_name = string_to_utf8(name); // 形如 xxx.xx.xx 的形式

    Class *c = load_class(loader, dot_to_slash_dup(utf8_name));
    if (c == NULL) {
        raise_exception(S(java_lang_ClassNotFoundException), NULL); // todo msg
        // throw java_lang_ClassNotFoundException();
    }
    if (initialize) {
        init_class(c);
    }
    return c->java_mirror;
}

/*
 * Return the Virtual Machine's Class Object for the named primitive type.
 *
 * static native Class<?> getPrimitiveClass(String name);
 */
static jclsref getPrimitiveClass(JNIEnv *env, jclass cls, jstrref name)
{
    // 这里得到的 class name 是诸如 "int, float" 之类的 primitive type
    const char *class_name = string_to_utf8(name);
    return JVM_FindPrimitiveClass(env, class_name);
}

/**
 * Returns the assertion status that would be assigned to this
 * class if it were to be initialized at the time this method is invoked.
 * If this class has had its assertion status set, the most recent
 * setting will be returned; otherwise, if any package default assertion
 * status pertains to this class, the most recent setting for the most
 * specific pertinent package default assertion status is returned;
 * otherwise, if this class is not a system class (i.e., it has a
 * class loader) its class loader's default assertion status is returned;
 * otherwise, the system class default assertion status is returned.
 * <p>
 * Few programmers will have any need for this method; it is provided
 * for the benefit of the JRE itself.  (It allows a class to determine at
 * the time that it is initialized whether assertions should be enabled.)
 * Note that this method is not guaranteed to return the actual
 * assertion status that was (or will be) associated with the specified
 * class when it was (or will be) initialized.
 *
 * @return the desired assertion status of the specified class.
 * @see    java.lang.ClassLoader#setClassAssertionStatus
 * @see    java.lang.ClassLoader#setPackageAssertionStatus
 * @see    java.lang.ClassLoader#setDefaultAssertionStatus
 * @since  1.4
 */
//public boolean desiredAssertionStatus() {
//    ClassLoader loader = getClassLoader();
//    // If the loader is null this is a system class, so ask the VM
//    if (loader == null)
//        return desiredAssertionStatus0(this);
//
//    // If the classloader has been initialized with the assertion
//    // directives, ask it. Otherwise, ask the VM.
//    synchronized(loader.assertionLock) {
//        if (loader.classAssertionStatus != null) {
//            return loader.desiredAssertionStatus(getName());
//        }
//    }
//    return desiredAssertionStatus0(this);
//}
//

/*
 * Retrieves the desired assertion status of this class from the VM
 *
 * private static native boolean desiredAssertionStatus0(Class<?> clazz);
 */
static jboolean desiredAssertionStatus0(JNIEnv *env, jclass cls, jclass clazz)
{
    // todo 本vm不讨论断言。desiredAssertionStatus0（）方法把false推入操作数栈顶
    return false;
}

/**
 * Determines if the specified {@code Object} is assignment-compatible
 * with the object represented by this {@code Class}.  This method is
 * the dynamic equivalent of the Java language {@code instanceof}
 * operator. The method returns {@code true} if the specified
 * {@code Object} argument is non-null and can be cast to the
 * reference type represented by this {@code Class} object without
 * raising a {@code ClassCastException.} It returns {@code false}
 * otherwise.
 *
 * <p> Specifically, if this {@code Class} object represents a
 * declared class, this method returns {@code true} if the specified
 * {@code Object} argument is an instance of the represented class (or
 * of any of its subclasses); it returns {@code false} otherwise. If
 * this {@code Class} object represents an array class, this method
 * returns {@code true} if the specified {@code Object} argument
 * can be converted to an object of the array class by an identity
 * conversion or by a widening reference conversion; it returns
 * {@code false} otherwise. If this {@code Class} object
 * represents an interface, this method returns {@code true} if the
 * class or any superclass of the specified {@code Object} argument
 * implements this interface; it returns {@code false} otherwise. If
 * this {@code Class} object represents a primitive type, this method
 * returns {@code false}.
 *
 * @param   obj the object to check
 * @return  true if {@code obj} is an instance of this class
 *
 * @since JDK1.1
 *
 * public native boolean isInstance(Object obj);
 */
static jboolean isInstance(JNIEnv *env, jclsref this, jref obj)
{
    return (obj != NULL && is_instance_of(obj, this->jvm_mirror)) ? true : false;
}

/**
 * Determines if the class or interface represented by this
 * {@code Class} object is either the same as, or is a superclass or
 * superinterface of, the class or interface represented by the specified
 * {@code Class} parameter. It returns {@code true} if so;
 * otherwise it returns {@code false}. If this {@code Class}
 * object represents a primitive type, this method returns
 * {@code true} if the specified {@code Class} parameter is
 * exactly this {@code Class} object; otherwise it returns
 * {@code false}.
 *
 * <p> Specifically, this method tests whether the type represented by the
 * specified {@code Class} parameter can be converted to the type
 * represented by this {@code Class} object via an identity conversion
 * or via a widening reference conversion. See <em>The Java Language
 * Specification</em>, sections 5.1.1 and 5.1.4 , for details.
 *
 * @param cls the {@code Class} object to be checked
 * @return the {@code boolean} value indicating whether objects of the
 * type {@code cls} can be assigned to objects of this class
 * @exception NullPointerException if the specified Class parameter is null.
 * @since JDK1.1
 *
 * public native boolean isAssignableFrom(Class<?> cls);
 */
static jboolean isAssignableFrom(JNIEnv *env, jclsref this, jclsref cls)
{
    if (cls == NULL) {
        // throw java_lang_NullPointerException();
        raise_exception(S(java_lang_NullPointerException), NULL);
        return false;
    }

    bool b = is_subclass_of(cls->jvm_mirror, this->jvm_mirror);
    return b ? true : false;
}

/**
 * Returns the Class representing the superclass of the entity
 * (class, interface, primitive type or void) represented by this
 * Class.  If this Class represents either the Object class, 
 * an interface, a primitive type, or void, then null is returned.  
 * If this object represents an array class then the
 * Class object representing the Object class is returned.
 *
 * @return the superclass of the class represented by this object.
 *
 * public native Class<? super T> getSuperclass();
 */
static jclsref getSuperclass(JNIEnv *env, jclsref this)
{
    return (*env)->GetSuperclass(env, this);
}

static JNINativeMethod methods[] = {
        JNINativeMethod_registerNatives,
        { "getPrimitiveClass", _STR_ CLS, getPrimitiveClass },
        // { "getName0", __STR, JVM_GetClassName },
        { "initClassName", __STR, JVM_InitClassName },
        { "getSimpleBinaryName0", __STR, JVM_GetSimpleBinaryName },
        { "forName0", _STR "ZLjava/lang/ClassLoader;" CLS_ CLS, forName0 },
        { "desiredAssertionStatus0", _CLS_ "Z", desiredAssertionStatus0 },

        { "isInstance", _OBJ_ "Z", isInstance },
        { "isAssignableFrom", _CLS_ "Z", isAssignableFrom },
        { "isInterface", "()Z", JVM_IsInterface },
        { "isArray", "()Z", JVM_IsArrayClass },
        { "isPrimitive", "()Z", JVM_IsPrimitiveClass },

        { "getSuperclass", __CLS, getSuperclass },
        { "getInterfaces0", "()[Ljava/lang/Class;", JVM_GetClassInterfaces },
        // { "getComponentType", __CLS, JVM_GetComponentType },
        { "getModifiers", "()I", JVM_GetClassModifiers },
        { "getEnclosingMethod0", "()[Ljava/lang/Object;", JVM_GetEnclosingMethodInfo },
        { "getDeclaringClass0", __CLS, JVM_GetDeclaringClass },
        { "getGenericSignature0", __STR, JVM_GetClassSignature },
        { "getProtectionDomain0", "()Ljava/security/ProtectionDomain;", JVM_GetProtectionDomain },
        { "getConstantPool", "()Lsun/reflect/ConstantPool;", JVM_GetClassConstantPool },

        { "getSigners", "()[Ljava/lang/Object;", JVM_GetClassSigners },
        { "setSigners", "([Ljava/lang/Object;)V", JVM_SetClassSigners },

        { "getRawAnnotations", "()[B", JVM_GetClassAnnotations },
        { "getRawTypeAnnotations", "()[B", JVM_GetClassTypeAnnotations },

        { "getDeclaredFields0", "(Z)[Ljava/lang/reflect/Field;", JVM_GetClassDeclaredFields },
        { "getDeclaredMethods0", "(Z)[Ljava/lang/reflect/Method;", JVM_GetClassDeclaredMethods },
        { "getDeclaredConstructors0", "(Z)[Ljava/lang/reflect/Constructor;", JVM_GetClassDeclaredConstructors },
        { "getDeclaredClasses0", "()[Ljava/lang/Class;", JVM_GetDeclaredClasses },

        { "getRecordComponents0", "()[Ljava/lang/reflect/RecordComponent;", JVM_GetRecordComponents },
        { "isRecord0", "()Z", JVM_IsRecord },
        { "getNestHost0", __CLS, JVM_GetNestHost },
        { "getNestMembers0", "()[Ljava/lang/Class;", JVM_GetNestMembers },
        { "isHidden", "()Z", JVM_IsHiddenClass },
        { NULL }
};

void java_lang_Class_registerNatives()
{
    register_natives("java/lang/Class", methods);
}
