#include <cassert>
#include "../../../../symbol.h"
#include "../../../jni_internal.h"
#include "../../../../objects/class_loader.h"
#include "../../../../metadata/method.h"
#include "../../../../metadata/class.h"
#include "../../../../interpreter/interpreter.h"

// private static native void initialize();
static void initialize()
{
    // todo
    Class *sys = loadBootClass(S(java_lang_System));
    initClass(sys);

    if (IS_JDK9_PLUS) {
        Method *m = sys->lookupStaticMethod("initPhase1", S(___V));
        assert(m != nullptr);
        execJavaFunc(m);

        //   todo
        printvm("initPhase2 is not implement\n");
//    m = sys->lookupStaticMethod("initPhase2", "(ZZ)I");
//    assert(m != nullptr);
//    jint ret = slot::getInt(execJavaFunc(m, {slot::islot(1), slot::islot(1)}));
//    assert(ret == 0); // 等于0表示成功

        m = sys->lookupStaticMethod("initPhase3", S(___V));
        assert(m != nullptr);
        execJavaFunc(m);
    } else {
        Method *m = sys->lookupStaticMethod("initializeSystemClass", S(___V));
        assert(m != nullptr);
        execJavaFunc(m);
    }
}

/*
 * Returns the first non-null class loader up the execution stack,
 * or null if only code from the null class loader is on the stack.
 *
 * public static native ClassLoader latestUserDefinedLoader();
 */
static jobject latestUserDefinedLoader()
{
    JVM_PANIC("latestUserDefinedLoader"); // todo
}

// public static void initializeFromArchive(Class c);
static void initializeFromArchive(jref c)
{
    return; // todo
    JVM_PANIC("initializeFromArchive"); // todo
}

static jlong getRandomSeedForCDSDump()
{
//    printvm("getRandomSeedForCDSDump\n");
    return 100; // todo
    JVM_PANIC("getRandomSeedForCDSDump"); // todo
}

static JNINativeMethod methods[] = {
        JNINativeMethod_registerNatives,
        { "initialize", "()V", TA(initialize) },
        { "latestUserDefinedLoader", "()Ljava/lang/ClassLoader;", TA(latestUserDefinedLoader) },
        { "initializeFromArchive", "(Ljava/lang/Class;)V", TA(initializeFromArchive) },
        { "getRandomSeedForCDSDump", "()J", TA(getRandomSeedForCDSDump) },
};

void sun_misc_VM_registerNatives()
{
    if (IS_JDK9_PLUS) {
        registerNatives("jdk/internal/misc/VM", methods, ARRAY_LENGTH(methods));
    } else {
        registerNatives("sun/misc/VM", methods, ARRAY_LENGTH(methods));
    }
}
