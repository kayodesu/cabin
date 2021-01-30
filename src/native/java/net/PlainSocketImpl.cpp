#include "../../jni_inner.h"

// static native void initIDs();
static void initIDs()
{
    // todo
    jvm_abort("not implement");
}

// static native int socket0(boolean stream) throws IOException;
static jint socket0(jbool stream)
{
    // todo
    jvm_abort("not implement");
}

// static native void bind0(int fd, InetAddress localAddress, int localport, boolean exclBind) throws IOException;
static void bind0(jint fd, jobject localAddress, jint localport, jbool exclBind)
{
    // todo
    jvm_abort("not implement");
}

// static native int connect0(int fd, InetAddress remote, int remotePort) throws IOException;
static jint connect0(jint fd, jobject localAdremotedress, jint remotePort)
{
    // todo
    jvm_abort("not implement");
}

// static native void waitForConnect(int fd, int timeout) throws IOException;
static void waitForConnect(jint fd, jint timeout)
{
    // todo
    jvm_abort("not implement");
}

// static native int localPort0(int fd) throws IOException;
static jint localPort0(jint fd)
{
    // todo
    jvm_abort("not implement");
}

// static native void localAddress(int fd, InetAddressContainer in) throws SocketException;
static void localAddress(jint fd, jobject in)
{
    // todo
    jvm_abort("not implement");
}

// static native void listen0(int fd, int backlog) throws IOException;
static void listen0(jint fd, jint backlog)
{
    // todo
    jvm_abort("not implement");
}

// static native int accept0(int fd, InetSocketAddress[] isaa) throws IOException;
// static native void waitForNewConnection(int fd, int timeout) throws IOException;
// static native int available0(int fd) throws IOException;
// static native void close0(int fd) throws IOException;
// static native void shutdown0(int fd, int howto) throws IOException;
// static native void setIntOption(int fd, int cmd, int optionValue) throws SocketException;
// static native void setSoTimeout0(int fd, int timeout) throws SocketException;
// static native int getIntOption(int fd, int cmd) throws SocketException;
// static native void sendOOB(int fd, int data) throws IOException;
// static native void configureBlocking(int fd, boolean blocking) throws IOException;

static JNINativeMethod methods[] = {
    JNINativeMethod_registerNatives,
    { "initIDs", "()V", (void *) initIDs },
    { "socket0", "(Z)I", (void *) socket0 },
    { "bind0", "(ILjava/net/InetAddress;IZ)V", (void *) bind0 },
    { "connect0", "(ILjava/net/InetAddress;I)I", (void *) connect0 },
    { "waitForConnect", "(II)V", (void *) waitForConnect },
    { "localPort0", "(I)I", (void *) localPort0 },
    { "localAddress", "(ILjava/net/)VInetAddressContainer;", (void *) localAddress },
    { "listen0", "(II)V", (void *) listen0 },
};

void java_net_PlainSocketImpl_registerNatives()
{
    registerNatives("java/net/PlainSocketImpl", methods, ARRAY_LENGTH(methods));
}