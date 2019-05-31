/*
 * Author: Jia Yang
 */

#include <unordered_map>
#include <cassert>
#include "registry.h"
#include "../symbol.h"
#include "../utf8.h"

using namespace std;

struct MethodInfo {
    const char *class_name, *method_name, *method_descriptor;

    bool operator==(const MethodInfo &x) const
    {
        return utf8_equals(class_name, x.class_name)
               && utf8_equals(method_name, x.method_name)
               && utf8_equals(method_descriptor, x.method_descriptor);
    }
};

struct MethodInfoHash {
    Utf8Hash h;

    size_t operator()(const MethodInfo &x) const noexcept
    {
        return h(x.class_name) ^ h(x.method_name) ^ h(x.method_descriptor);
    }
};

static unordered_map<MethodInfo, native_method_t, MethodInfoHash> nativeMethods;

void register_native_method(
        const char *class_name, const char *method_name, const char *method_descriptor, native_method_t method)
{
    const MethodInfo key = { class_name, method_name, method_descriptor };
    nativeMethods.insert(make_pair(key, method));
}

native_method_t findNativeMethod(const char *class_name, const char *method_name, const char *method_descriptor)
{
    assert(class_name != nullptr);
    assert(method_name != nullptr);
    assert(method_descriptor != nullptr);

    const MethodInfo key = { class_name, method_name, method_descriptor };
    auto iter = nativeMethods.find(key);
    if (iter != nativeMethods.end()) {
        return iter->second;
    }

    // todo not find;   UnsatisfiedLinkError异常
//    jvm_abort("can't find native function: %s~%s~%s\n", key.class_name, key.method_name, key.method_descriptor);
    return nullptr;
}

#undef R
#define R(className, func_name) \
        register_native_method(className, S(registerNatives), S(___V), [](Frame *frame){ }); \
        void func_name(); \
        func_name();

void register_all_native_methods()
{
    R(S(java_lang_Class), java_lang_Class_registerNatives);
    R(S(java_lang_Float), java_lang_Float_registerNatives);
    R(S(java_lang_System), java_lang_System_registerNatives);
    R(S(java_lang_Double), java_lang_Double_registerNatives);
    R(S(java_lang_Object), java_lang_Object_registerNatives);
    R(S(java_lang_String), java_lang_String_registerNatives);
    R(S(java_lang_Throwable), java_lang_Throwable_registerNatives);
    R(S(java_lang_Thread), java_lang_Thread_registerNatives);
    R(S(java_lang_Runtime), java_lang_Runtime_registerNatives);
    R(S(java_lang_reflect_Field), java_lang_reflect_Field_registerNatives);
    R(S(java_lang_reflect_Executable), java_lang_reflect_Executable_registerNatives);
    R(S(java_lang_ClassLoader), java_lang_ClassLoader_registerNatives);
    R(S(java_lang_ClassLoader_NativeLibrary), java_lang_ClassLoader$NativeLibrary_registerNatives);

    R("java/io/FileDescriptor", java_io_FileDescriptor_registerNatives);
    R("java/io/FileInputStream", java_io_FileInputStream_registerNatives);
    R("java/io/FileOutputStream", java_io_FileOutputStream_registerNatives);
    R("java/io/WinNTFileSystem", java_io_WinNTFileSystem_registerNatives);

    R("java/nio/Bits", java_nio_Bits_registerNatives);

    R("sun/misc/VM", sun_misc_VM_registerNatives);
    R("sun/misc/Unsafe", sun_misc_Unsafe_registerNatives);
    R("sun/misc/Signal", sun_misc_Signal_registerNatives);
    R("sun/misc/Version", sun_misc_Version_registerNatives);

    R("sun/io/Win32ErrorMode", sun_io_Win32ErrorMode_registerNatives);

    R("sun/reflect/Reflection", sun_reflect_Reflection_registerNatives);
    R("sun/reflect/NativeConstructorAccessorImpl", sun_reflect_NativeConstructorAccessorImpl_registerNatives);
    R("sun/reflect/NativeMethodAccessorImpl", sun_reflect_NativeMethodAccessorImpl_registerNatives);
    R("sun/reflect/ConstantPool", sun_reflect_ConstantPool_registerNatives);

    R("java/security/AccessController", java_security_AccessController_registerNatives);

    R("java/util/concurrent/atomic/AtomicLong", java_util_concurrent_atomic_AtomicLong_registerNatives);
}
