/*
 * Author: Yo Ka
 */

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
    jvm_abort("pageSize");
}

const char *osName()
{
    struct utsname x;
    uname(&x);
    return x.sysname;
}

const char *osArch()
{
    jvm_abort("osArch");
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
    jvm_abort("getTimeZone");
}

#endif
