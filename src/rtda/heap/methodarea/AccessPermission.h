/*
 * Author: Jia Yang
 */

#ifndef JVM_ACCESSPERMISSION_H
#define JVM_ACCESSPERMISSION_H

#include "../../../jvm.h"


#define ACC_PUBLIC       0x0001 // class field method
#define ACC_PRIVATE      0x0002 //       field method
#define ACC_PROTECTED    0x0004 //       field method
#define ACC_STATIC       0x0008 //       field method
#define ACC_FINAL        0x0010 // class field method
/*
 * ACC_SUPER标志用于确定该Class文件里面的invokespecial指令使用的是哪一种执行语义。
 * 目前Java虚拟机的编译器都应当设置这个标志。
 * ACC_SUPER标记是为了向后兼容旧编译器编译的Class文件而存在的，
 * 在JDK1.0.2版本以前的编译器产生的Class文件中，access_flag里面没有ACC_SUPER标志。
 * 同时，JDK1.0.2前的Java虚拟机遇到ACC_SUPER标记会自动忽略它。
 */
#define ACC_SUPER        0x0020 // class
#define ACC_SYNCHRONIZED 0x0020 //             method 方法由管程同步
#define ACC_VOLATILE     0x0040 //       field        表示字段是易变的。
#define ACC_BRIDGE       0x0040 //             method 方法由编译器产生
#define ACC_TRANSIENT    0x0080 //       field        表示字段不会被序列化。
#define ACC_VARARGS      0x0080 //             method 表示方法带有变长参数
#define ACC_NATIVE       0x0100 //             method 方法引用非java语言的本地方法
#define ACC_INTERFACE    0x0200 // class
#define ACC_ABSTRACT     0x0400 // class       method 方法没有具体实现
#define ACC_STRICT       0x0800 //             method 方法使用FP-strict浮点格式
#define ACC_SYNTHETIC    0x1000 // class field method 表示成员由编译器自动产生。
#define ACC_ANNOTATION   0x2000 // class
#define ACC_ENUM         0x4000 // class field        表示字段为枚举类型


struct AccessPermission {
    u2 accessFlags;
    
    bool isPublic() const {
        return ((accessFlags) & ACC_PUBLIC) != 0;
    }

    bool isPrivate() const {
        return ((accessFlags) & ACC_PRIVATE) != 0;
    }

    bool isProtected() const {
        return ((accessFlags) & ACC_PROTECTED) != 0;
    }

    bool isStatic() const {
        return ((accessFlags) & ACC_STATIC) != 0;
    }

    bool isFinal() const {
        return ((accessFlags) & ACC_FINAL) != 0;
    }

    bool isSuper() const {
        return (((accessFlags) & ACC_SUPER) != 0);
    }

    bool isSynchronized() const {
        return ((accessFlags) & ACC_SYNCHRONIZED) != 0;
    }

    bool isVolatile() const {
        return ((accessFlags) & ACC_VOLATILE) != 0;
    }

    bool isBridge() const {
        return ((accessFlags) & ACC_BRIDGE) != 0;
    }

    bool isTransient() const {
        return (((accessFlags) & ACC_TRANSIENT) != 0);
    }

    bool isVarargs() const {
        return ((accessFlags) & ACC_VARARGS) != 0;
    }

    bool isNative() const {
        return ((accessFlags) & ACC_NATIVE) != 0;
    }

    bool isInterface() const {
        return ((accessFlags) & ACC_INTERFACE) != 0;
    }

    bool isAbstract() const {
        return ((accessFlags) & ACC_ABSTRACT) != 0;
    }

    bool isStrict() const {
        return ((accessFlags) & ACC_STRICT) != 0;
    }

    bool isSynthetic() const {
        return ((accessFlags) & ACC_SYNTHETIC) != 0;
    }

    bool isAnnotation() const {
        return ((accessFlags) & ACC_ANNOTATION) != 0;
    }

    bool isEnum() const {
        return ((accessFlags) & ACC_ENUM) != 0;
    }

    void setSynthetic() {
        (accessFlags) |= ACC_SYNTHETIC;  // todo
    }
};

#endif //JVM_ACCESSPERMISSION_H
