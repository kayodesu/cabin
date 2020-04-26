#include "cli.h"
#include "jni_inner.h"
#include "../objects/class_loader.h"

using namespace utf8;

extern "C" jobject cli_cloneObject(jobject o)
{
    jref r = to_object_ref(o)->clone();
    return to_jobject(r);
}

extern "C" int cli_isSubclassOf(jclass sub, jclass base)
{
    Class *s = to_object_ref<Class>(sub);
    Class *b = to_object_ref<Class>(base);
    return s->isSubclassOf(b) ? 1 : 0;
}

extern "C" void cli_initClass(jclass clazz)
{
    Class *c = to_object_ref<Class>(clazz);
    initClass(c);
}

extern "C" jstring cli_intern(jstring s)
{
    jref r = to_object_ref(s);
    if (r->clazz != stringClass) {
        // todo error
        jvm_abort("xxxxxxxx");
    }
    
    return to_jstring(stringClass->intern(r));
}

extern "C" jclass cli_loadBootClass(const char *name)
{
    return to_jclass(loadBootClass(dots2SlashDup(name)));
}

extern "C" jclass cli_findLoadedClass(jobject loader, const char *name)
{
    return to_jclass(findLoadedClass(to_object_ref(loader), dots2SlashDup(name)));
}

extern "C" jclass cli_defineClass0(jobject loader, 
                    jstring name, jbyteArray b, jint off, jint len, jobject pd)
{
    Class *c = defineClass(to_object_ref(loader), to_object_ref(name), 
                        to_object_ref<Array>(b), off, len, to_object_ref(pd));
    return to_jclass(c);
}

extern "C" jclass cli_defineClass1(jobject loader, jstring name,
                    jbyteArray b, jint off, jint len, jobject pd, jstring source)
{
    Class *c = defineClass(to_object_ref(loader), to_object_ref(name),
                    to_object_ref<Array>(b), off, len, to_object_ref(pd), to_object_ref(source));
    return to_jclass(c);
}

extern "C" jclass cli_arrayClass(jclass componentClass)
{
    Class *c = to_object_ref<Class>(componentClass);
    return to_jclass(c->arrayClass());
}