#ifndef CABIN_SYSINFO_H
#define CABIN_SYSINFO_H

int processorNumber();

int pageSize();

// 返回操作系统的名称。e.g. window 10
const char *osName();

// 返回操作系统的架构。e.g. amd64
const char *osArch();

const char *getFileSeparator();
const char *getPathSeparator();
const char *getLineSeparator();

std::string getTimeZone();

#endif // CABIN_SYSINFO_H