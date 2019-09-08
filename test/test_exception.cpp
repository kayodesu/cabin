#include <iostream>
#include "../vm/exceptions.h"

/*
 * Author: kayo
 */

using namespace std;

void initJVM(int argc, char* argv[]);

int main()
{
    initJVM(0, nullptr);
    
    raiseException(CLASS_NOT_FOUND_EXCEPTION);
    
    return 0;
}
