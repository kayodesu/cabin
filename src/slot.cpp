#include "slot.h"

#define ISLOT(slot_point) (* (jint *) (slot_point))
#define FSLOT(slot_point) (* (jfloat *) (slot_point))
#define LSLOT(slot_point) (* (jlong *) (slot_point))
#define DSLOT(slot_point) (* (jdouble *) (slot_point))
#define RSLOT(slot_point) (* (jref *) (slot_point))

slot_t slot::islot(jint v)
{
    slot_t s;
    setInt(&s, v);
    return s;
}

slot_t slot::fslot(jfloat v)
{
    slot_t s;
    setFloat(&s, v);
    return s;
}

slot_t slot::rslot(jref v)
{
    slot_t s;
    setRef(&s, v);
    return s;
}

slot_t slot::dslot(jdouble v)
{
    slot_t s;
    setDouble(&s, v);
    return s;
}

void slot::setInt(slot_t *slots, jint v)       
{
    assert(slots != nullptr); 
    ISLOT(slots) = v; 
}

void slot::setByte(slot_t *slots, jbyte v)     
{ 
    assert(slots != nullptr); 
    setInt(slots, v);
}

void slot::setBool(slot_t *slots, jbool v)    
{
    assert(slots != nullptr); 
    setInt(slots, v); 
}

void slot::setChar(slot_t *slots, jchar v)    
{ 
    assert(slots != nullptr); 
    setInt(slots, v); 
}

void slot::setShort(slot_t *slots, jshort v) 
{ 
    assert(slots != nullptr);
    setInt(slots, v); 
}

void slot::setFloat(slot_t *slots, jfloat v)  
{ 
    assert(slots != nullptr);
    FSLOT(slots) = v;
}
    
void slot::setLong(slot_t *slots, jlong v)  
{
    assert(slots != nullptr); 
    LSLOT(slots) = v;
}

void slot::setDouble(slot_t *slots, jdouble v)
{ 
    assert(slots != nullptr);
    DSLOT(slots) = v; 
}

void slot::setRef(slot_t *slots, jref v)      
{ 
    assert(slots != nullptr); 
    RSLOT(slots) = v; 
}

jint slot::getInt(const slot_t *slots)
{ 
    assert(slots != nullptr); 
    return ISLOT(slots); 
}

jbyte slot::getByte(const slot_t *slots)
{ 
    assert(slots != nullptr);
    return jint2jbyte(getInt(slots)); 
}

jbool slot::getBool(const slot_t *slots)
{
    assert(slots != nullptr);
    return jint2jbool(getInt(slots)); 
}

jchar slot::getChar(const slot_t *slots)
{
    assert(slots != nullptr);
    return jint2jchar(getInt(slots)); 
}

jshort slot::getShort(const slot_t *slots)
{ 
    assert(slots != nullptr); 
    return jint2jshort(getInt(slots)); 
}

jfloat slot::getFloat(const slot_t *slots)
{ 
    assert(slots != nullptr); 
    return FSLOT(slots); 
}

jlong slot::getLong(const slot_t *slots)
{ 
    assert(slots != nullptr);
    return LSLOT(slots); 
}

jdouble slot::getDouble(const slot_t *slots)
{
    assert(slots != nullptr); 
    return DSLOT(slots);
}

jref slot::getRef(const slot_t *slots)
{
    assert(slots != nullptr);
    return RSLOT(slots);
}