/*
 * Author: kayo
 */

#ifndef KAYO_JNI_INTERFACE_H
#define KAYO_JNI_INTERFACE_H

class Frame;

void initJNI();

void *findNativeMethod(const char *class_name, const char *method_name, const char *method_type);

void callJNIMethod(Frame *frame);

#endif //KAYO_JNI_INTERFACE_H
