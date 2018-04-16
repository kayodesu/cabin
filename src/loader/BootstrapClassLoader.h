/*
 * Author: Jia Yang
 */

#ifndef JVM_BOOTSTRAPCLASSLOADER_H
#define JVM_BOOTSTRAPCLASSLOADER_H


#include "ClassLoader.h"

/*
 * BootstrapClassLoader负责加载<JAVA_HOME>\lib目录中的类。
 * 按照文件名识别，仅加载 rt.jar
 */
class BootstrapClassLoader: public ClassLoader {
public:
    void loadJavaLib();
};


#endif //JVM_BOOTSTRAPCLASSLOADER_H
