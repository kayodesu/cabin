#include "../../jni_inner.h"
#include "../../../metadata/constant_pool.h"
#include "../../../metadata/class.h"

// private native int getSize0(Object constantPoolOop);
static jint getSize0(jobject _this, jobject constantPoolOop)
{
    ConstantPool *cp = (ConstantPool *) constantPoolOop;
    return cp->size;
}

// private native Class getClassAt0(Object constantPoolOop, int i);
static jclass getClassAt0(jobject _this, jobject constantPoolOop, jint i)
{
    ConstantPool *cp = (ConstantPool *) constantPoolOop;
    return cp->resolveClass((u2)i)->java_mirror;
}

// private native long getLongAt0(Object constantPoolOop, int i);
static jlong getLongAt0(jobject _this, jobject constantPoolOop, jint i)
{
    ConstantPool *cp = (ConstantPool *) constantPoolOop;
    return cp->_long((u2)i);
}

// private native String getUTF8At0(Object constantPoolOop, int i);
static jstring getUTF8At0(jobject _this, jobject constantPoolOop, jint i)
{
    ConstantPool *cp = (ConstantPool *) constantPoolOop;
    return cp->resolveString(i);
}

static JNINativeMethod methods[] = {
        JNINativeMethod_registerNatives,
        { "getSize0", "(Ljava/lang/Object;)I", (void *) getSize0 },
        { "getClassAt0", "(Ljava/lang/Object;I)Ljava/lang/Class;", (void *) getClassAt0 },
        { "getLongAt0", "(Ljava/lang/Object;I)J", (void *) getLongAt0 },
        { "getUTF8At0", "(Ljava/lang/Object;I)Ljava/lang/String;", (void *) getUTF8At0 },
};

void sun_reflect_ConstantPool_registerNatives()
{
    registerNatives("sun/reflect/ConstantPool", methods, ARRAY_LENGTH(methods));
}
