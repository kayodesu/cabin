#include <iostream>
#include "desc_strs.h"
#include "../src/jvmstd.h"
#include "../src/metadata/method.h"
#include "../src/objects/mh.h"
#include "../src/objects/array_object.h"
#include "../src/metadata/descriptor.h"

using namespace std;

void initJVM(int argc, char *argv[]);

void printMT(jref mt)
{
    Array *ptypes = mt->getRefField<Array>("ptypes", S(array_java_lang_Class)); // Class<?>[]
    for (int i = 0; i < ptypes->len; ++i) {
        auto t = ptypes->get<ClassObject *>(i);
        cout << t->jvm_mirror->class_name;
        if (i < ptypes->len - 1)
            cout << ", ";
        else
            cout << endl;
    }
}

int main(int argc, char *argv[])
{
    initJVM(argc, argv);

    for (auto &d : method_descriptors) {
        cout << "--------------- " << endl << d << endl;
        jref mt = findMethodType(d, g_system_class_loader);
        printMT(mt);

        string desc = unparseMethodDescriptor(mt);
        cout << desc.c_str() << endl;

        if (strcmp(d, desc.c_str()) != 0)
            throw runtime_error("error");
    }
}

