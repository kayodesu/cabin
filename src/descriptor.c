#include "cabin.h"
#include "dynstr.h"


// @b0: include
// @e：exclude
// eg. Ljava/lang/String;
TJE static Object *convert_desc_element_to_class_object(char **b0, const char *e, jref loader)
{
    assert(b0 != NULL && *b0 != NULL && e != NULL && *b0 <= e);
    char *b = *b0;

    if (*b == 'L') { // reference
        char *t = strchr(b, ';');
        if (t == NULL || t >= e) {
            goto error;
        }

        b++; // jump 'L'
        int len = t - b + 1;
        char class_name[len];
        for (int i = 0; i < len; i++) {
            class_name[i] = b[i];
        }
        class_name[len - 1] = 0;
        Class *c = load_class(loader, class_name);

        *b0 = t + 1;
        return c->java_mirror;
    }

    if (*b == '[') { // array reference, 描述符形如 [B 或 [[Ljava/lang/String; 的形式
        char *t = b;
        while (*(++t) == '[');

        if (!is_prim_descriptor(*t)) {
            t = strchr(t, ';');
            if (t == NULL || t >= e) {
                goto error;
            }
        }

        t++;
        int len = t - b + 1;
        char class_name[len];
        for (int i = 0; i < len; i++) {
            class_name[i] = b[i];
        }
        class_name[len - 1] = 0;
        Class *c = load_array_class(loader, class_name);
        *b0 = t;
        return c->java_mirror;
    }

    if (is_prim_descriptor(*b)) { // prim type
        const char *class_name = get_prim_class_name(*b);
        (*b0)++;
        return load_boot_class(class_name)->java_mirror;
    }

error:
    raise_exception(S(java_lang_UnknownError), NULL); // todo msg
    return NULL;
    // throw java_lang_UnknownError(); // todo
}

// @b: include
// @e：exclude
// eg. I[BLjava/lang/String;ZZ, return 5.
static int num_elements_in_descriptor(const char *b, const char *e)
{
    assert(b != NULL && e != NULL);

    int no_params;
    b--;
    for(no_params = 0; ++b < e; no_params++) {
        if(*b == '[')
            while(*++b == '[');
        if(*b == 'L')
            while(*++b != ';');
    }

    return no_params;
}

int num_elements_in_method_descriptor(const char *method_descriptor)
{
    assert(method_descriptor != NULL && method_descriptor[0] == '(');

    const char *b = method_descriptor + 1; // jump '('
    const char *e = strchr(method_descriptor, ')');
    if (e == NULL) {
        // todo error
        JVM_PANIC("error");
    }
    return num_elements_in_descriptor(b, e);
}

//int numElementsInDescriptor(const char *descriptor)
//{
//    assert(descriptor != NULL);
//    return numElementsInDescriptor(descriptor, descriptor + strlen(descriptor));
//}

// @b: include
// @e：exclude
// eg. I[BLjava/lang/String;ZZ
static jarrRef convertDesc2ClassObjectArray(char *b, char *e, jref loader)
{
    int num = num_elements_in_descriptor(b, e);
    jarrRef types = alloc_class_array(num);

    for (int i = 0; b < e; i++) {
        Object *co = convert_desc_element_to_class_object(&b, e, loader);
        assert(i < num);
        array_set_ref(types, i, co);
    }

    return types;
}

bool parse_method_descriptor(const char *desc, jref loader, jarrRef *ptypes, jref *rtype)
{
    assert(desc != NULL);

    char *e = strchr(desc, ')');
    if (e == NULL || *desc != '(') {
        raise_exception(S(java_lang_UnknownError), NULL); // todo msg
        // throw java_lang_UnknownError(); // todo
        return false;
    }

    if (ptypes != NULL) {
        *ptypes = convertDesc2ClassObjectArray((char *) (desc + 1), e, loader);
    }

    e++; // jump ')'

    if (rtype != NULL) {
        *rtype = convert_desc_element_to_class_object(&e, e + strlen(e), loader);
    }

    return true;
}

static void convert_type_to_desc(Class *type, DynStr *desc)
{
    assert(type != NULL);

    if (is_prim_class(type)) {
        dynstr_concat(desc, get_prim_descriptor_by_class_name(type->class_name));
    } else if (is_array_class(type)) {
        dynstr_concat(desc, type->class_name);
    } else {
        // 普通类
        dynstr_concat(desc, "L");
        dynstr_concat(desc, type->class_name);
        dynstr_concat(desc, ";");
    }
}

char *unparse_method_descriptor(jarrRef ptypes /*Class *[]*/, jclsRef rtype)
{
    DynStr desc;
    dynstr_init(&desc);

    if (ptypes == NULL) { // no argument
        dynstr_copy(&desc, "()");
    } else {
        dynstr_copy(&desc, "(");

        for (int i = 0; i < ptypes->arr_len; i++) {
            jclsRef co = array_get(jclsRef, ptypes, i);
            assert(co != NULL);
            convert_type_to_desc(co->jvm_mirror, &desc);
            // oss << convertTypeToDesc(co->jvm_mirror);        
        }

        dynstr_concat(&desc, ")");
    }

    if (rtype == NULL) { // no return value
        dynstr_concat(&desc, "V");
    } else {
        convert_type_to_desc(rtype->jvm_mirror, &desc);
    }

    return desc.buf;
}

char *unparse_method_descriptor0(jref method_type)
{
    assert(method_type != NULL);

    // private final Class<?>[] ptypes;
    jarrRef ptypes = get_ref_field(method_type, "ptypes", S(array_java_lang_Class));
    // private final Class<?> rtype;
    jref rtype = get_ref_field(method_type, "rtype", S(sig_java_lang_Class));

    return unparse_method_descriptor(ptypes, rtype);
}