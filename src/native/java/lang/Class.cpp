/*
 * 实现 java/lang/Class 类下的 native方法
 *
 * Author: Jia Yang
 */

#include <algorithm>
#include "../../../interpreter/StackFrame.h"
#include "../../registry.h"
#include "../../../rtda/heap/objectarea/JstringObj.h"
#include "../../../rtda/heap/methodarea/Jmethod.h"
#include "../../../rtda/heap/methodarea/Jfield.h"
#include "../../../jvm.h"
#include "../../../rtda/thread/Jthread.h"
#include "../../../rtda/heap/objectarea/JclassObj.h"
#include "../../../rtda/heap/objectarea/strpool.h"
#include "../../../interpreter/slot/IntSlot.h"

using namespace std;

/*
 * Called after security check for system loader access checks have been made.
 *
 * private static native Class<?> forName0
 *  (String name, boolean initialize, ClassLoader loader, Class<?> caller) throws ClassNotFoundException;
 */
static void forName0(StackFrame *frame) {
    JstringObj *jstrObj = static_cast<JstringObj *>(frame->getLocalVars(0)->getReference());
    string name = jstrToStr(jstrObj->value());
    replace(name.begin(), name.end(), '.', '/');
    Jclass *c = classLoader->loadClass(name);

    int initialize = frame->getLocalVars(1)->getInt();

    if (initialize && !c->isInited) {
        // todo do init
        c->clinit(frame);
        frame->reader->setPc(frame->thread->pc);
    } else {
        frame->operandStack.push(c->getClassObj());
    }
}

/*
 * Return the Virtual Machine's Class object for the named
 * primitive type.
 */
// static native Class<?> getPrimitiveClass(String name);
static void getPrimitiveClass(StackFrame *frame) {
    auto className = static_cast<JstringObj *>(frame->getLocalVars(0)->getReference());
    // 这里得到的className是诸如 "int", "float" 之类的值
    Jclass *c = frame->method->jclass->loader->loadClass(jstrToStr(className->value()));
    frame->operandStack.push(c->getClassObj());
}

/**
 * Returns the  name of the entity (class, interface, array class,
 * primitive type, or void) represented by this {@code Class} object,
 * as a {@code String}.
 *
 * <p> If this class object represents a reference type that is not an
 * array type then the binary name of the class is returned, as specified
 * by
 * <cite>The Java&trade; Language Specification</cite>.
 *
 * <p> If this class object represents a primitive type or void, then the
 * name returned is a {@code String} equal to the Java language
 * keyword corresponding to the primitive type or void.
 *
 * <p> If this class object represents a class of arrays, then the internal
 * form of the name consists of the name of the element type preceded by
 * one or more '{@code [}' characters representing the depth of the array
 * nesting.  The encoding of element type names is as follows:
 *
 * <blockquote><table summary="Element types and encodings">
 * <tr><th> Element Type <th> &nbsp;&nbsp;&nbsp; <th> Encoding
 * <tr><td> boolean      <td> &nbsp;&nbsp;&nbsp; <td align=center> Z
 * <tr><td> byte         <td> &nbsp;&nbsp;&nbsp; <td align=center> B
 * <tr><td> char         <td> &nbsp;&nbsp;&nbsp; <td align=center> C
 * <tr><td> class or interface
 *                       <td> &nbsp;&nbsp;&nbsp; <td align=center> L<i>classname</i>;
 * <tr><td> double       <td> &nbsp;&nbsp;&nbsp; <td align=center> D
 * <tr><td> float        <td> &nbsp;&nbsp;&nbsp; <td align=center> F
 * <tr><td> int          <td> &nbsp;&nbsp;&nbsp; <td align=center> I
 * <tr><td> long         <td> &nbsp;&nbsp;&nbsp; <td align=center> J
 * <tr><td> short        <td> &nbsp;&nbsp;&nbsp; <td align=center> S
 * </table></blockquote>
 *
 * <p> The class or interface name <i>classname</i> is the binary name of
 * the class specified above.
 *
 * <p> Examples:
 * <blockquote><pre>
 * String.class.getName()
 *     returns "java.lang.String"
 * byte.class.getName()
 *     returns "byte"
 * (new Object[3]).getClass().getName()
 *     returns "[Ljava.lang.Object;"
 * (new int[3][4][5][6][7][8][9]).getClass().getName()
 *     returns "[[[[[[[I"
 * </pre></blockquote>
 *
 * @return  the name of the class or interface
 *          represented by this object.
 */
// private native String getName0();
static void getName0(StackFrame *frame) {
    auto thisObj = static_cast<JclassObj *>(frame->getLocalVars(0)->getReference());
    string className = thisObj->getClassName();
    // 这里需要的是 java.lang.Object 这样的类名，而非 java/lang/Object
    // 所以需要进行一下替换
    replace(className.begin(), className.end(), '/', '.');
    frame->operandStack.push(new JstringObj(frame->method->jclass->loader, strToJstr(className)));
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
static void desiredAssertionStatus0(StackFrame *frame) {
    // todo 本书不讨论断言。desiredAssertionStatus0（）方法把false推入操作数栈顶
    frame->operandStack.push(0); // todo pushBoolean?????????????????????????????????????????????
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
static void isInstance(StackFrame *frame) {
    jvmAbort("");
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
 * @exception NullPointerException if the specified Class parameter is
 *            null.
 * @since JDK1.1
 *
 * public native boolean isAssignableFrom(Class<?> cls);
 */
static void isAssignableFrom(StackFrame *frame) {
    jvmAbort("没有实现   isAssignableFrom");
}

/**
 * Determines if the specified {@code Class} object represents an
 * interface type.
 *
 * @return  {@code true} if this object represents an interface;
 *          {@code false} otherwise.
 *
 * public native boolean isInterface();
 */
static void isInterface(StackFrame *frame) {
    jvmAbort("");
}

/**
 * Determines if this {@code Class} object represents an array class.
 *
 * @return  {@code true} if this object represents an array class;
 *          {@code false} otherwise.
 * @since   JDK1.1
 *
 * public native boolean isArray();
 */
static void isArray(StackFrame *frame) {
    jvmAbort("");
}

/**
 * Determines if the specified {@code Class} object represents a
 * primitive type.
 *
 * <p> There are nine predefined {@code Class} objects to represent
 * the eight primitive types and void.  These are created by the Java
 * Virtual Machine, and have the same names as the primitive types that
 * they represent, namely {@code boolean}, {@code byte},
 * {@code char}, {@code short}, {@code int},
 * {@code long}, {@code float}, and {@code double}.
 *
 * <p> These objects may only be accessed via the following public static
 * final variables, and are the only {@code Class} objects for which
 * this method returns {@code true}.
 *
 * @return true if and only if this class represents a primitive type
 *
 * @see     java.lang.Boolean#TYPE
 * @see     java.lang.Character#TYPE
 * @see     java.lang.Byte#TYPE
 * @see     java.lang.Short#TYPE
 * @see     java.lang.Integer#TYPE
 * @see     java.lang.Long#TYPE
 * @see     java.lang.Float#TYPE
 * @see     java.lang.Double#TYPE
 * @see     java.lang.Void#TYPE
 * @since JDK1.1
 *
 * public native boolean isPrimitive();
 */
static void isPrimitive(StackFrame *frame) {
    JclassObj *thisObj = static_cast<JclassObj *>(frame->getLocalVars(0)->getReference());
    bool b = isPrimitiveByClassName(thisObj->getClassName());
    frame->operandStack.push(b ? 1 : 0); // todo
}

/**
 * Returns the {@code Class} representing the superclass of the entity
 * (class, interface, primitive type or void) represented by this
 * {@code Class}.  If this {@code Class} represents either the
 * {@code Object} class, an interface, a primitive type, or void, then
 * null is returned.  If this object represents an array class then the
 * {@code Class} object representing the {@code Object} class is
 * returned.
 *
 * @return the superclass of the class represented by this object.
 *
 * public native Class<? super T> getSuperclass();
 */
static void getSuperclass(StackFrame *frame) {
    jvmAbort("");
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
//public Class<?>[] getInterfaces() {
//    ReflectionData<T> rd = reflectionData();
//    if (rd == null) {
//        // no cloning required
//        return getInterfaces0();
//    } else {
//        Class<?>[] interfaces = rd.interfaces;
//        if (interfaces == null) {
//            interfaces = getInterfaces0();
//            rd.interfaces = interfaces;
//        }
//        // defensively copy before handing over to user code
//        return interfaces.clone();
//    }
//}
//
//private native Class<?>[] getInterfaces0();
static void getInterfaces0(StackFrame *frame) {
    jvmAbort("");
}

/**
 * Returns the {@code Class} representing the component type of an
 * array.  If this class does not represent an array class this method
 * returns null.
 *
 * @return the {@code Class} representing the component type of this
 * class if this class is an array
 * @see     java.lang.reflect.Array
 * @since JDK1.1
 */
//public native Class<?> getComponentType();
static void getComponentType(StackFrame *frame) {
    jvmAbort("");
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
static void getModifiers(StackFrame *frame) {
    jvmAbort("");
}

/**
 * Gets the signers of this class.
 *
 * @return  the signers of this class, or null if there are no signers.  In
 *          particular, this method returns null if this object represents
 *          a primitive type or void.
 * @since   JDK1.1
 */
//public native Object[] getSigners();
static void getSigners(StackFrame *frame) {
    jvmAbort("");
}


/**
 * Set the signers of this class.
 */
//native void setSigners(Object[] signers);
static void setSigners(StackFrame *frame) {
    jvmAbort("");
}

// private native Object[] getEnclosingMethod0();
static void getEnclosingMethod0(StackFrame *frame) {
    jvmAbort("");
}

/**
 * If the class or interface represented by this {@code Class} object
 * is a member of another class, returns the {@code Class} object
 * representing the class in which it was declared.  This method returns
 * null if this class or interface is not a member of any other class.  If
 * this {@code Class} object represents an array class, a primitive
 * type, or void,then this method returns null.
 *
 * @return the declaring class for this class
 * @throws SecurityException
 *         If a security manager, <i>s</i>, is present and the caller's
 *         class loader is not the same as or an ancestor of the class
 *         loader for the declaring class and invocation of {@link
 *         SecurityManager#checkPackageAccess s.checkPackageAccess()}
 *         denies access to the package of the declaring class
 * @since JDK1.1
 */
//@CallerSensitive
//public Class<?> getDeclaringClass() throws SecurityException {
//        final Class<?> candidate = getDeclaringClass0();
//
//        if (candidate != null)
//        candidate.checkPackageAccess(
//        ClassLoader.getClassLoader(Reflection.getCallerClass()), true);
//        return candidate;
//}
//
//private native Class<?> getDeclaringClass0();
static void getDeclaringClass0(StackFrame *frame) {
    jvmAbort("");
}

/*
 * Returns the ProtectionDomain of this class.
 */
//private native java.security.ProtectionDomain getProtectionDomain0();
static void getProtectionDomain0(StackFrame *frame) {
    jvmAbort("");
}

// Generic signature handling
//private native String getGenericSignature0();
static void getGenericSignature0(StackFrame *frame) {
    jvmAbort("");
}


// Annotations handling
//native byte[] getRawAnnotations();
static void getRawAnnotations(StackFrame *frame) {
    jvmAbort("");
}

//// Since 1.8
//native byte[] getRawTypeAnnotations();
static void getRawTypeAnnotations(StackFrame *frame) {
    jvmAbort("");
}


//static byte[] getExecutableTypeAnnotationBytes(Executable ex) {
//return getReflectionFactory().getExecutableTypeAnnotationBytes(ex);
//}
//

// native ConstantPool getConstantPool();
static void getConstantPool(StackFrame *frame) {
    jvmAbort("");
}

// private native Field[] getDeclaredFields0(boolean publicOnly);
static void getDeclaredFields0(StackFrame *frame) {
    JclassObj *thisObj = static_cast<JclassObj *>(frame->getLocalVars(0)->getReference());
    bool publicOnly = frame->getLocalVars(1)->getInt() != 0;  // todo


    Jclass *cls = classLoader->loadClass(thisObj->getClassName());
    vector<Jfield> fields = cls->getFields(publicOnly);

    auto jlrFieldCls = classLoader->loadClass("java/lang/reflect/Field");
    auto jlrFields = new JarrayObj(jlrFieldCls->arrayClass(), fields.size());

    /*
     * Field(Class<?> declaringClass,
     *      String name,
     *      Class<?> type,
     *      int modifiers,
     *      int slot,
     *      String signature,
     *      byte[] annotations)
     */
    auto fieldConstructor = jlrFieldCls->getConstructor(
            "(Ljava/lang/Class;" "Ljava/lang/String;" "Ljava/lang/Class;" "II" "Ljava/lang/String;" "[B)V");
    assert(fieldConstructor != nullptr);

    for (int i = 0; i < fields.size(); i++) {
//        jprintf("%s\n", fields[i].toString().c_str());
        auto jlrFieldObj = new Jobject(jlrFieldCls);

        Jfield &jfield = fields[i];

        // invoke constructor of class java/lang/reflect/Field

        /*
         * 这里必须把字符串放入字符串池中
         * 因为 java/lang/Class 类中 searchFields函数查找field时
         * 名称比较用的 ‘==’ 来比较两个字符串而不是equals，
         * 所以必须保证是从字符串池中拿到的同一个字符串。
         */
        JstringObj *name = putStrToPool(frame->method->jclass->loader, strToJstr(jfield.name));

        jprintf("%s   **************************************************************\n", jstrToStr(name->value()).c_str());
        bool ee = jlrFieldObj->getClass()->isVolatile();
        jprintf("oooooooooooooooooooooooo                  %d\n", ee);
        Slot *args[] = {
            new ReferenceSlot(jlrFieldObj), // this
            new ReferenceSlot(thisObj), // declaringClass
            new ReferenceSlot(name), // name
            new ReferenceSlot(jfield.getType()), // type   todo
            new IntSlot(jfield.accessFlags), // modifiers
            new IntSlot(jfield.id), // slot   todo
            new ReferenceSlot(nullptr), // signature  todo
            new ReferenceSlot(nullptr), // annotations  todo
        };
        frame->invokeMethod(fieldConstructor, args);

        Jvalue v;
        v.r = name;
        jlrFieldObj->setInstanceFieldValue("name", "Ljava/lang/String;", v);

        jlrFields->set(i, jlrFieldObj);
    }

    frame->operandStack.push(jlrFields);
}

// private native Method[] getDeclaredMethods0(boolean publicOnly);
static void getDeclaredMethods0(StackFrame *frame) {
    jvmAbort("");
}

// private native Constructor<T>[] getDeclaredConstructors0(boolean publicOnly);
static void getDeclaredConstructors0(StackFrame *frame) {
    jvmAbort("");
}

// private native Class<?>[] getDeclaredClasses0();
static void getDeclaredClasses0(StackFrame *frame) {
    jvmAbort("");
}


void java_lang_Class_registerNatives() {
    registerNativeMethod("java/lang/Class", "registerNatives", "()V", [](StackFrame *){});
    registerNativeMethod("java/lang/Class", "getPrimitiveClass", "(Ljava/lang/String;)Ljava/lang/Class;", getPrimitiveClass);
    registerNativeMethod("java/lang/Class", "getName0", "()Ljava/lang/String;", getName0);
    registerNativeMethod("java/lang/Class", "forName0",
                         "(Ljava/lang/String;ZLjava/lang/ClassLoader;Ljava/lang/Class;)Ljava/lang/Class;", forName0);
    registerNativeMethod("java/lang/Class", "desiredAssertionStatus0", "(Ljava/lang/Class;)Z", desiredAssertionStatus0);

    registerNativeMethod("java/lang/Class", "isInstance", "(Ljava/lang/Object;)Z", isInstance);
    registerNativeMethod("java/lang/Class", "isAssignableFrom", "(Ljava/lang/Class;)Z", isAssignableFrom);
    registerNativeMethod("java/lang/Class", "isInterface", "()Z", isInterface);
    registerNativeMethod("java/lang/Class", "isArray", "()Z", isArray);
    registerNativeMethod("java/lang/Class", "isPrimitive", "()Z", isPrimitive);

    registerNativeMethod("java/lang/Class", "getSuperclass", "()Ljava/lang/Class;", getSuperclass);
    registerNativeMethod("java/lang/Class", "getInterfaces0", "()[Ljava/lang/Class;", getInterfaces0);
    registerNativeMethod("java/lang/Class", "getComponentType", "()Ljava/lang/Class;", getComponentType);
    registerNativeMethod("java/lang/Class", "getModifiers", "()I", getModifiers);
    registerNativeMethod("java/lang/Class", "getEnclosingMethod0", "()[Ljava/lang/Object;", getEnclosingMethod0);
    registerNativeMethod("java/lang/Class", "getDeclaringClass0", "()Ljava/lang/Class;", getDeclaringClass0);
    registerNativeMethod("java/lang/Class", "getGenericSignature0", "()Ljava/lang/String;", getGenericSignature0);
    registerNativeMethod("java/lang/Class", "getConstantPool", "()Lsun/reflect/ConstantPool;", getConstantPool);

    registerNativeMethod("java/lang/Class", "getRawAnnotations", "()[B", getRawAnnotations);

    registerNativeMethod("java/lang/Class", "getDeclaredFields0", "(Z)[Ljava/lang/reflect/Field;", getDeclaredFields0);
    registerNativeMethod("java/lang/Class", "getDeclaredMethods0", "(Z)[Ljava/lang/reflect/Method;", getDeclaredMethods0);
    registerNativeMethod("java/lang/Class", "getDeclaredConstructors0", "(Z)[Ljava/lang/reflect/Constructor;", getDeclaredConstructors0);
}
