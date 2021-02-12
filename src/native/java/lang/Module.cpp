#include "../../jni_internal.h"

// JVM_DefineModule
// private static native void defineModule0(
//                  Module module, boolean isOpen, String version, String location, String[] pns);
static void defineModule0(jobject module, jbool isOpen, jstring version, jstring location, jobject pns)
{
    // todo
    JVM_PANIC("not implement");
}

// JVM_AddReadsModule
// private static native void addReads0(Module from, Module to);
static void addReads0(jobject from, jobject to)
{
    // todo
    JVM_PANIC("not implement");
}

// JVM_AddModuleExports
// private static native void addExports0(Module from, String pn, Module to);
static void addExports0(jobject from, jstring pn, jobject to)
{
    // todo
    JVM_PANIC("not implement");
}

// JVM_AddModuleExportsToAll
// private static native void addExportsToAll0(Module from, String pn);
static void addExportsToAll0(jobject from, jstring pn)
{
    // todo
    JVM_PANIC("not implement");
}

// JVM_AddModuleExportsToAllUnnamed
// private static native void addExportsToAllUnnamed0(Module from, String pn);
static void addExportsToAllUnnamed0(jobject from, jstring pn)
{
    // todo
    JVM_PANIC("not implement");
}

#undef _M
#define _M "(Ljava/lang/Module;"

static JNINativeMethod methods[] = {
        JNINativeMethod_registerNatives,
        { "defineModule0", _M "Z" STR STR "[" STR_ "V", TA(defineModule0) },
        { "addReads0", _M "Ljava/lang/Module;)V", TA(addReads0) },
        { "addExports0", _M STR "Ljava/lang/Module;)V", TA(addExports0) },
        { "addExportsToAll0", _M STR_ "V", TA(addExportsToAll0) },
        { "addExportsToAllUnnamed0", _M STR_ "V", TA(addExportsToAllUnnamed0) },
};

#undef _M

void java_lang_Module_registerNatives()
{
    registerNatives("java/lang/Module", methods, ARRAY_LENGTH(methods));
}
