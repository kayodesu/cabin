#ifndef CABIN_JAVA_CLASSES_H
#define CABIN_JAVA_CLASSES_H

class Class;
class Field;

void checkInjectedFields(Class *c);

class java_lang_invoke_MemberName {
    static Field *vmindex;
public:
    void init();
};


#endif // CABIN_JAVA_CLASSES_H
