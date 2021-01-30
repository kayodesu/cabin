#include <sstream>
#include "frame.h"

using namespace std;

string Frame::toString() const
{
    ostringstream oss;
    oss << "(" << this << ")";
    if (method->isNative())
        oss << "(native)";
    oss << method->clazz->class_name << "~" << method->name << "~" << method->descriptor;
    oss << ", pc = " << reader.pc;
    return oss.str();
}
