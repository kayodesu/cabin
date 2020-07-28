/*
 * Author: Yo Ka
 */
#include <iostream>
#include "../src/jvmstd.h"
#include "../src/objects/invoke.h"
#include "../src/objects/array_object.h"
#include "../src/objects/string_object.h"

using namespace std;
using namespace method_type;

void initJVM(int argc, char *argv[]);

int main(int argc, char *argv[])
{
    initJVM(argc, argv);

    const char *descriptors[] = {
            "()V",
            "(I)V",
            "(Ljava/lang/Integer;)V",
            "(Ljava/lang/Object;)V",
            "(II[Ljava/lang/Object;)Ljava/lang/Integer;",
            "([Ljava/lang/Object;)Ljava/lang/Object;",
            "([[[Ljava/lang/Object;)[[Ljava/lang/Object;",
            "(ZBSIJFD)[[Ljava/lang/String;",
    };

    for (auto &d : descriptors) {
        cout << "---------------" << endl;
        jref mt = fromMethodDescriptor(d, g_system_class_loader);
        Array *ptypes = parameterTypes(mt); // Class<?>[]
        for (int i = 0; i < ptypes->len; ++i) {
            auto t = ptypes->get<ClassObject *>(i);
            cout << t->jvm_mirror->class_name << endl;
        }
        cout << toMethodDescriptor(mt)->toUtf8() << endl;
    }
}

