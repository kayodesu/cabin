#include "../../jni_internal.h"

// private static native NetworkInterface[] getAll() throws SocketException;
static jobject getAll()
{
    // todo
    JVM_PANIC("not implement");
}

// private static native NetworkInterface getByName0(String name) throws SocketException;
static jobject getByName0(jstring name)
{
    // todo
    JVM_PANIC("not implement");
}

// private static native NetworkInterface getByIndex0(int index) throws SocketException;
static jobject getByIndex0(jint index)
{
    // todo
    JVM_PANIC("not implement");
}

// private static native NetworkInterface getByInetAddress0(InetAddress addr) throws SocketException;
static jobject getByInetAddress0(jobject addr)
{
    // todo
    JVM_PANIC("not implement");
}

// private static native boolean isUp0(String name, int ind) throws SocketException;
static jbool isUp0(jstring name, jint ind)
{
    // todo
    JVM_PANIC("not implement");
}

// private static native boolean isLoopback0(String name, int ind) throws SocketException;
static jbool isLoopback0(jstring name, jint ind)
{
    // todo
    JVM_PANIC("not implement");
}

// private static native boolean supportsMulticast0(String name, int ind) throws SocketException;
static jbool supportsMulticast0(jstring name, jint ind)
{
    // todo
    JVM_PANIC("not implement");
}


// private static native boolean isP2P0(String name, int ind) throws SocketException;
static jbool isP2P0(jstring name, jint ind)
{
    // todo
    JVM_PANIC("not implement");
}

// private static native byte[] getMacAddr0(byte[] inAddr, String name, int ind) throws SocketException;
static jobject getMacAddr0(jobject inAddr, jstring name, jint ind)
{
    // todo
    JVM_PANIC("not implement");
}

// private static native int getMTU0(String name, int ind) throws SocketException;
static jint getMTU0(jstring name, jint ind)
{
    // todo
    JVM_PANIC("not implement");
}

// private static native void init();
static void init()
{
    // todo
    JVM_PANIC("not implement");
}

static JNINativeMethod methods[] = {
    JNINativeMethod_registerNatives,
    { "getAll", "()[Ljava/net/NetworkInterface;", TA(getAll) },
    { "getByName0", _STR_ "Ljava/net/NetworkInterface;", TA(getByName0) },
    { "getByIndex0", "(I)Ljava/net/NetworkInterface;", TA(getByIndex0) },
    { "getByInetAddress0", "(Ljava/net/InetAddress;)Ljava/net/NetworkInterface;", TA(getByInetAddress0) },
    { "isUp0", "(Ljava/lang/String;I)Z", TA(isUp0) },
    { "isLoopback0", "(Ljava/lang/String;I)Z", TA(isLoopback0) },
    { "supportsMulticast0", "(Ljava/lang/String;I)Z", TA(supportsMulticast0) },
    { "isP2P0", "(Ljava/lang/String;I)Z", TA(isP2P0) },
    { "getMacAddr0", "([BLjava/lang/String;I)[B", TA(getMacAddr0) },
    { "getMTU0", "(Ljava/lang/String;I)I", TA(getMTU0) },
    { "init", "()V", TA(init) },
};

void java_net_NetworkInterface_registerNatives()
{
    registerNatives("java/net/NetworkInterface", methods, ARRAY_LENGTH(methods));
}