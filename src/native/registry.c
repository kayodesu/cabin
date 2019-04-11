/*
 * Author: Jia Yang
 */

#include <stdio.h>
#include <string.h>
#include <assert.h>
#include "registry.h"
#include "../jvm.h"
#include "../util/hashmap.h"
#include "../jvmdef.h"

// key 的组成为 "class_name~method_name~method_descriptor" 的形式，
// key 的长度要足够存储上述格式的字符和外加字符串结尾的'\0'
#define KEY_LEN(class_name, method_name, method_descriptor) \
                 (strlen(class_name) + strlen(method_name) + strlen(method_descriptor) + 3 + 1)

static struct hashmap *native_methods;

void registerNatives(Frame *frame) { /* do nothing */ }

void register_native_method(const char *key, native_method_t method)
{
    hashmap_put(native_methods, key, method);
}

native_method_t find_native_method(const char *class_name, const char *method_name, const char *method_descriptor)
{
    assert(class_name != NULL);
    assert(method_name != NULL);
    assert(method_descriptor != NULL);

    size_t key_len = KEY_LEN(class_name, method_name, method_descriptor);
    char key[key_len];
    snprintf(key, key_len - 1, "%s~%s~%s", class_name, method_name, method_descriptor);
    key[key_len - 1] = 0;

    native_method_t method = hashmap_find(native_methods, key);
    if (method != NULL) {
        return method;
    }

    // todo not find;   UnsatisfiedLinkError异常
//    jvm_abort("can't find native function: %s\n", key); /////////////// todo
    return NULL;
}

#define DECLARE_AND_INVOKE(func_name) void func_name(); func_name()

void register_all_native_methods()
{
    native_methods = hashmap_create_str_key(false);

    DECLARE_AND_INVOKE(java_lang_Class_registerNatives);
    DECLARE_AND_INVOKE(java_lang_Float_registerNatives);
    DECLARE_AND_INVOKE(java_lang_System_registerNatives);
    DECLARE_AND_INVOKE(java_lang_Double_registerNatives);
    DECLARE_AND_INVOKE(java_lang_Object_registerNatives);
    DECLARE_AND_INVOKE(java_lang_String_registerNatives);
    DECLARE_AND_INVOKE(java_lang_Throwable_registerNatives);
    DECLARE_AND_INVOKE(java_lang_Thread_registerNatives);
    DECLARE_AND_INVOKE(java_lang_Runtime_registerNatives);
    DECLARE_AND_INVOKE(java_lang_reflect_Field_registerNatives);
    DECLARE_AND_INVOKE(java_lang_reflect_Executable_registerNatives);
    DECLARE_AND_INVOKE(java_lang_ClassLoader_registerNatives);
    DECLARE_AND_INVOKE(java_lang_ClassLoader$NativeLibrary_registerNatives);

    DECLARE_AND_INVOKE(java_io_FileDescriptor_registerNatives);
    DECLARE_AND_INVOKE(java_io_FileInputStream_registerNatives);
    DECLARE_AND_INVOKE(java_io_FileOutputStream_registerNatives);
    DECLARE_AND_INVOKE(java_io_WinNTFileSystem_registerNatives);

    DECLARE_AND_INVOKE(java_nio_Bits_registerNatives);

    DECLARE_AND_INVOKE(sun_misc_VM_registerNatives);
    DECLARE_AND_INVOKE(sun_misc_Unsafe_registerNatives);
    DECLARE_AND_INVOKE(sun_misc_Signal_registerNatives);
    DECLARE_AND_INVOKE(sun_misc_Version_registerNatives);

    DECLARE_AND_INVOKE(sun_io_Win32ErrorMode_registerNatives);

    DECLARE_AND_INVOKE(sun_reflect_Reflection_registerNatives);
    DECLARE_AND_INVOKE(sun_reflect_NativeConstructorAccessorImpl_registerNatives);
    DECLARE_AND_INVOKE(sun_reflect_NativeMethodAccessorImpl_registerNatives);
    DECLARE_AND_INVOKE(sun_reflect_ConstantPool_registerNatives);

    DECLARE_AND_INVOKE(java_security_AccessController_registerNatives);

    DECLARE_AND_INVOKE(java_util_concurrent_atomic_AtomicLong_registerNatives);
}
