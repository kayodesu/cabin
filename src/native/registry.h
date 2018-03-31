/*
 * Author: Jia Yang
 */

#ifndef JVM_REGISTRY_H
#define JVM_REGISTRY_H

#include <string>
#include <functional>
#include <utility>

#include "../interpreter/StackFrame.h"

//struct NativeMethod {
//    std::string className;
//    std::string methodName;
//    std::string methodDescriptor;
//    std::function<void(StackFrame *)> nativeMethod;
//
//    NativeMethod(std::string className0, std::string methodName0,
//                 std::string methodDescriptor0, std::function<void(StackFrame *)> nativeMethod0)
//            : className(std::move(className0)), methodName(std::move(methodName0)),
//              methodDescriptor(std::move(methodDescriptor0)), nativeMethod(std::move(nativeMethod0)){}
//
//    static bool equals(const NativeMethod &m1, const NativeMethod &m2) {
//        return true;
//    }
//
//    struct Cmp {
//        bool operator()(const NativeMethod &m1, const NativeMethod &m2) {
//            return m1.className == m2.className
//                   and m1.methodName == m2.methodName
//                   and m1.methodDescriptor == m2.methodDescriptor;
//        }
//    };
//};

// 注册所有的本地方法
void registerAllNativeMethods();

// 注册本地方法
void registerNativeMethod(const std::string &className,
                          const std::string &methodName,
                          const std::string &methodDescriptor,
                          std::function<void(StackFrame *)> nativeMethod);

std::function<void(StackFrame *)> findNativeMethod(const std::string &className,
                                                    const std::string &methodName,
                                                    const std::string &methodDescriptor);

void printRegisteredNativeMethods();

#endif //JVM_REGISTRY_H
