/*
 * Author: Yo Ka
 */

#ifndef KAYO_JNI_INTERFACE_H
#define KAYO_JNI_INTERFACE_H

class Frame;

void initJNI();

void *findNativeMethod(const char *class_name, const char *method_name, const char *method_type);

#endif //KAYO_JNI_INTERFACE_H
