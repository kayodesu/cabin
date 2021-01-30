#include <iostream>
#include "desc_strs.h"
#include "../src/jvmstd.h"
#include "../src/metadata/method.h"
#include "../src/objects/array_object.h"
#include "../src/metadata/descriptor.h"

using namespace std;

void initJVM(int argc, char *argv[]);

int main(int argc, char *argv[])
{
    initJVM(argc, argv);

    jref scl = getSystemClassLoader();

    for (auto &d : method_descriptors) {
        cout << d << endl;
        cout << numElementsInDescriptor(d + 1, strchr(d, ')')) << " | ";
        pair<Array *, ClassObject *> p = parseMethodDescriptor((char *) d, scl);
        assert(p.first != nullptr);
        assert(p.second != nullptr);
        for (int i = 0; i < p.first->len; i++) {
            auto co = p.first->get<ClassObject *>(i);
            cout << co->jvm_mirror->class_name;
            if (i < p.first->len - 1)
                cout << ", ";
            else
                cout << " | ";
        }
        cout << p.second->jvm_mirror->class_name << endl;

        cout << "--- unparse ---" << unparseMethodDescriptor(p.first, p.second) << endl << endl;
    }

    cout << "---------" << endl;
    cout << unparseMethodDescriptor(nullptr, nullptr) << endl;
}

