#ifndef CABIN_SLOT_H
#define CABIN_SLOT_H

#include <assert.h>
#include <stdint.h>

#include "cabin.h"

// 一个slot_t类型必须可以容纳 jbool, jbyte, jchar, jshort，jint，jfloat, jref 称为类型一
// jlong, jdouble 称为类型二，占两个slot
typedef intptr_t slot_t;

_Static_assert(sizeof(slot_t) >= sizeof(jbool), "");
_Static_assert(sizeof(slot_t) >= sizeof(jbyte), "");
_Static_assert(sizeof(slot_t) >= sizeof(jchar), "");
_Static_assert(sizeof(slot_t) >= sizeof(jshort), "");
_Static_assert(sizeof(slot_t) >= sizeof(jint), "");
_Static_assert(sizeof(slot_t) >= sizeof(jfloat), "");
_Static_assert(sizeof(slot_t) >= sizeof(jref), "");
_Static_assert(2*sizeof(slot_t) >= sizeof(jlong), "");
_Static_assert(2*sizeof(slot_t) >= sizeof(jdouble), "");

#define ISLOT(slot_point) (* (jint *) (slot_point))
#define FSLOT(slot_point) (* (jfloat *) (slot_point))
#define LSLOT(slot_point) (* (jlong *) (slot_point))
#define DSLOT(slot_point) (* (jdouble *) (slot_point))
#define RSLOT(slot_point) (* (jref *) (slot_point))

/* setter */

static inline void slot_set_int(slot_t *slots, jint v)       
{
    assert(slots != NULL); 
    ISLOT(slots) = v; 
}

static inline void slot_set_byte(slot_t *slots, jbyte v)     
{ 
    assert(slots != NULL); 
    slot_set_int(slots, v);
}

static inline void slot_set_bool(slot_t *slots, jbool v)    
{
    assert(slots != NULL); 
    slot_set_int(slots, v); 
}
#define slot_set_boolean slot_set_bool

static inline void slot_set_char(slot_t *slots, jchar v)    
{ 
    assert(slots != NULL); 
    slot_set_int(slots, v); 
}

static inline void slot_set_short(slot_t *slots, jshort v) 
{ 
    assert(slots != NULL);
    slot_set_int(slots, v); 
}

static inline void slot_set_float(slot_t *slots, jfloat v)  
{ 
    assert(slots != NULL);
    FSLOT(slots) = v;
}
    
static inline void slot_set_long(slot_t *slots, jlong v)  
{
    assert(slots != NULL); 
    LSLOT(slots) = v;
}

static inline void slot_set_double(slot_t *slots, jdouble v)
{ 
    assert(slots != NULL);
    DSLOT(slots) = v; 
}

static inline void slot_set_ref(slot_t *slots, jref v)      
{ 
    assert(slots != NULL); 
    RSLOT(slots) = v; 
}

/* getter */

#define slot_get_byte(slot_point)   JINT_TO_JBYTE(ISLOT(slot_point))
#define slot_get_bool(slot_point)   JINT_TO_JBOOL(ISLOT(slot_point))
#define slot_get_boolean slot_get_bool
#define slot_get_char(slot_point)   JINT_TO_JCHAR(ISLOT(slot_point))
#define slot_get_short(slot_point)  JINT_TO_JSHORT(ISLOT(slot_point))
#define slot_get_int(slot_point)    ISLOT(slot_point)
#define slot_get_float(slot_point)  FSLOT(slot_point)
#define slot_get_long(slot_point)   LSLOT(slot_point)
#define slot_get_double(slot_point) DSLOT(slot_point)
#define slot_get_ref(slot_point)    RSLOT(slot_point)

/* builder */

static inline slot_t islot(jint v)
{
    slot_t s;
    slot_set_int(&s, v);
    return s;
}

static inline slot_t fslot(jfloat v)
{
    slot_t s;
    slot_set_float(&s, v);
    return s;
}

static inline slot_t rslot(jref v)
{
    slot_t s;
    slot_set_ref(&s, v);
    return s;
}

#endif // CABIN_SLOT_H