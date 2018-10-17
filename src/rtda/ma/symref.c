/*
 * Author: Jia Yang
 */

#include "symref.h"
#include "../../jvm.h"
#include "jfield.h"
#include "jclass.h"

// todo 这些可以放在类的解析阶段（JClass* ClassLoader::resolution）进行吗
struct jclass* resolve_class(const struct jclass *visitor, const char *class_name)
{
    if (class_name == NULL || strlen(class_name) == 0) {
        jvm_abort("class name is empty\n");
    }

    struct jclass *jclass = classloader_load_class(visitor->loader, class_name);
    // todo jclass 的有效性检查
    if (jclass_is_accessible_to(jclass, visitor)) {
        return jclass;
    }

    // todo  java.lang.IllegalAccessError
    jvm_abort("java.lang.IllegalAccessError\n");
}

static struct jmethod* resolve_method_ref(const struct jclass *visitor, struct method_ref *ref, bool is_static)
{
    assert(visitor != NULL);
    assert(ref != NULL);

    if (ref->resolved_method == NULL) {
        ref->resolved_class = resolve_class(visitor, ref->class_name);
        ref->resolved_method = jclass_lookup_method(ref->resolved_class, ref->name, ref->descriptor);
        if (ref->resolved_method == NULL) {
            jvm_abort("error\n"); // todo
        }

        // Note: ref->resolved_class 可能等于 ref->method->jclass，也可能是 ref->method->jclass 的子类
    }

    if (is_static != IS_STATIC(ref->resolved_method->access_flags)) {
        // todo java.lang.IncompatibleClassChangeError
        jvm_abort("java.lang.IncompatibleClassChangeError\n");
    }

    if (!jmethod_is_accessible_to(ref->resolved_method, visitor)) {
        // java.lang.IllegalAccessError   todo
        jmethod_print(ref->resolved_method);
        jclass_print(visitor);
        jvm_abort("java.lang.IllegalAccessError\n");
    }

    return ref->resolved_method;
}

struct jmethod* resolve_static_method_ref(const struct jclass *visitor, struct method_ref *ref)
{
    return resolve_method_ref(visitor, ref, true);
}

struct jmethod* resolve_non_static_method_ref(const struct jclass *visitor, struct method_ref *ref)
{
    return resolve_method_ref(visitor, ref, false);
}

static struct jfield* resolve_field_ref(const struct jclass *visitor, struct field_ref *ref, bool is_static)
{
    assert(visitor != NULL);
    assert(ref != NULL);

    if (ref->resolved_field == NULL) {
        ref->resolved_class = resolve_class(visitor, ref->class_name);
        ref->resolved_field = jclass_lookup_field(ref->resolved_class, ref->name, ref->descriptor);
    }

    if (is_static != IS_STATIC(ref->resolved_field->access_flags)) {
        // todo java.lang.IncompatibleClassChangeError
        printvm("%s~%s~%s, %s\n", ref->class_name, ref->name, ref->descriptor, ref->resolved_class->class_name);
        jvm_abort("java.lang.IncompatibleClassChangeError, %d, %d\n", is_static, IS_STATIC(ref->resolved_field->access_flags));
    }

    if (!jfield_is_accessible_to(ref->resolved_field, visitor)) {
        // java.lang.IllegalAccessError   todo
        jvm_abort("java.lang.IllegalAccessError\n");
    }

    return ref->resolved_field;
}

struct jfield* resolve_static_field_ref(const struct jclass *visitor, struct field_ref *ref)
{
    return resolve_field_ref(visitor, ref, true);
}

struct jfield* resolve_non_static_field_ref(const struct jclass *visitor, struct field_ref *ref)
{
    return resolve_field_ref(visitor, ref, false);
}