#include <stdbool.h>

#include "cabin.h"
#include "attributes.h"
#include "bytecode_reader.h"
#include "meta.h"
#include "class_loader.h"
#include "symbol.h"

void init_field(Field *f, Class *c, BytecodeReader *r)
{
    assert(f != NULL && c != NULL && r != NULL);

    f->clazz = c;
    ConstantPool *cp = &c->cp;

    f->access_flags = bcr_readu2(r);
    f->name = cp_utf8(cp, bcr_readu2(r));
    f->descriptor = cp_utf8(cp, bcr_readu2(r));

    f->category_two = (f->descriptor[0] == 'J' || f->descriptor[0]== 'D');
    f->deprecated = false;
    f->signature = NULL;

    if (IS_STATIC(f)) {
        memset(&f->static_value, 0, sizeof(f->static_value));
    } else {
        f->id = -1;
    }

    // parse field's attributes
    u2 attr_count = bcr_readu2(r);
    for (int i = 0; i < attr_count; i++) {
        const char *attr_name = cp_utf8(cp, bcr_readu2(r));
        u4 attr_len = bcr_readu4(r);

        if (S(Deprecated) == attr_name) {
            f->deprecated = true;
        } else if (S(ConstantValue) == attr_name) {
            /*
             * ConstantValue属性表示一个常量字段的值。
             * 在一个field_info结构的属性表中最多只能有一个ConstantValue属性。
             *
             * 非静态字段包含了ConstantValue属性，那么这个属性必须被虚拟机所忽略。
             */
            u2 index = bcr_readu2(r);
            if (IS_STATIC(f)) {
                utf8_t d = *f->descriptor;
                if (d == 'Z') {
                    f->static_value.z = JINT_TO_JBOOL(cp_get_int(cp, index));
                } else if (d == 'B') {
                    f->static_value.b = JINT_TO_JBYTE(cp_get_int(cp, index));
                } else if (d == 'C') {
                    f->static_value.c = JINT_TO_JCHAR(cp_get_int(cp, index));
                } else if (d == 'S') {
                    f->static_value.s = JINT_TO_JSHORT(cp_get_int(cp, index));
                } else if (d == 'I') {
                    f->static_value.i = cp_get_int(cp, index);
                } else if (d == 'J') {
                    f->static_value.j = cp_get_long(cp, index);
                } else if (d == 'F') {
                    f->static_value.f = cp_get_float(cp, index);
                } else if (d == 'D') {
                    f->static_value.d = cp_get_double(cp, index);
                } else if(utf8_equals(f->descriptor, S(sig_java_lang_String))) {
                    f->static_value.r = resolve_string(cp, index);
                }
            }
        } else if (S(Synthetic) == attr_name) {
            SET_SYNTHETIC(f);
        } else if (S(Signature) == attr_name) {
            f->signature = cp_utf8(cp, bcr_readu2(r));
        } else if (S(RuntimeVisibleAnnotations) == attr_name) {
            f->rt_visi_annos_count = bcr_readu2(r);
            BUILD_ARRAY(f->rt_visi_annos, f->rt_visi_annos_count, annotation_init, r);
        } else if (S(RuntimeInvisibleAnnotations) == attr_name) {
            f->rt_invisi_annos_count = bcr_readu2(r);
            BUILD_ARRAY(f->rt_invisi_annos, f->rt_invisi_annos_count, annotation_init, r);
        } else {
            // unknown attribute
            bcr_skip(r, attr_len);
        }
    }
}

void init_field0(Field *f, Class *c, const utf8_t *name, const utf8_t *descriptor, int access_flags)
{
    assert(f != NULL && c != NULL && name != NULL && descriptor != NULL);
    
    f->clazz = c;
    f->name = name;
    f->descriptor = descriptor;
    f->access_flags = access_flags;

    f->category_two = (descriptor[0] == 'J' || descriptor[0]== 'D');
    f->deprecated = false;
    f->signature = NULL;

    if (IS_STATIC(f)) {
        memset(&f->static_value, 0, sizeof(f->static_value));
    } else {
        f->id = -1;
    }
}

jclsRef get_field_type(Field *f)
{
    if (*f->descriptor == '[') { // array
        return load_array_class(f->clazz->loader, f->descriptor)->java_mirror;
    } 
    
    if (*f->descriptor == 'L') { // non array Object
        utf8_t buf[strlen(f->descriptor)];
        buf[strlen(strcpy(buf, f->descriptor + 1)) - 1] = 0; // don't include the first 'L' and the last ';'
        return load_class(f->clazz->loader, buf)->java_mirror;
    }

    // primitive
    assert(strlen(f->descriptor) == 1);
    const utf8_t *n = get_prim_class_name(*f->descriptor);
    assert(n != NULL);
    return load_boot_class(n)->java_mirror;
}

bool is_prim_field(const Field *f)
{
    return get_prim_class_name(*f->descriptor) != NULL;
}

char *get_field_info(const Field *f)
{
    char *info = vm_calloc(sizeof(char) * INFO_MSG_MAX_LEN);
    snprintf(info, INFO_MSG_MAX_LEN - 1, "%s~%s~%s~%d", 
                f->clazz->class_name, f->name, f->descriptor, f->id);
    return info;
}
