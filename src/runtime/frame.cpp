/*
 * Author: kayo
 */

#include <sstream>
#include "frame.h"
#include "../objects/class.h"

using namespace std;


//Frame::Frame(Method *m, bool vm_invoke, slot_t *lvars, slot_t *ostack, Frame *prev)
//        : method(m), reader(m->code, m->codeLen), vm_invoke(vm_invoke), lvars(lvars), ostack(ostack), prev(prev)
//{
//    assert(m != nullptr);
//}

//size_t Frame::size(const Method *m)
//{
//    assert(m != nullptr);
//    return sizeof(Frame) + (m->maxStack*sizeof(slot_t)) + (m->maxLocals*sizeof(slot_t));
//}
//
//size_t Frame::size() const
//{
//    assert(method != nullptr);
//    return size(method);
//}

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
