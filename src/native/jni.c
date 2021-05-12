// #include <vector>
#include "jni_internal.h"
#include "../runtime/vm_thread.h"

// using namespace std;

void initJNI()
{
#ifdef R
#undef R
#endif

#define R(method_name) void method_name(); /* declare */ method_name() /* invoke */

    // register all native methods // todo 不要一次全注册，需要时再注册
    R(java_lang_Class_registerNatives);
    R(java_lang_Float_registerNatives);
    R(java_lang_System_registerNatives);
    R(java_lang_Double_registerNatives);
    R(java_lang_Object_registerNatives);
    R(java_lang_String_registerNatives);
    R(java_lang_StringUTF16_registerNatives);
    R(java_lang_Package_registerNatives);
    R(java_lang_Throwable_registerNatives);
    R(java_lang_Thread_registerNatives);
    R(java_lang_Runtime_registerNatives);
    R(java_lang_Shutdown_registerNatives);
    R(java_lang_Module_registerNatives);
    R(java_lang_StackTraceElement_registerNatives);
    R(java_lang_NullPointerException_registerNatives);
    R(java_lang_ClassLoader_registerNatives);
    R(java_lang_ClassLoader$NativeLibrary_registerNatives);

    R(java_lang_reflect_Field_registerNatives);
    R(java_lang_reflect_Executable_registerNatives);
    R(java_lang_reflect_Array_registerNatives);
    R(java_lang_reflect_Proxy_registerNatives);

    R(java_lang_invoke_MethodHandle_registerNatives);
    R(java_lang_invoke_MethodHandleNatives_registerNatives);

    R(java_io_FileDescriptor_registerNatives);
    R(java_io_FileInputStream_registerNatives);
    R(java_io_FileOutputStream_registerNatives);
    R(java_io_WinNTFileSystem_registerNatives);
    R(java_io_RandomAccessFile_registerNatives);

    R(java_nio_Bits_registerNatives);

    R(java_net_InetAddress_registerNatives);
    R(java_net_Inet4Address_registerNatives);
    R(java_net_Inet6Address_registerNatives);
    R(java_net_AbstractPlainSocketImpl_registerNatives);
    R(java_net_AbstractPlainDatagramSocketImpl_registerNatives);
    R(java_net_NetworkInterface_registerNatives);
    R(java_net_PlainSocketImpl_registerNatives);
    R(java_net_InetAddressImplFactory_registerNatives);

    R(sun_misc_VM_registerNatives);
    R(sun_misc_Unsafe_registerNatives);
    R(sun_misc_Signal_registerNatives);
    R(sun_misc_Version_registerNatives);
    R(sun_misc_URLClassPath_registerNatives);
    R(sun_misc_Perf_registerNatives);

    R(sun_io_Win32ErrorMode_registerNatives);

    R(sun_reflect_Reflection_registerNatives);
    R(sun_reflect_NativeConstructorAccessorImpl_registerNatives);
    R(sun_reflect_NativeMethodAccessorImpl_registerNatives);
    R(sun_reflect_ConstantPool_registerNatives);

    R(sun_management_VMManagementImpl_registerNatives);
    R(sun_management_ThreadImpl_registerNatives);

    R(java_security_AccessController_registerNatives);

    R(java_util_TimeZone_registerNatives);
    R(java_util_concurrent_atomic_AtomicLong_registerNatives);
    R(java_util_zip_ZipFile_registerNatives);

    R(jdk_internal_util_SystemProps$Raw_registerNatives);
#undef R
}

#define NATIVE_METHOD_OWNED_CLASS_MAX_COUNT 512 // big enough

struct native_owned_class {
    const utf8_t *class_name;
    JNINativeMethod *methods;
    int methods_count;
} classes[NATIVE_METHOD_OWNED_CLASS_MAX_COUNT];

int classes_count = 0;

// static vector<tuple<const char * /* class name */, JNINativeMethod *, int /* methods count */>> native_methods;

void registerNatives(const char *class_name, JNINativeMethod *methods, int methods_count)
{
    assert(class_name != NULL && methods != NULL && methods_count > 0);
    // native_methods.emplace_back(class_name, methods, methods_count);
    classes[classes_count++] = (struct native_owned_class) { class_name, methods, methods_count };
    assert(classes_count < NATIVE_METHOD_OWNED_CLASS_MAX_COUNT);
}

JNINativeMethod *findNativeMethod(const char *class_name, const char *method_name, const char *method_descriptor)
{
    assert(class_name != NULL && method_name != NULL);

    // for (auto &t: native_methods) {
    //     if (utf8_equals(get<0>(t), class_name)) {
    //         JNINativeMethod *methods = get<1>(t);
    //         for (int i = 0; i < get<2>(t); i++) {
    //             if (utf8_equals(methods[i].name, method_name) && utf8_equals(methods[i].descriptor, method_descriptor)) {
    //                 return methods + i;
    //             }
    //         }
    //         return NULL; // not find
    //     }
    // }

    for (int i = 0; i < classes_count; i++) {
        if (utf8_equals(classes[i].class_name, class_name)) {
            for (int j = 0; j < classes[i].methods_count; j++) {
                if (utf8_equals(classes[i].methods[j].name, method_name) 
                    && utf8_equals(classes[i].methods[j].descriptor, method_descriptor)) {
                    return &(classes[i].methods[j]);
                }
            }
            return NULL; // not find
        }
    }

    return NULL; // not find
}
