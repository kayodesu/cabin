#include <iostream>
#include "desc_strs.h"
#include "../src/jvmstd.h"
#include "../src/metadata/method.h"
#include "../src/objects/array_object.h"
#include "../src/metadata/descriptor.h"

using namespace std;

void initJVM(int argc, char *argv[]);

#define LEN 20

void testCloneIntArray()
{
    cout << "-------------------" << endl;

    Array *x = newArray(loadArrayClass("[I"), LEN);
    for (int i = 0; i < LEN; ++i) {
        x->setInt(i, i);
    }

    Array *x0 = (Array *) x->clone();
    for (int i = 0; i < LEN; ++i) {
        x0->setInt(i, i + 100);
    }

    cout << x << ", " << x->len << endl;
    for (int i = 0; i < LEN; ++i) {
        cout << x->get<jint>(i);
        if (i == LEN - 1)
            cout << "." << endl;
        else
            cout << ", ";
    }

    cout << x0 << ", " << x0->len << endl;
    for (int i = 0; i < LEN; ++i) {
        cout << x0->get<jint>(i);
        if (i == LEN - 1)
            cout << "." << endl;
        else
            cout << ", ";
    }
}

void testCloneStringArray()
{
    cout << "-------------------" << endl;

    Array *x = newArray(loadArrayClass("[Ljava/lang/String;"), LEN);
    for (int i = 0; i < LEN; ++i) {
        ostringstream oss;
        oss << i;
        x->setRef(i, newString(oss.str().c_str()));
    }

    Array *x0 = (Array *) x->clone();
    for (int i = 0; i < LEN; ++i) {
        ostringstream oss;
        oss << i + 100;
        x0->setRef(i, newString(oss.str().c_str()));
    }

    cout << x << ", " << x->len << endl;
    for (int i = 0; i < LEN; ++i) {
        cout << x->get<Object *>(i)->toUtf8();
        if (i == LEN - 1)
            cout << "." << endl;
        else
            cout << ", ";
    }

    cout << x0 << ", " << x0->len << endl;
    for (int i = 0; i < LEN; ++i) {
        cout << x0->get<Object *>(i)->toUtf8();
        if (i == LEN - 1)
            cout << "." << endl;
        else
            cout << ", ";
    }
}

int main(int argc, char *argv[])
{
    initJVM(argc, argv);

    testCloneIntArray();
    testCloneStringArray();

    return 0;
}
