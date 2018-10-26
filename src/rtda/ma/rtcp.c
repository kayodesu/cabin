/*
 * Author: Jia Yang
 */

#include <stdlib.h>
#include "rtcp.h"
#include "../../classfile/constant.h"
#include "../../util/encoding.h"
#include "../../util/convert.h"
#include "../../slot.h"
#include "symref.h"
#include "../../classfile/attribute.h"


static bool check(const struct rtcp *rtcp, int index, int type)
{
    assert(rtcp != NULL);
    assert(index > 0 && index < rtcp->count);

    if (rtcp->pool[index].t != type) {
        // todo error
        if (rtcp->pool[index].t == INTERFACE_METHOD_REF_CONSTANT) {
            struct method_ref *ref = rtcp->pool[index].v.p;
            printvm("%s~%s~%s\n", ref->class_name, ref->name, ref->descriptor);
        }
        jvm_abort("error. want %d, bug tag = %d.\n", type, rtcp->pool[index].t);
        return false;
    }
    return true;
}

static bool check2(const struct rtcp *rtcp, int index, int type1, int type2)
{
    assert(rtcp != NULL);
    assert(index > 0 && index < rtcp->count);

    if (rtcp->pool[index].t != type1 && rtcp->pool[index].t != type2) {
        // todo error
        jvm_abort("error. want %d or %d, bug tag = %d.\n", type1, type2, rtcp->pool[index].t);
        return false;
    }
    return true;
}

static void build_utf8_constant(struct rtcp *rtcp, void **cfcp, size_t index)
{
    struct utf8_constant *c = cfcp[index];
    VM_MALLOCS(char, c->length + 1, utf8);
    rtcp->pool[index] = (struct rtc) { UTF8_CONSTANT, .v.p = decode_mutf8(c->bytes, c->length, utf8) };
}

static void build_string_constant(struct rtcp *rtcp, void **cfcp, size_t index)
{
    struct string_constant *c = cfcp[index];
    build_utf8_constant(rtcp, cfcp, c->string_index);
    rtcp->pool[index] = (struct rtc) { STRING_CONSTANT, .v.p = (void *) rtcp_get_str(rtcp, c->string_index) };
}

static void build_class_constant(struct rtcp *rtcp, void **cfcp, size_t index)
{
    struct class_constant *c = cfcp[index];
    build_utf8_constant(rtcp, cfcp, c->name_index);  // todo
    rtcp->pool[index] = (struct rtc) { CLASS_CONSTANT, .v.p = (void *) rtcp_get_str(rtcp, c->name_index) };
}

static void build_name_and_type_constant(struct rtcp *rtcp, void **cfcp, size_t index)
{
    struct name_and_type_constant *c = cfcp[index];

    build_utf8_constant(rtcp, cfcp, c->name_index);
    build_utf8_constant(rtcp, cfcp, c->descriptor_index);

    VM_MALLOC(struct name_and_type, name_and_type);
    name_and_type->name = rtcp_get_str(rtcp, c->name_index);
    name_and_type->descriptor = rtcp_get_str(rtcp, c->descriptor_index);
    rtcp->pool[index] = (struct rtc) { NAME_AND_TYPE_CONSTANT, .v.p = name_and_type };
}

static void build_field_ref_constant(struct rtcp *rtcp, void **cfcp, size_t index)
{
    struct member_ref_constant *c = cfcp[index];

    build_class_constant(rtcp, cfcp, c->class_index);  // todo
    build_name_and_type_constant(rtcp,cfcp, c->name_and_type_index);

    VM_MALLOC(struct field_ref, ref);
    ref->class_name = rtcp_get_class_name(rtcp, c->class_index);
    const struct name_and_type *nt = rtcp_get_name_and_type(rtcp, c->name_and_type_index);
    ref->name = nt->name;
    ref->descriptor = nt->descriptor;
    ref->resolved_class = NULL;
    ref->resolved_field = NULL;
    rtcp->pool[index] = (struct rtc) { FIELD_REF_CONSTANT, .v.p = ref };
}

static void build_method_ref_constant(struct rtcp *rtcp, void **cfcp, size_t index)
{
    struct member_ref_constant *c = cfcp[index];

    build_class_constant(rtcp, cfcp, c->class_index);  // todo
    build_name_and_type_constant(rtcp,cfcp, c->name_and_type_index);

    VM_MALLOC(struct method_ref, ref);
    ref->class_name = rtcp_get_class_name(rtcp, c->class_index);
    const struct name_and_type *nt = rtcp_get_name_and_type(rtcp, c->name_and_type_index);
    ref->name = nt->name;
    ref->descriptor = nt->descriptor;
    ref->resolved_class = NULL;
    ref->resolved_method = NULL;
    rtcp->pool[index] = (struct rtc) { METHOD_REF_CONSTANT, .v.p = ref };
}

static void build_interface_method_ref_constant(struct rtcp *rtcp, void **cfcp, size_t index)
{
    struct member_ref_constant *c = cfcp[index];

    build_class_constant(rtcp, cfcp, c->class_index);  // todo
    build_name_and_type_constant(rtcp,cfcp, c->name_and_type_index);

    VM_MALLOC(struct method_ref, ref);
    ref->class_name = rtcp_get_class_name(rtcp, c->class_index);
    const struct name_and_type *nt = rtcp_get_name_and_type(rtcp, c->name_and_type_index);
    ref->name = nt->name;
    ref->descriptor = nt->descriptor;
    ref->resolved_class = NULL;
    ref->resolved_method = NULL;
    rtcp->pool[index] = (struct rtc) { INTERFACE_METHOD_REF_CONSTANT, .v.p = ref };
}

static void build_integer_constant(struct rtcp *rtcp, void **cfcp, size_t index)
{  // todo 大小端
    int32_t i = bytes_to_int32(((struct four_bytes_num_constant *)cfcp[index])->bytes);
    rtcp->pool[index] = (struct rtc) { INTEGER_CONSTANT, .v.i = i }; // todo int32_t to jint ???????????
}

static void build_float_constant(struct rtcp *rtcp, void **cfcp, size_t index)
{
    jfloat f = bytes_to_float(((struct four_bytes_num_constant *)cfcp[index])->bytes);
    rtcp->pool[index] = (struct rtc) { FLOAT_CONSTANT, .v.f = f };
}

void build_long_constant(struct rtcp *rtcp, void **cfcp, size_t index)
{
    int64_t l = bytes_to_int64(((struct eight_bytes_num_constant *)cfcp[index])->bytes);
    rtcp->pool[index] = (struct rtc) { LONG_CONSTANT, .v.l = l };  // todo int64_t to jlong ???????????
}

static void build_double_constant(struct rtcp *rtcp, void **cfcp, size_t index)
{
    jdouble d = bytes_to_double(((struct eight_bytes_num_constant *)cfcp[index])->bytes);
    rtcp->pool[index] = (struct rtc) { DOUBLE_CONSTANT, .v.d = d };
}

static void build_method_handle_constant(struct rtcp *rtcp, void **cfcp, size_t index)
{
    struct method_handle_constant *c = cfcp[index];

    VM_MALLOC(struct method_handle, handle);
    handle->kind = c->reference_kind;
    switch (handle->kind) {
        case REF_KIND_GET_FIELD:
        case REF_KIND_GET_STATIC:
        case REF_KIND_PUT_FIELD:
        case REF_KIND_PUT_STATIC:
            handle->ref.fr = rtcp_get_field_ref(rtcp, c->reference_index);
            break;
        case REF_KIND_INVOKE_VIRTUAL:
        case REF_KIND_NEW_INVOKE_SPECIAL:
            handle->ref.mr = rtcp_get_method_ref(rtcp, c->reference_index);
            break;
        case REF_KIND_INVOKE_STATIC:
        case REF_KIND_INVOKE_SPECIAL:
            check2(rtcp, c->reference_index, METHOD_REF_CONSTANT, INTERFACE_METHOD_REF_CONSTANT);
            handle->ref.mr = rtcp->pool[index].v.p;
            break;
        case REF_KIND_INVOKE_INTERFACE:
            handle->ref.mr = rtcp_get_interface_method_ref(rtcp, c->reference_index);
            break;
        default:
            printvm("unknown method handle kind: %d\n", handle->kind); // todo error
            break;
    }

    rtcp->pool[index] = (struct rtc) { METHOD_HANDLE_CONSTANT, .v.p = handle };
}

static void build_method_type_constant(struct rtcp *rtcp, void **cfcp, size_t index)
{
    struct method_type_constant *c = cfcp[index];
    build_utf8_constant(rtcp, cfcp, c->descriptor_index);
    rtcp->pool[index] = (struct rtc) { METHOD_TYPE_CONSTANT, .v.p = (void *) rtcp_get_str(rtcp, c->descriptor_index) };
}

static void build_invoke_dynamic_constant(struct rtcp *rtcp, void **cfcp, size_t index, const struct bootstrap_methods_attribute *bootstrap_methods_attribute)
{
    struct invoke_dynamic_constant *c = cfcp[index];
    if (c->bootstrap_method_attr_index >= bootstrap_methods_attribute->num_bootstrap_methods) {
        printvm("error\n"); // todo
        return;
    }

    struct bootstrap_method *bm = bootstrap_methods_attribute->bootstrap_methods + c->bootstrap_method_attr_index;

    VM_MALLOC_EXT(struct invoke_dynamic_ref, 1, sizeof(struct rtc) * bm->num_bootstrap_arguments, ref);
    ref->argc = bm->num_bootstrap_arguments;
    ref->handle = rtcp_get_method_handle(rtcp, bm->bootstrap_method_ref);
    if (ref->handle->kind != REF_KIND_INVOKE_STATIC && ref->handle->kind != REF_KIND_NEW_INVOKE_SPECIAL) {
        printvm("error\n"); // todo
        return;
    }

    for (int i = 0; i < ref->argc; i++) {
        u2 k = bm->bootstrap_arguments[i]; // 在常量池（rtcp）中的索引
        if (k >= rtcp->count) {
            printvm("error\n"); // todo
            return;
        }
        // 常量池在该索引出必须是下列八种结构之一
        switch (rtcp->pool[k].t) {
            case STRING_CONSTANT:
            case CLASS_CONSTANT:
            case INTEGER_CONSTANT:
            case LONG_CONSTANT:
            case FLOAT_CONSTANT:
            case DOUBLE_CONSTANT:
            case METHOD_HANDLE_CONSTANT:
            case METHOD_TYPE_CONSTANT:
                ref->args[i] = rtcp->pool[k];
                break;
            default:
                printvm("error\n"); // todo
                return;
        }
    }

    ref->nt = rtcp_get_name_and_type(rtcp, c->name_and_type_index);

    rtcp->pool[index] = (struct rtc) { INVOKE_DYNAMIC_CONSTANT, .v.p = ref };
}

struct rtcp* rtcp_create(void **cfcp, size_t count, const struct bootstrap_methods_attribute *bootstrap_methods_attribute)
{
    VM_MALLOC(struct rtcp, rtcp);

    rtcp->count = count;
    rtcp->pool = malloc(sizeof(*(rtcp->pool)) * count);

    for (size_t i = 1; i < count; i++) {  // 从 1 开始
        u1 tag = CONSTANT_TAG(cfcp[i]);

        if (tag == CLASS_CONSTANT)  build_class_constant(rtcp, cfcp, i);
        else if (tag == FIELD_REF_CONSTANT) build_field_ref_constant(rtcp, cfcp, i);
        else if (tag == METHOD_REF_CONSTANT) build_method_ref_constant(rtcp, cfcp, i);
        else if (tag == INTERFACE_METHOD_REF_CONSTANT) build_interface_method_ref_constant(rtcp, cfcp, i);
        else if (tag == STRING_CONSTANT) build_string_constant(rtcp, cfcp, i);
        else if (tag == INTEGER_CONSTANT) build_integer_constant(rtcp, cfcp, i);
        else if (tag == FLOAT_CONSTANT) build_float_constant(rtcp, cfcp, i);
        else if (tag == LONG_CONSTANT) build_long_constant(rtcp, cfcp, i);
        else if (tag == DOUBLE_CONSTANT) build_double_constant(rtcp, cfcp, i);
        else if (tag == NAME_AND_TYPE_CONSTANT) build_name_and_type_constant(rtcp, cfcp, i);
        else if (tag == UTF8_CONSTANT) build_utf8_constant(rtcp, cfcp, i);
        else if (tag == METHOD_HANDLE_CONSTANT) build_method_handle_constant(rtcp, cfcp, i);
        else if (tag == METHOD_TYPE_CONSTANT) build_method_type_constant(rtcp, cfcp, i);
        else if (tag == INVOKE_DYNAMIC_CONSTANT) build_invoke_dynamic_constant(rtcp, cfcp, i, bootstrap_methods_attribute);
        else { jvm_abort("error\n"); } // todo error

        if (tag == LONG_CONSTANT || tag == DOUBLE_CONSTANT) {
            i++; // todo
            rtcp->pool[i] = (struct rtc) { -1, .v.p = NULL };  // todo
        }
    }

    return rtcp;
}

void rtcp_destroy(struct rtcp *rtcp)
{
    // todo
}

const u1 rtcp_get_type(const struct rtcp *rtcp, int index)
{
    return rtcp->pool[index].t;
}

jint rtcp_get_int(const struct rtcp *rtcp, int index)
{
    check(rtcp, index, INTEGER_CONSTANT);
    return rtcp->pool[index].v.i;
}

jfloat rtcp_get_float(const struct rtcp *rtcp, int index)
{
    check(rtcp, index, FLOAT_CONSTANT);
    return rtcp->pool[index].v.f;
}

jlong rtcp_get_long(const struct rtcp *rtcp, int index)
{
    check(rtcp, index, LONG_CONSTANT);
    return rtcp->pool[index].v.l;
}

jdouble rtcp_get_double(const struct rtcp *rtcp, int index)
{
    check(rtcp, index, DOUBLE_CONSTANT);
    return rtcp->pool[index].v.d;
}

const char* rtcp_get_str(const struct rtcp *rtcp, int index)
{
    check2(rtcp, index, UTF8_CONSTANT, STRING_CONSTANT);
    return rtcp->pool[index].v.p;
}

const char* rtcp_get_class_name(const struct rtcp *rtcp, int index)
{
    check(rtcp, index, CLASS_CONSTANT);
    return rtcp->pool[index].v.p;
}

const struct name_and_type* rtcp_get_name_and_type(const struct rtcp *rtcp, int index)
{
    check(rtcp, index, NAME_AND_TYPE_CONSTANT);
    return rtcp->pool[index].v.p;
}

struct field_ref* rtcp_get_field_ref(const struct rtcp *rtcp, int index)
{
    check(rtcp, index, FIELD_REF_CONSTANT);
    return rtcp->pool[index].v.p;
}

struct method_ref* rtcp_get_method_ref(const struct rtcp *rtcp, int index)
{
    check(rtcp, index, METHOD_REF_CONSTANT);
    return rtcp->pool[index].v.p;
}

struct method_ref* rtcp_get_interface_method_ref(const struct rtcp *rtcp, int index)
{
    check(rtcp, index, INTERFACE_METHOD_REF_CONSTANT);
    return rtcp->pool[index].v.p;
}

struct method_handle* rtcp_get_method_handle(const struct rtcp *rtcp, int index)
{
    check(rtcp, index, METHOD_HANDLE_CONSTANT);
    return rtcp->pool[index].v.p;
}

const char* rtcp_get_method_type(const struct rtcp *rtcp, int index)
{
    check(rtcp, index, METHOD_TYPE_CONSTANT);
    return rtcp->pool[index].v.p;
}

struct invoke_dynamic_ref* rtcp_get_invoke_dynamic(const struct rtcp *rtcp, int index)
{
    check(rtcp, index, INVOKE_DYNAMIC_CONSTANT);
    return rtcp->pool[index].v.p;
}

