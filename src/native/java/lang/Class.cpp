#include <vector>
#include "../../../objects/object.h"
#include "../../../metadata/class.h"
#include "../../../metadata/field.h"
#include "../../../objects/array.h"
#include "../../../interpreter/interpreter.h"
#include "../../../runtime/frame.h"
#include "../../../runtime/vm_thread.h"
#include "../../jni_internal.h"
#include "../../../exception.h"

using namespace std;
using namespace utf8;
using namespace slot;

/*
 * Called after security check for system loader access checks have been made.
 *
 * private static native Class<?> forName0
 *  (String name, boolean initialize, ClassLoader loader, Class<?> caller) throws ClassNotFoundException;
 */
static jclass forName0(jstring name, jboolean initialize, jobject loader, jclass caller)
{
    const utf8_t *utf8_name = name->toUtf8(); // 形如 xxx.xx.xx 的形式

    Class *c = loadClass(loader, dot2SlashDup(utf8_name));
    if (c == nullptr) {
        throw java_lang_ClassNotFoundException();
    }
    if (initialize) {
        initClass(c);
    }
    return c->java_mirror;
}

/*
 * Return the Virtual Machine's Class Object for the named primitive type.
 *
 * static native Class<?> getPrimitiveClass(String name);
 */
static jclass getPrimitiveClass(jstring name)
{
    // 这里得到的 class name 是诸如 "int, float" 之类的 primitive type
    const char *class_name = name->toUtf8();;
    return loadBootClass(class_name)->java_mirror;
}

/*
 * Returns the name of the entity (class, interface, array class,
 * primitive type, or void) represented by this Class object, as a String.
 *
 * Examples:
 * String.class.getName()
 *     returns "java.lang.String"
 * byte.class.getName()
 *     returns "byte"
 * (new Object[3]).getClass().getName()
 *     returns "[Ljava.lang.Object;"
 * (new int[3][4][5][6][7][8][9]).getClass().getName()
 *     returns "[[[[[[[I"
 *
 * private native String getName0();
 */
static jstring getName0(jclass _this)
{
    jstrref name = newString(slash2DotDup(_this->jvm_mirror->class_name));
    assert(g_string_class != nullptr);
    return g_string_class->intern(name);
}

/*
 * Cache the name to reduce the number of calls into the VM.
 * This field would be set by VM itself during initClassName call.
 * 
 * private transient String name;
 * private native String initClassName();
 */ 
static jstring initClassName(jclass _this)
{
    jstring class_name = getName0(_this);
    _this->setRefField("name", "Ljava/lang/String;", class_name);
    return class_name;
}

/*
 * Returns the "simple binary name" of the underlying class, i.e.,
 * the binary name without the leading enclosing class name.
 * Returns null if the underlying class is a top level class.
 * 
 * private native String getSimpleBinaryName0();
 */
static jstring getSimpleBinaryName0(jclass _this)
{
    JVM_PANIC("not implement");
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
static jboolean desiredAssertionStatus0()
{
    // todo 本vm不讨论断言。desiredAssertionStatus0（）方法把false推入操作数栈顶
    return jfalse;
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
static jboolean isInstance(jclass _this, jobject obj)
{
    return (obj != nullptr && obj->isInstanceOf(_this->jvm_mirror)) ? jtrue : jfalse;
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
static jboolean isAssignableFrom(jclass _this, jclass cls)
{
    if (cls == nullptr) {
        throw java_lang_NullPointerException();
        return false;
    }

    bool b = cls->jvm_mirror->isSubclassOf(_this->jvm_mirror);
    return b ? jtrue : jfalse;
}

/*
 * Determines if the specified class Object represents an interface type.
 *
 * public native boolean isInterface();
 */
static jboolean isInterface(jclass _this)
{
    return _this->jvm_mirror->isInterface() ? jtrue : jfalse;
}

/*
 * Determines if this class Object represents an array class.
 *
 * public native boolean isArray();
 */
static jboolean isArray(jclass _this)
{
    return _this->jvm_mirror->isArrayClass() ? jtrue : jfalse;  // todo
}

// public native boolean isPrimitive();
static jboolean isPrimitive(jclass _this)
{
    bool b = _this->jvm_mirror->isPrimClass();
    return b ? jtrue : jfalse;
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
static jclass getSuperclass(jclass _this)
{
    Class *c = _this->jvm_mirror;
    if (c->isInterface() || c->isPrimClass() || c->isVoidClass())
        return nullptr;
    if (c->super_class == nullptr)
        return nullptr;
    return c->super_class->java_mirror;
}

/**
 * Determines the interfaces implemented by the class or interface
 * represented by this object.
 *
 * <p> If this object represents a class, the return value is an array
 * containing objects representing all interfaces implemented by the
 * class. The order of the interface objects in the array corresponds to
 * the order of the interface names in the {@code implements} clause
 * of the declaration of the class represented by this object. For
 * example, given the declaration:
 * <blockquote>
 * {@code class Shimmer implements FloorWax, DessertTopping { ... }}
 * </blockquote>
 * suppose the value of {@code s} is an instance of
 * {@code Shimmer}; the value of the expression:
 * <blockquote>
 * {@code s.getClass().getInterfaces()[0]}
 * </blockquote>
 * is the {@code Class} object that represents interface
 * {@code FloorWax}; and the value of:
 * <blockquote>
 * {@code s.getClass().getInterfaces()[1]}
 * </blockquote>
 * is the {@code Class} object that represents interface
 * {@code DessertTopping}.
 *
 * <p> If this object represents an interface, the array contains objects
 * representing all interfaces extended by the interface. The order of the
 * interface objects in the array corresponds to the order of the interface
 * names in the {@code extends} clause of the declaration of the
 * interface represented by this object.
 *
 * <p> If this object represents a class or interface that implements no
 * interfaces, the method returns an array of length 0.
 *
 * <p> If this object represents a primitive type or void, the method
 * returns an array of length 0.
 *
 * <p> If this {@code Class} object represents an array type, the
 * interfaces {@code Cloneable} and {@code java.io.Serializable} are
 * returned in that order.
 *
 * @return an array of interfaces implemented by this class.
 */
//private native Class<?>[] getInterfaces0();
static jobject getInterfaces0(jclass _this)
{
    Class *c = _this->jvm_mirror;
    auto interfaces = newClassArray(c->interfaces.size());
    for (size_t i = 0; i < c->interfaces.size(); i++) {
        assert(c->interfaces[i] != nullptr);
        interfaces->setRef(i, c->interfaces[i]->java_mirror);
    }

    return interfaces;
}

/*
 * Returns the representing the component type of an array.
 * If this class does not represent an array class this method returns null.
 *
 * like:
 * [[I -> [I -> int -> null
 *
 * public native Class<?> getComponentType();
 */
static jclass getComponentType(jclass _this)
{
    Class *c = _this->jvm_mirror;
    if (c->isArrayClass()) {
        return c->componentClass()->java_mirror;
    } else {
        return nullptr;
    }
}

/**
 * Returns the Java language modifiers for this class or interface, encoded
 * in an integer. The modifiers consist of the Java Virtual Machine's
 * constants for {@code public}, {@code protected},
 * {@code private}, {@code final}, {@code static},
 * {@code abstract} and {@code interface}; they should be decoded
 * using the methods of class {@code Modifier}.
 *
 * <p> If the underlying class is an array class, then its
 * {@code public}, {@code private} and {@code protected}
 * modifiers are the same as those of its component type.  If this
 * {@code Class} represents a primitive type or void, its
 * {@code public} modifier is always {@code true}, and its
 * {@code protected} and {@code private} modifiers are always
 * {@code false}. If this object represents an array class, a
 * primitive type or void, then its {@code final} modifier is always
 * {@code true} and its interface modifier is always
 * {@code false}. The values of its other modifiers are not determined
 * by this specification.
 *
 * <p> The modifier encodings are defined in <em>The Java Virtual Machine
 * Specification</em>, table 4.1.
 *
 * @return the {@code int} representing the modifiers for this class
 * @see     java.lang.reflect.Modifier
 * @since JDK1.1
 */
//public native int getModifiers();
static jint getModifiers(jclass _this)
{
    return _this->jvm_mirror->access_flags;
}

/*
 * Gets the signers of this class.
 *
 * the signers of this class, or null if there are no signers.
 * In particular, this method returns null
 * if this object represents a primitive type or void.
 *
 * public native Object[] getSigners();
 */
static jobject getSigners(jclass _this)
{
    JVM_PANIC("getSigners"); // todo
}


/**
 * Set the signers of this class.
 * 
 * native void setSigners(Object[] signers);
 */
static void setSigners(jclass _this, jobject signers)
{
    JVM_PANIC("setSigners"); // todo
}

// private native Object[] getEnclosingMethod0();
static jobject getEnclosingMethod0(jclass _this)
{
    Class *c = _this->jvm_mirror;
    if (c->enclosing.clazz == nullptr) {
        return nullptr;
    }

    auto result = newObjectArray(3);
    result->setRef(0, c->enclosing.clazz->java_mirror);
    result->setRef(1, c->enclosing.name);
    result->setRef(2, c->enclosing.descriptor);

    return result;
}

/*
 * Returns the ProtectionDomain of this class.
 */
// private native java.security.ProtectionDomain getProtectionDomain0();
static jobject getProtectionDomain0(jclass _this)
{
    return nullptr;
    JVM_PANIC("getProtectionDomain0");
}

// Generic signature handling
// private native String getGenericSignature0();
static jstring getGenericSignature0(jclass _this)
{
    Class *c = _this->jvm_mirror;
    if (c->signature != nullptr)
        return newString(c->signature);
    return nullptr;
}

// Annotations handling
//native byte[] getRawAnnotations();
static jobject getRawAnnotations(jclass _this)
{
    JVM_PANIC("getRawAnnotations");
}

// native byte[] getRawTypeAnnotations();
static jobject getRawTypeAnnotations(jclass _this)
{
    JVM_PANIC("getRawTypeAnnotations");
}

// native ConstantPool getConstantPool();
static jobject getConstantPool(jclass _this)
{
    Class *cp_class = loadBootClass("sun/reflect/ConstantPool");
    jobject cp = cp_class->allocObject();
    cp->setRefField("constantPoolOop", OBJ, (jobject) &_this->jvm_mirror->cp); // todo 应该传递一个正在的 Object *
    return cp;
}

// private native Field[] getDeclaredFields0(boolean publicOnly);
static jobject getDeclaredFields0(jclass _this, jboolean public_only)
{
    Class *cls = _this->jvm_mirror;
    jint count = public_only ? cls->public_fields_count : cls->fields.size();

    Class *field_class = loadBootClass(S(java_lang_reflect_Field));
    auto field_array = field_class->arrayClass()->allocArray(count);

    Method *constructor;
    if (!IS_JDK9_PLUS) {        
        //  Field(Class<?> declaringClass, String name, Class<?> type,
        //     int modifiers, int slot, String signature, byte[] annotations)
        constructor = field_class->getConstructor(_CLS STR CLS "II" STR "[B)V");
    } else {
        // Field(Class<?> declaringClass, String name, Class<?> type, int modifiers,
        //     boolean trustedFinal, int slot, String signature, byte[] annotations)
        constructor = field_class->getConstructor(_CLS STR CLS "IZI" STR "[B)V");
    }

    // invoke constructor of class java/lang/reflect/Field
    for (int i = 0; i < count; i++) {
        Object *o = field_class->allocObject();
        field_array->setRef(i, o);

        if (!IS_JDK9_PLUS) {
            execJavaFunc(constructor, {
                    rslot(o), // this
                    rslot(_this), // declaring class
                    // name must be interned.
                    // 参见 java/lang/reflect/Field 的说明
                    rslot(g_string_class->intern(cls->fields[i]->name)), // name
                    rslot(cls->fields[i]->getType()), // type
                    islot(cls->fields[i]->access_flags), /* modifiers todo */
                    islot(cls->fields[i]->id), /* slot   todo */
                    rslot(cls->fields[i]->signature != nullptr ? newString(cls->fields[i]->signature) : jnull), /* signature  todo */
                    rslot(jnull), /* annotations  todo */
            });
        } else {
            execJavaFunc(constructor, {
                    rslot(o), // this
                    rslot(_this), // declaring class
                    // name must be interned.
                    // 参见 java/lang/reflect/Field 的说明
                    rslot(g_string_class->intern(cls->fields[i]->name)), // name
                    rslot(cls->fields[i]->getType()), // type
                    islot(cls->fields[i]->access_flags), /* modifiers todo */
                    islot(cls->fields[i]->isFinal() ? jtrue : jfalse), // todo trustedFinal
                    islot(cls->fields[i]->id), /* slot   todo */
                    rslot(cls->fields[i]->signature != nullptr ? newString(cls->fields[i]->signature) : jnull), /* signature  todo */
                    rslot(jnull), /* annotations  todo */
            });
        }
    }
    
    return field_array;
}

/*
 * 注意 getDeclaredMethods 和 getMethods 方法的不同。
 * getDeclaredMethods(),该方法是获取本类中的所有方法，包括私有的(private、protected、默认以及public)的方法。
 * getMethods(),该方法是获取本类以及父类或者父接口中所有的公共方法(public修饰符修饰的)
 *
 * getDeclaredMethods 强调的是本类中定义的方法，不包括继承而来的。
 * 不包括 class initialization method(<clinit>)和构造函数(<init>)
 *
 * private native Method[] getDeclaredMethods0(boolean publicOnly);
 */
static jobject getDeclaredMethods0(jclass _this, jboolean public_only)
{
    Class *cls = _this->jvm_mirror;
    jint count = public_only ? cls->public_methods_count : cls->methods.size();

    Class *method_class = loadBootClass(S(java_lang_reflect_Method));

    /*
     * Method(Class<?> declaringClass, String name, Class<?>[] parameterTypes, Class<?> returnType,
     *      Class<?>[] checkedExceptions, int modifiers, int slot, String signature,
     *      byte[] annotations, byte[] parameterAnnotations, byte[] annotationDefault)
     */
    Method *constructor = method_class->getConstructor(_CLS STR "[" CLS CLS "[" CLS "II" STR "[B[B[B)V");

    vector<jref> methods;
    for (int i = 0; i < count; i++) {
        Method *method = cls->methods[i];
        if (method->isClassInit() || method->isObjectInit()) {
            continue;
        }
        Object *o = method_class->allocObject();
        methods.push_back(o);

        execJavaFunc(constructor, {
                rslot(o),        /* this  */
                rslot(_this), /* declaring class */
                // name must be interned.
                // 参见 java/lang/reflect/Method 的说明
                rslot(g_string_class->intern(method->name)), /* name */
                rslot(method->getParameterTypes()), /* parameter types */
                rslot(method->getReturnType()),     /* return type */
                rslot(method->getExceptionTypes()), /* checked exceptions */
                islot(method->access_flags), /* modifiers todo */
                islot(i), /* slot   todo */
                rslot(method->signature != nullptr ? newString(method->signature) : jnull), /* signature  todo */
                rslot(jnull), /* annotations  todo */
                rslot(jnull), /* parameter annotations  todo */
                rslot(jnull), /* annotation default  todo */
        });
    }
        
    Array *method_array = method_class->arrayClass()->allocArray(methods.size());
    for (size_t i = 0; i < methods.size(); i++) {
        method_array->setRef(i, methods[i]);
    }
    return method_array;
}

// private native Constructor<T>[] getDeclaredConstructors0(boolean publicOnly);
static jobject getDeclaredConstructors0(jclass _this, jboolean public_only)
{
   Class *cls = _this->jvm_mirror;

    std::vector<Method *> constructors = cls->getConstructors(public_only);
    int count = constructors.size();

    Class *constructor_class = loadBootClass("java/lang/reflect/Constructor");
    auto constructor_array = constructor_class->arrayClass()->allocArray(count);

    /*
     * Constructor(Class<T> declaringClass, Class<?>[] parameterTypes,
     *      Class<?>[] checkedExceptions, int modifiers, int slot,
     *      String signature, byte[] annotations, byte[] parameterAnnotations)
     */
    Method *constructor_constructor = constructor_class->getConstructor(_CLS "[" CLS "[" CLS "II" STR "[B[B)V");

    // invoke constructor of class java/lang/reflect/Constructor
    for (int i = 0; i < count; i++) {
        auto constructor = constructors[i];
        Object *o = constructor_class->allocObject();
        constructor_array->setRef(i, o);

        execJavaFunc(constructor_constructor, {
                rslot(o), // this
                rslot(_this), // declaring class
                rslot(constructor->getParameterTypes()),  // parameter types
                rslot(constructor->getExceptionTypes()),  // checked exceptions
                islot(constructor->access_flags), // modifiers todo
                islot(i), // slot   todo
                rslot(jnull), // signature  todo
                rslot(jnull), // annotations  todo
                rslot(jnull), // parameter annotations  todo
        });
    }
    
    return constructor_array;
}

/*
 * getClasses和getDeclaredClasses的区别：
 * getClasses得到该类及其父类所有的public的内部类。
 * getDeclaredClasses得到该类所有的内部类，除去父类的。
 *
 * private native Class<?>[] getDeclaredClasses0();
 */
static jobject getDeclaredClasses0(jclass _this)
{
    JVM_PANIC("getDeclaredClasses0");
}

/**
 * If the class or interface represented by this {@code Class} object
 * is a member of another class, returns the {@code Class} object
 * representing the class in which it was declared.  This method returns
 * null if this class or interface is not a member of any other class.  If
 * this {@code Class} object represents an array class, a primitive
 * type, or void,then this method returns null.
 *
 * 如果此类为内部类，返回其外部类
 *
 * @return the declaring class for this class
 * @throws SecurityException
 *         If a security manager, <i>s</i>, is present and the caller's
 *         class loader is not the same as or an ancestor of the class
 *         loader for the declaring class and invocation of {@link
 *         SecurityManager#checkPackageAccess s.checkPackageAccess()}
 *         denies access to the package of the declaring class
 * @since JDK1.1
 *
 * private native Class<?> getDeclaringClass0();
 */
static jclass getDeclaringClass0(jclass _this)
{
    Class *c = _this->jvm_mirror;
    if (c->isArrayClass()) {
        return nullptr;
    }

    char buf[strlen(c->class_name) + 1];
    strcpy(buf, c->class_name);
    char *last_dollar = strrchr(buf, '$'); // 内部类标识：out_class_name$inner_class_name
    if (last_dollar == nullptr) {
        return nullptr;
    } 
    
    *last_dollar = 0;
    c = loadClass(c->loader, buf);
    assert(c != nullptr);
    return c->java_mirror;
}

// private native RecordComponent[] getRecordComponents0();
static jobject getRecordComponents0(jclass _this)
{
    JVM_PANIC("getRecordComponents0");
}

// private native boolean isRecord0();
static jbool isRecord0(jclass _this)
{
    JVM_PANIC("isRecord0");
}

// private native Class<?> getNestHost0();
static jclass getNestHost0(jclass _this)
{
    JVM_PANIC("getNestHost0");
}

// private native Class<?>[] getNestMembers0();
static jobject getNestMembers0(jclass _this)
{
    JVM_PANIC("getNestMembers0");
}

/**
 * Returns {@code true} if and only if the underlying class is a hidden class.
 *
 * @return {@code true} if and only if this class is a hidden class.
 *
 * @since 15
 * @see MethodHandles.Lookup#defineHiddenClass
 * 
 * @HotSpotIntrinsicCandidate
 * public native boolean isHidden();
 */
static jboolean isHidden(jclass _this)
{
    return false; // todo
}     

static JNINativeMethod methods[] = {
        JNINativeMethod_registerNatives,
        { "getPrimitiveClass", _STR_ CLS, TA(getPrimitiveClass) },
        { "getName0", __STR, TA(getName0) },
        { "initClassName", __STR, TA(initClassName) },
        { "getSimpleBinaryName0", __STR, TA(getSimpleBinaryName0) },
        { "forName0", _STR "ZLjava/lang/ClassLoader;" CLS_ CLS, TA(forName0) },
        { "desiredAssertionStatus0", _CLS_ "Z", TA(desiredAssertionStatus0) },

        { "isInstance", _OBJ_ "Z", TA(isInstance) },
        { "isAssignableFrom", _CLS_ "Z", TA(isAssignableFrom) },
        { "isInterface", "()Z", TA(isInterface) },
        { "isArray", "()Z", TA(isArray) },
        { "isPrimitive", "()Z", TA(isPrimitive) },

        { "getSuperclass", __CLS, TA(getSuperclass) },
        { "getInterfaces0", "()[Ljava/lang/Class;", TA(getInterfaces0) },
        { "getComponentType", __CLS, TA(getComponentType) },
        { "getModifiers", "()I", TA(getModifiers) },
        { "getEnclosingMethod0", "()[Ljava/lang/Object;", TA(getEnclosingMethod0) },
        { "getDeclaringClass0", __CLS, TA(getDeclaringClass0) },
        { "getGenericSignature0", __STR, TA(getGenericSignature0) },
        { "getProtectionDomain0", "()Ljava/security/ProtectionDomain;", TA(getProtectionDomain0) },
        { "getConstantPool", "()Lsun/reflect/ConstantPool;", TA(getConstantPool) },

        { "getSigners", "()[Ljava/lang/Object;", TA(getSigners) },
        { "setSigners", "([Ljava/lang/Object;)V", TA(setSigners) },

        { "getRawAnnotations", "()[B", TA(getRawAnnotations) },
        { "getRawTypeAnnotations", "()[B", TA(getRawTypeAnnotations) },

        { "getDeclaredFields0", "(Z)[Ljava/lang/reflect/Field;", TA(getDeclaredFields0) },
        { "getDeclaredMethods0", "(Z)[Ljava/lang/reflect/Method;", TA(getDeclaredMethods0) },
        { "getDeclaredConstructors0", "(Z)[Ljava/lang/reflect/Constructor;", TA(getDeclaredConstructors0) },
        { "getDeclaredClasses0", "()[Ljava/lang/Class;", TA(getDeclaredClasses0) },

        { "getRecordComponents0", "()[Ljava/lang/reflect/RecordComponent;", TA(getRecordComponents0) },
        { "isRecord0", "()Z", TA(isRecord0) },
        { "getNestHost0", __CLS, TA(getNestHost0) },
        { "getNestMembers0", "()[Ljava/lang/Class;", TA(getNestMembers0) },
        { "isHidden", "()Z", TA(isHidden) },
};

void java_lang_Class_registerNatives()
{
    registerNatives("java/lang/Class", methods, ARRAY_LENGTH(methods));
}
