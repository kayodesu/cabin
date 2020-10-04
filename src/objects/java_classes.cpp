/*
 * Author: Yo Ka
 */

#include "java_classes.h"
#include "../metadata/class.h"

void java_lang_invoke_MemberName::init()
{
    Class *c = loadBootClass("java/lang/invoke/MemberName");
    // private Class<?> clazz;
//    mn_clazz_field = c->getDeclaredField(S(clazz), S(sig_java_lang_Class));
    // private String name;
//    mn_name_field = c->getDeclaredField(S(name), S(sig_java_lang_String));
    // private Object type;
//    mn_type_field = c->getDeclaredField(S(type), S(sig_java_lang_Object));
    // private int flags;
//    mn_flags_field = c->getDeclaredField(S(flags), S(I));

    // private intptr_t   vmindex;     // member index within class or interface
}
