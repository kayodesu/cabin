/*
 * Author: Jia Yang
 */

#ifndef JVM_JVM_H
#define JVM_JVM_H

#include <string>
#include "rtda/heap/methodarea/ClassLoader.h"
#include "rtda/heap/objectarea/Jobject.h"

extern ClassLoader *classLoader;
extern Jobject *mainThreadGroup;

void startJVM(const std::string &mainClassName);

#endif //JVM_JVM_H
