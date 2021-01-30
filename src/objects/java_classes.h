#ifndef YOVM_JAVA_CLASSES_H
#define YOVM_JAVA_CLASSES_H

class Class;
class Field;

void checkInjectedFields(Class *c);

class java_lang_invoke_MemberName {
    static Field *vmindex;
public:
    void init();
};


#endif //YOVM_JAVA_CLASSES_H
