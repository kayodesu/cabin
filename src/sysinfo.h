/*
 * Author: Yo Ka
 */

#ifndef JVM_SYSINFO_H
#define JVM_SYSINFO_H

int processorNumber();

long freeMemoryCount();

long totalMemoryCount();

long maxMemoryCount();

// 返回操作系统的名称。e.g. window 10
char *osName();

// 返回操作系统的架构。e.g. amd64
char *osArch();

#endif