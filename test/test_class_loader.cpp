#include <iostream>
#include "../vm/rtda/heap/Object.h"
#include "../vm/objects/class_loader.h"

/*
 * Author: kayo
 */

using namespace std;

void initJVM(int argc, char* argv[]);

int main()
{
    initJVM(0, nullptr);

    Object *scl = getSystemClassLoader();
    cout << scl << endl;
}
