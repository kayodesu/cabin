/*
 * Author: Yo Ka
 */
#include <iostream>
#include "../../src/jvmstd.h"
#include "../../src/objects/class_loader.h"
#include "../../src/metadata/class.h"

using namespace std;

void initJVM(int argc, char *argv[]);

static void printAllClassLoaders()
{
    unordered_set<const Object *> loaders = getAllClassLoaders();
    for (auto x: loaders) {
        if (x == BOOT_CLASS_LOADER)
            cout << "boot class loader" << endl;
        else
            cout << x->clazz->class_name << endl;
    }
}

int main(int argc, char *argv[])
{
    initJVM(argc, argv);
    
    printAllClassLoaders();
    cout << "---------------" << endl;

    Object *scl = getSystemClassLoader();
    loadClass(scl, "HelloWorld");

    printAllClassLoaders();
}
