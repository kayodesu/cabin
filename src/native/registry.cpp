/*
 * Author: kayo
 */

#include <unordered_map>
#include <cassert>
#include "registry.h"
#include "../symbol.h"
#include "../util/encoding.h"

using namespace std;
using namespace utf8;

struct MethodInfo {
    const char *class_name, *method_name, *method_descriptor;

    bool operator==(const MethodInfo &x) const
    {
        return equals(class_name, x.class_name)
               && equals(method_name, x.method_name)
               && equals(method_descriptor, x.method_descriptor);
    }
};

struct MethodInfoHash {
    utf8::Hash h;

    size_t operator()(const MethodInfo &x) const noexcept
    {
        return h(x.class_name) ^ h(x.method_name) ^ h(x.method_descriptor);
    }
};

static unordered_map<MethodInfo, native_method_t, MethodInfoHash> nativeMethods;

void registerNative(const char *className,
                    const char *methodName, const char *methodDescriptor, native_method_t method)
{
    assert(className != nullptr);
    assert(methodName != nullptr);
    assert(methodDescriptor != nullptr);
    assert(method != nullptr);

    const MethodInfo key = { className, methodName, methodDescriptor };
    nativeMethods.insert(make_pair(key, method));
}

native_method_t findNative(const char *className, const char *methodName, const char *methodDescriptor)
{
    assert(className != nullptr);
    assert(methodName != nullptr);
    assert(methodDescriptor != nullptr);

    const MethodInfo key = { className, methodName, methodDescriptor };
    auto iter = nativeMethods.find(key);
    if (iter != nativeMethods.end()) {
        return iter->second;
    }

    // todo not find;   UnsatisfiedLinkError异常
//    jvm_abort("can't find native function: %s~%s~%s\n", key.class_name, key.method_name, key.method_descriptor);
    return nullptr;
}


void java_lang_Class_registerNatives();
void java_lang_Float_registerNatives();
void java_lang_System_registerNatives();
void java_lang_Double_registerNatives();
void java_lang_Object_registerNatives();
void java_lang_String_registerNatives();
void java_lang_Package_registerNatives();
void java_lang_Throwable_registerNatives();
void java_lang_Thread_registerNatives();
void java_lang_Runtime_registerNatives();
void java_lang_ClassLoader_registerNatives();
void java_lang_ClassLoader$NativeLibrary_registerNatives();

void java_lang_reflect_Field_registerNatives();
void java_lang_reflect_Executable_registerNatives();
void java_lang_reflect_Array_registerNatives();
void java_lang_reflect_Proxy_registerNatives();

void java_lang_invoke_MethodHandle_registerNatives();
void java_lang_invoke_MethodHandleNatives_registerNatives();

void java_io_FileDescriptor_registerNatives();
void java_io_FileInputStream_registerNatives();
void java_io_FileOutputStream_registerNatives();
void java_io_WinNTFileSystem_registerNatives();
void java_io_RandomAccessFile_registerNatives();

void java_nio_Bits_registerNatives();

void sun_misc_VM_registerNatives();
void sun_misc_Unsafe_registerNatives();
void sun_misc_Signal_registerNatives();
void sun_misc_Version_registerNatives();
void sun_misc_URLClassPath_registerNatives();
void sun_misc_Perf_registerNatives();

void sun_io_Win32ErrorMode_registerNatives();

void sun_reflect_Reflection_registerNatives();
void sun_reflect_NativeConstructorAccessorImpl_registerNatives();
void sun_reflect_NativeMethodAccessorImpl_registerNatives();
void sun_reflect_ConstantPool_registerNatives();

void sun_management_VMManagementImpl_registerNatives();
void sun_management_ThreadImpl_registerNatives();

void java_security_AccessController_registerNatives();

void java_util_concurrent_atomic_AtomicLong_registerNatives();
void java_util_zip_ZipFile_registerNatives();

static struct {
    const char *className;
    void (*registerNatives)();
} natives[] = {
        { S(java_lang_Class), java_lang_Class_registerNatives },
        { S(java_lang_Float), java_lang_Float_registerNatives },
        { S(java_lang_System), java_lang_System_registerNatives },
        { S(java_lang_Double), java_lang_Double_registerNatives },
        { S(java_lang_Object), java_lang_Object_registerNatives },
        { S(java_lang_String), java_lang_String_registerNatives },
        { S(java_lang_Package), java_lang_Package_registerNatives },
        { S(java_lang_Throwable), java_lang_Throwable_registerNatives },
        { S(java_lang_Thread), java_lang_Thread_registerNatives },
        { S(java_lang_Runtime), java_lang_Runtime_registerNatives },
        { S(java_lang_ClassLoader), java_lang_ClassLoader_registerNatives },
        { S(java_lang_ClassLoader_NativeLibrary), java_lang_ClassLoader$NativeLibrary_registerNatives },

        { S(java_lang_reflect_Field), java_lang_reflect_Field_registerNatives },
        { S(java_lang_reflect_Executable), java_lang_reflect_Executable_registerNatives },
        { "java/lang/reflect/Array", java_lang_reflect_Array_registerNatives },
        { "java/lang/reflect/Proxy", java_lang_reflect_Proxy_registerNatives },

        { "java/lang/invoke/MethodHandle", java_lang_invoke_MethodHandle_registerNatives },
        { "java/lang/invoke/MethodHandleNatives", java_lang_invoke_MethodHandleNatives_registerNatives },

        { "java/io/FileDescriptor", java_io_FileDescriptor_registerNatives },
        { "java/io/FileInputStream", java_io_FileInputStream_registerNatives },
        { "java/io/FileOutputStream", java_io_FileOutputStream_registerNatives },
        { "java/io/WinNTFileSystem", java_io_WinNTFileSystem_registerNatives },
        { "java/io/RandomAccessFile", java_io_RandomAccessFile_registerNatives },

        { "java/nio/Bits", java_nio_Bits_registerNatives },

        { "sun/misc/VM", sun_misc_VM_registerNatives },
        { "sun/misc/Unsafe", sun_misc_Unsafe_registerNatives },
        { "sun/misc/Signal", sun_misc_Signal_registerNatives },
        { "sun/misc/Version", sun_misc_Version_registerNatives },
        { "sun/misc/URLClassPath", sun_misc_URLClassPath_registerNatives },
        { "sun/misc/Perf", sun_misc_Perf_registerNatives },

        { "sun/io/Win32ErrorMode", sun_io_Win32ErrorMode_registerNatives },

        { "sun/reflect/Reflection", sun_reflect_Reflection_registerNatives },
        { "sun/reflect/NativeConstructorAccessorImpl", sun_reflect_NativeConstructorAccessorImpl_registerNatives },
        { "sun/reflect/NativeMethodAccessorImpl", sun_reflect_NativeMethodAccessorImpl_registerNatives },
        { "sun/reflect/ConstantPool", sun_reflect_ConstantPool_registerNatives },

        { "sun/management/VMManagementImpl", sun_management_VMManagementImpl_registerNatives },
        { "sun/management/ThreadImpl", sun_management_ThreadImpl_registerNatives },

        { "java/security/AccessController", java_security_AccessController_registerNatives },

        { "java/util/concurrent/atomic/AtomicLong", java_util_concurrent_atomic_AtomicLong_registerNatives },
        { "java/util/zip/ZipFile", java_util_zip_ZipFile_registerNatives },
};

// 注册所有的本地方法  // todo 不要一次全注册，需要时再注册
void register_all_native_methods()
{
    for (auto &native : natives) {
        registerNative(native.className, S(registerNatives), S(___V), [](Frame *frame){ });
        native.registerNatives();
    }
}

void initJNI()
{
    register_all_native_methods();
}
