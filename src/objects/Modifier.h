/*
 * Author: kayo
 */

#ifndef KAYOVM_MODIFIER_H
#define KAYOVM_MODIFIER_H

#include "../jtypes.h"

class Modifier {
private:
    Modifier() = default;

public:
    static constexpr jint MOD_PUBLIC       = 0x00000001; // class field method
    static constexpr jint MOD_PRIVATE      = 0x00000002; //       field method
    static constexpr jint MOD_PROTECTED    = 0x00000004; //       field method
    static constexpr jint MOD_STATIC       = 0x00000008; //       field method
    static constexpr jint MOD_FINAL        = 0x00000010; // class field method
    static constexpr jint MOD_SUPER        = 0x00000020; // class
    static constexpr jint MOD_SYNCHRONIZED = 0x00000020; //             method 方法由管程同步
    static constexpr jint MOD_VOLATILE     = 0x00000040; //       field        表示字段是易变的。
    static constexpr jint MOD_TRANSIENT    = 0x00000080; //       field        表示字段不会被序列化。
    static constexpr jint MOD_NATIVE       = 0x00000100; //             method 方法引用非java语言的本地方法
    static constexpr jint MOD_INTERFACE    = 0x00000200; // class
    static constexpr jint MOD_ABSTRACT     = 0x00000400; // class       method 方法没有具体实现
    static constexpr jint MOD_STRICT       = 0x00000800; //             method 方法使用FP-strict浮点格式
    static constexpr jint MOD_BRIDGE       = 0x00000040; //             method 方法由编译器产生
    static constexpr jint MOD_VARARGS      = 0x00000080; //             method 表示方法带有变长参数
    static constexpr jint MOD_SYNTHETIC    = 0x00001000; // class field method 表示成员由编译器自动产生。
    static constexpr jint MOD_ANNOTATION   = 0x00002000; // class
    static constexpr jint MOD_ENUM         = 0x00004000; // class field        表示字段为枚举类型
    static constexpr jint MOD_MODULE       = 0x00008000; // class              表示class文件描述的是一个模块
    static constexpr jint MOD_MANDATED     = 0x00008000;

    static bool isPublic(jint mod)       { return (mod & MOD_PUBLIC)       != 0; }
    static bool isPrivate(jint mod)      { return (mod & MOD_PRIVATE)      != 0; }
    static bool isProtected(jint mod)    { return (mod & MOD_PROTECTED)    != 0; }
    static bool isStatic(jint mod)       { return (mod & MOD_STATIC)       != 0; }
    static bool isFinal(jint mod)        { return (mod & MOD_FINAL)        != 0; }
    static bool isSuper(jint mod)        { return (mod & MOD_SUPER)        != 0; }
    static bool isSynchronized(jint mod) { return (mod & MOD_SYNCHRONIZED) != 0; }
    static bool isVolatile(jint mod)     { return (mod & MOD_VOLATILE)     != 0; }
    static bool isBridge(jint mod)       { return (mod & MOD_BRIDGE)       != 0; }
    static bool isTransient(jint mod)    { return (mod & MOD_TRANSIENT)    != 0; }
    static bool isVarargs(jint mod)      { return (mod & MOD_VARARGS)      != 0; }
    static bool isNative(jint mod)       { return (mod & MOD_NATIVE)       != 0; }
    static bool isInterface(jint mod)    { return (mod & MOD_INTERFACE)    != 0; }
    static bool isAbstract(jint mod)     { return (mod & MOD_ABSTRACT)     != 0; }
    static bool isStrict(jint mod)       { return (mod & MOD_STRICT)       != 0; }
    static bool isSynthetic(jint mod)    { return (mod & MOD_SYNTHETIC)    != 0; }
    static bool isAnnotation(jint mod)   { return (mod & MOD_ANNOTATION)   != 0; }
    static bool isEnum(jint mod)         { return (mod & MOD_ENUM)         != 0; }
    static bool isModule(jint mod)       { return (mod & MOD_MODULE)       != 0; }
    static bool isMandated(jint mod)     { return (mod & MOD_MANDATED)     != 0; }

    static void setSynthetic(jint &mod)   { mod |= MOD_SYNTHETIC; }
};

#endif //KAYOVM_MODIFIER_H
