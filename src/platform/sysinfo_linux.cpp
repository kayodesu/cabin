#ifdef __linux__

#include <unistd.h>
#include "../jvmstd.h"

using namespace std;

int processorNumber()
{
    return sysconf(_SC_NPROCESSORS_CONF);
}

int pageSize()
{
    JVM_PANIC("pageSize");
}

const char *osName()
{
    struct utsname x;
    uname(&x);
    return x.sysname;
}

const char *osArch()
{
    JVM_PANIC("osArch");
}

const char *getFileSeparator()
{
    return "/";
}

const char *getPathSeparator()
{
    return ":";
}

const char *getLineSeparator()
{
    return "\n";
}

string getTimeZone()
{
    JVM_PANIC("getTimeZone");
}

#endif
