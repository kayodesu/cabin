/*
 * Author: Yo Ka
 */

#ifndef JVM_SYSINFO_H
#define JVM_SYSINFO_H

int processorNumber();

int pageSize();

long freeMemoryCount();

long totalMemoryCount();

long maxMemoryCount();

// 返回操作系统的名称。e.g. window 10
const char *osName();

// 返回操作系统的架构。e.g. amd64
const char *osArch();

// return timezone now.
//std::wstring getTimeZone();

#endif