/*
 * Author: Yo Ka
 */
#include <iostream>
#include "../src/jvmstd.h"
#include "../src/objects/invoke.h"
#include "../src/objects/string_object.h"

using namespace std;
using namespace method_type;

void initJVM(int argc, char *argv[]);

int main(int argc, char *argv[])
{
    initJVM(argc, argv);

    const char *descriptors[] = {
            "(I)V",
            "(Ljava/lang/Integer;)V",
            "(Ljava/lang/Object;)V",
            "(II[Ljava/lang/Object;)Ljava/lang/Integer;",
            "([Ljava/lang/Object;)Ljava/lang/Object;",
            "([[[Ljava/lang/Object;)[[Ljava/lang/Object;",
    };

    for (size_t i = 0; i < sizeof(descriptors)/sizeof(*descriptors); i++) {
        jref mh = fromMethodDescriptor(descriptors[i], g_system_class_loader);
        cout << toMethodDescriptor(mh)->toUtf8() << endl;
    }
}

