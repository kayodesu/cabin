/*
 * Author: Yo Ka
 */
#include <iostream>
#include "../src/jvmstd.h"
#include "../src/metadata/method.h"
#include "../src/objects/invoke.h"
#include "../src/objects/array_object.h"
#include "../src/objects/string_object.h"
#include "../src/interpreter/interpreter.h"

using namespace std;
using namespace method_type;

void initJVM(int argc, char *argv[]);

int main(int argc, char *argv[])
{
    initJVM(argc, argv);

    const char *descriptors[] = {
            "()V",
            "(I)V",
            "(B)C",
            "(Ljava/lang/Integer;)V",
            "(Ljava/lang/Object;)V",
            "(II[Ljava/lang/Object;)Ljava/lang/Integer;",
            "([Ljava/lang/Object;)Ljava/lang/Object;",
            "([[[Ljava/lang/Object;)[[Ljava/lang/Object;",
            "(ZBSIJFD)[[Ljava/lang/String;",
            "(ZZZZZZZZZZZZZZZZ)Z",
    };

    for (auto &d : descriptors) {
        cout << "---------------" << endl;
        jref mt = fromMethodDescriptor(d, g_system_class_loader);
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

    //////////////////////////////////////////////////////////
    cout << "---------------" << endl;

    Class *c = loadBootClass("sun/invoke/util/BytecodeDescriptor");
    Method *m = c->getDeclaredStaticMethod("parseMethod",
                               "(Ljava/lang/String;Ljava/lang/ClassLoader;)Ljava/util/List;");
    jref list = RSLOT(execJavaFunc(m, { newString("(ZZZZZZZZZZZZZZZZ)Z"), g_system_class_loader }));
    Array *arr = list->getRefField<Array>("elementData", S(array_java_lang_Object));
    cout << arr->get<ClassObject *>(0)->jvm_mirror->class_name << endl;
    cout << arr->get<ClassObject *>(1)->jvm_mirror->class_name << endl;
}

