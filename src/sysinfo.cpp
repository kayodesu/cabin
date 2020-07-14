/*
 * Author: Yo Ka
 */

#include <windows.h> // todo
#include "jvmstd.h"

int processorNumber()
{
    SYSTEM_INFO sysInfo;
    GetSystemInfo(&sysInfo);
    return sysInfo.dwNumberOfProcessors;
}

long freeMemoryCount()
{
    jvm_abort("freeMemory");
}

long totalMemoryCount()
{
    // MEMORYSTATUSEX status;
    // GlobalMemoryStatusEx(&status);
    jvm_abort("totalMemory");
}

long maxMemoryCount()
{
    jvm_abort("maxMemory");
}

char *osName()
{
    jvm_abort("osName");
}

char *osArch()
{
    jvm_abort("osArch");
}