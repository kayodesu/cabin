/*
 * Author: Yo Ka
 */

#include <sstream>
#include "frame.h"

using namespace std;

string Frame::toString() const
{
    ostringstream oss;
    oss << "(" << this << ")";
    if (method->isNative())
        oss << "(native)";
    oss << method->clazz->className << "~" << method->name << "~" << method->type;
    oss << ", pc = " << reader.pc;
    return oss.str();
}
