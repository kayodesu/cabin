/*
 * Author: Yo Ka
 */
#include <iostream>
#include "desc_strs.h"
#include "../src/jvmstd.h"
#include "../src/metadata/method.h"
#include "../src/objects/mh.h"
#include "../src/objects/array_object.h"

using namespace std;
using namespace method_type;

void initJVM(int argc, char *argv[]);

void printMT(jref mt)
{
    Array *ptypes = parameterTypes(mt); // Class<?>[]
    for (int i = 0; i < ptypes->len; ++i) {
        auto t = ptypes->get<ClassObject *>(i);
        cout << t->jvm_mirror->class_name;
        if (i < ptypes->len - 1)
            cout << ", ";
        else
            cout << endl;
    }
    cout << toMethodDescriptor(mt)->toUtf8() << endl;
}

int main(int argc, char *argv[])
{
    initJVM(argc, argv);

    for (auto &d : method_descriptors) {
        cout << "--------------- " << d << endl;
        jref mt = findMethodType(d, g_system_class_loader);
        printMT(mt);
    }
}

