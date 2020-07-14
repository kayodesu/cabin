/*
 * Author: Yo Ka
 */

#include "../../jni_inner.h"

// JVM_DefineModule
// private static native void defineModule0(
//                  Module module, boolean isOpen, String version, String location, String[] pns);
static void defineModule0(jobject module, jbool isOpen, jstring version, jstring location, jobjectArray pns)
{
    // todo
    jvm_abort("not implement");
}

// JVM_AddReadsModule
// private static native void addReads0(Module from, Module to);
static void addReads0(jobject from, jobject to)
{
    // todo
    jvm_abort("not implement");
}

// JVM_AddModuleExports
// private static native void addExports0(Module from, String pn, Module to);
static void addExports0(jobject from, jstring pn, jobject to)
{
    // todo
    jvm_abort("not implement");
}

// JVM_AddModuleExportsToAll
// private static native void addExportsToAll0(Module from, String pn);
static void addExportsToAll0(jobject from, jstring pn)
{
    // todo
    jvm_abort("not implement");
}

// JVM_AddModuleExportsToAllUnnamed
// private static native void addExportsToAllUnnamed0(Module from, String pn);
static void addExportsToAllUnnamed0(jobject from, jstring pn)
{
    // todo
    jvm_abort("not implement");
}


static JNINativeMethod methods[] = {
        JNINativeMethod_registerNatives,
        { "defineModule0", "(Ljava/lang/Module;Z" STR STR "[Ljava/lang/String;)V", (void *) defineModule0 },
        { "addReads0", "(Ljava/lang/Module;Ljava/lang/Module;)V", (void *) addReads0 },
        { "addExports0", "(Ljava/lang/Module;Ljava/lang/String;Ljava/lang/Module;)V", (void *) addExports0 },
        { "addExportsToAll0", "(Ljava/lang/Module;Ljava/lang/String;)V", (void *) addExportsToAll0 },
        { "addExportsToAllUnnamed0", "(Ljava/lang/Module;Ljava/lang/String;)V", (void *) addExportsToAllUnnamed0 },
};

void java_lang_Module_registerNatives()
{
    registerNatives("java/lang/Module", methods, ARRAY_LENGTH(methods));
}
