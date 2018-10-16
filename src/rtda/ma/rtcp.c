/*
 * Author: Jia Yang
 */

#include <limits.h>
#include <stdlib.h>
#include "rtcp.h"
#include "../../classfile/constant.h"
#include "../../util/encoding.h"
#include "../../jvm.h"
#include "../../util/convert.h"
#include "../../slot.h"

static void build_utf8_constant(struct rtcp *rtcp, void **cfcp, size_t index)
{
    struct utf8_constant *c = cfcp[index];
    char *utf8 = malloc(sizeof(*utf8) * (c->length + 1)); // todo NULL
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

    struct name_and_type name_and_type;
    name_and_type.name = rtcp_get_str(rtcp, c->name_index);
    name_and_type.descriptor = rtcp_get_str(rtcp, c->descriptor_index);
    rtcp->pool[index] = (struct rtc) { NAME_AND_TYPE_CONSTANT, .v.nt = name_and_type };
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
    // printvm("not parse method_handle_constant\n");
    // todo
    rtcp->pool[index] = (struct rtc) { -1, .v.p = NULL }; //todo
}

static void build_method_type_constant(struct rtcp *rtcp, void **cfcp, size_t index)
{
    // printvm("not parse method_type_constant\n");
    // todo
    rtcp->pool[index] = (struct rtc) { -1, .v.p = NULL }; //todo
}

static void build_invoke_dynamic_constant(struct rtcp *rtcp, void **cfcp, size_t index)
{
    // printvm("not parse method_dynamic_constant\n");
    // todo
    rtcp->pool[index] = (struct rtc) { -1, .v.p = NULL }; //todo
}

struct rtcp* rtcp_create(void **cfcp, size_t count)
{
    VM_MALLOC(struct rtcp, rtcp);

    rtcp->count = count;
    rtcp->pool = malloc(sizeof(*(rtcp->pool)) * count);//new std::pair<u1, void *>[count];

    for (size_t i = 1; i < count; i++) {  // 从 1 开始
        u1 tag = CONSTANT_TAG(cfcp[i]);// cfcp[i]->tag;

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
        else if (tag == INVOKE_DYNAMIC_CONSTANT) build_invoke_dynamic_constant(rtcp, cfcp, i);

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

static bool check(const struct rtcp *rtcp, int index, int type)
{
    assert(rtcp != NULL);
    assert(index > 0 && index < rtcp->count);

    if (rtcp->pool[index].t != type) {
        // todo error
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
    return &(rtcp->pool[index].v.nt);
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

