/*
 * Author: Yo Ka
 */
#include <iostream>
#include "../../src/jvmstd.h"
#include "../../src/objects/class_loader.h"
#include "../../src/metadata/class.h"

using namespace std;

void initJVM(int argc, char *argv[]);

static void print(Class *c)
{
    cout << "---------------" << endl;
    cout << c->class_name << ", ";
    cout << c->java_mirror->jvm_mirror->class_name << ", ";
    cout << (c == c->java_mirror->jvm_mirror) << endl;
}

int main(int argc, char *argv[])
{
    initJVM(argc, argv);

//    printBootLoadedClasses();
    print(loadBootClass("boolean"));
    print(loadClass(getSystemClassLoader(), "HelloWorld"));
}
