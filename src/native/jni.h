#ifndef CABIN_JNI_H
#define CABIN_JNI_H

struct JNINativeMethod {
    const char *name;
    const char *descriptor;

    const std::type_info &type;
    void *func;
};

void initJNI();

#endif //CABIN_JNI_H
