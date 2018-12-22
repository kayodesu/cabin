/*
 * Author: Jia Yang
 */

#include <stdlib.h>
#include <stdbool.h>
#include <assert.h>
#include "../../jvm.h"
#include "rtcp.h"
#include "../../classfile/constant.h"
#include "../../util/encoding.h"
#include "../../util/convert.h"
#include "symref.h"
#include "../../classfile/attribute.h"
#include "../../loader/classloader.h"
#include "../heap/jobject.h"


/*
 * RunTime Constant Pool
 */
struct rtcp {
    struct rtc *pool;
    size_t count;
};

#define check(rtcp, index, type) \
do { \
    if ((rtcp)->pool[index].t != (type)) { \
        VM_UNKNOWN_ERROR("want %d, bug tag = %d, index = %d.\n", type, (rtcp)->pool[index].t, index); \
    } \
} while (false)

#define check2(rtcp, index, type1, type2) \
do { \
    if ((rtcp)->pool[index].t != (type1) && (rtcp)->pool[index].t != (type2)) { \
        VM_UNKNOWN_ERROR("want %d or %d, bug tag = %d, index = %d.\n", type1, type2, (rtcp)->pool[index].t, index); \
    } \
} while (false)

static void build_utf8_constant(struct rtcp *rtcp, const struct constant *cfcp, size_t index)
{
    const struct constant *c = cfcp + index;
    u2 len = c->u.utf8_constant.length;
    VM_MALLOCS(char, len + 1, utf8);
    rtcp->pool[index] = (struct rtc) { UTF8_CONSTANT, .v.p = decode_mutf8(c->u.utf8_constant.bytes, len, utf8) };
}

static void build_string_constant(struct rtcp *rtcp, const struct constant *cfcp, size_t index)
{
    const struct constant *c = cfcp + index;
    rtcp->pool[index] = (struct rtc) { STRING_CONSTANT, .v.p = (void *) rtcp_get_str(rtcp, c->u.string_index) };
}

static void build_class_constant(struct rtcp *rtcp, const struct constant *cfcp, size_t index)
{
    const struct constant *c = cfcp + index;
    rtcp->pool[index] = (struct rtc) { CLASS_CONSTANT, .v.p = (void *) rtcp_get_str(rtcp, c->u.class_name_index) };
}

static void build_name_and_type_constant(struct rtcp *rtcp, const struct constant *cfcp, size_t index)
{
    const struct constant *c = cfcp + index;

    VM_MALLOC(struct name_and_type, name_and_type);
    name_and_type->name = rtcp_get_str(rtcp, c->u.name_and_type_constant.name_index);
    name_and_type->descriptor = rtcp_get_str(rtcp, c->u.name_and_type_constant.descriptor_index);
    rtcp->pool[index] = (struct rtc) { NAME_AND_TYPE_CONSTANT, .v.p = name_and_type };
}

static void build_field_ref_constant(struct rtcp *rtcp, const struct constant *cfcp, size_t index)
{
    const struct constant *c = cfcp + index;

    VM_MALLOC(struct field_ref, ref);
    ref->class_name = rtcp_get_class_name(rtcp, c->u.field_ref_constant.class_index);
    const struct name_and_type *nt = rtcp_get_name_and_type(rtcp, c->u.field_ref_constant.name_and_type_index);
    ref->name = nt->name;
    ref->descriptor = nt->descriptor;
    ref->resolved_class = NULL;
    ref->resolved_field = NULL;
    rtcp->pool[index] = (struct rtc) { FIELD_REF_CONSTANT, .v.p = ref };
}

static void build_method_ref_constant(struct rtcp *rtcp, const struct constant *cfcp, size_t index)
{
    const struct constant *c = cfcp + index;

    VM_MALLOC(struct method_ref, ref);
    ref->class_name = rtcp_get_class_name(rtcp, c->u.method_ref_constant.class_index);
    const struct name_and_type *nt = rtcp_get_name_and_type(rtcp, c->u.method_ref_constant.name_and_type_index);
    ref->name = nt->name;
    ref->descriptor = nt->descriptor;
    ref->resolved_class = NULL;
    ref->resolved_method = NULL;
    rtcp->pool[index] = (struct rtc) { METHOD_REF_CONSTANT, .v.p = ref };
}

static void build_interface_method_ref_constant(struct rtcp *rtcp, const struct constant *cfcp, size_t index)
{
    const struct constant *c = cfcp + index;

    VM_MALLOC(struct method_ref, ref);
    ref->class_name = rtcp_get_class_name(rtcp, c->u.interface_method_ref_constant.class_index);
    const struct name_and_type *nt = rtcp_get_name_and_type(rtcp, c->u.interface_method_ref_constant.name_and_type_index);
    ref->name = nt->name;
    ref->descriptor = nt->descriptor;
    ref->resolved_class = NULL;
    ref->resolved_method = NULL;
    rtcp->pool[index] = (struct rtc) { INTERFACE_METHOD_REF_CONSTANT, .v.p = ref };
}

static void build_integer_constant(struct rtcp *rtcp, const struct constant *cfcp, size_t index)
{  // todo 大小端
    int32_t i = bytes_to_int32(cfcp[index].u.bytes4);
    rtcp->pool[index] = (struct rtc) { INTEGER_CONSTANT, .v.i = i }; // todo int32_t to jint ???????????
}

static void build_float_constant(struct rtcp *rtcp, const struct constant *cfcp, size_t index)
{
    jfloat f = bytes_to_float(cfcp[index].u.bytes4);
    rtcp->pool[index] = (struct rtc) { FLOAT_CONSTANT, .v.f = f };
}

static void build_long_constant(struct rtcp *rtcp, const struct constant *cfcp, size_t index)
{
    int64_t l = bytes_to_int64(cfcp[index].u.bytes8);
    rtcp->pool[index] = (struct rtc) { LONG_CONSTANT, .v.l = l };  // todo int64_t to jlong ???????????
}

static void build_double_constant(struct rtcp *rtcp, const struct constant *cfcp, size_t index)
{
    jdouble d = bytes_to_double(cfcp[index].u.bytes8);
    rtcp->pool[index] = (struct rtc) { DOUBLE_CONSTANT, .v.d = d };
}

static void build_method_handle_constant(struct rtcp *rtcp, const struct constant *cfcp, size_t index)
{
    const struct constant *c = cfcp + index;

    VM_MALLOC(struct method_handle, handle);
    handle->kind = c->u.method_handle_constant.reference_kind;
    const u2 reference_index = c->u.method_handle_constant.reference_index;
    switch (handle->kind) {
        case REF_KIND_GET_FIELD:
        case REF_KIND_GET_STATIC:
        case REF_KIND_PUT_FIELD:
        case REF_KIND_PUT_STATIC:
            handle->ref.fr = rtcp_get_field_ref(rtcp, reference_index);
            break;
        case REF_KIND_INVOKE_VIRTUAL:
        case REF_KIND_NEW_INVOKE_SPECIAL:
            handle->ref.mr = rtcp_get_method_ref(rtcp, reference_index);
            break;
        case REF_KIND_INVOKE_STATIC:
        case REF_KIND_INVOKE_SPECIAL:
            /*
             * the constant_pool entry at reference_index must be
             * either a CONSTANT_Methodref_info structure
             * or a CONSTANT_InterfaceMethodref_info structure
             * representing a class's or interface's method for which a method handle is to be created.
             */
            if (cfcp[reference_index].tag == METHOD_REF_CONSTANT) {
                handle->ref.mr = rtcp_get_method_ref(rtcp, reference_index);
            } else if (cfcp[reference_index].tag == INTERFACE_METHOD_REF_CONSTANT) {
                handle->ref.mr = rtcp_get_interface_method_ref(rtcp, reference_index);
            } else {
                VM_UNKNOWN_ERROR("unknown constant tag: %d\n", cfcp[reference_index].tag);
            }
            break;
        case REF_KIND_INVOKE_INTERFACE:
            handle->ref.mr = rtcp_get_interface_method_ref(rtcp, reference_index);
            break;
        default:
            VM_UNKNOWN_ERROR("unknown method handle kind: %d\n", handle->kind);
            break;
    }

    rtcp->pool[index] = (struct rtc) { METHOD_HANDLE_CONSTANT, .v.p = handle };
}

static void build_method_type_constant(struct rtcp *rtcp, const struct constant *cfcp, size_t index)
{
    const struct constant *c = cfcp + index;
    rtcp->pool[index]
            = (struct rtc) { METHOD_TYPE_CONSTANT, .v.p = (void *) rtcp_get_str(rtcp, c->u.method_descriptor_index) };
}

static void build_invoke_dynamic_constant(struct rtcp *rtcp,
                                          const struct constant *cfcp, size_t index,
                                          const struct bootstrap_methods_attribute *bootstrap_methods_attribute)
{
    const struct constant *c = cfcp + index;
    if (c->u.invoke_dynamic_constant.bootstrap_method_attr_index >= bootstrap_methods_attribute->num_bootstrap_methods) {
        jvm_abort("error\n"); // todo
        return;
    }

    struct bootstrap_method *bm
            = bootstrap_methods_attribute->bootstrap_methods + c->u.invoke_dynamic_constant.bootstrap_method_attr_index;

    VM_MALLOC_EXT(struct invoke_dynamic_ref, 1, sizeof(int) * bm->num_bootstrap_arguments, ref);
    ref->argc = bm->num_bootstrap_arguments;
    ref->handle = rtcp_get_method_handle(rtcp, bm->bootstrap_method_ref);
    if (ref->handle->kind != REF_KIND_INVOKE_STATIC && ref->handle->kind != REF_KIND_NEW_INVOKE_SPECIAL) {
        VM_UNKNOWN_ERROR("handle kind. %d", ref->handle->kind); // todo
        return;
    }

    for (int i = 0; i < ref->argc; i++) {
        u2 k = bm->bootstrap_arguments[i]; // 在常量池（rtcp）中的索引
        assert(k < rtcp->count);
        // 常量池在该索引出必须是下列八种结构之一
        // todo 如果这里直接转成slot需要load class，会不会导致递归调用
        switch (rtcp->pool[k].t) {
            case STRING_CONSTANT:
            case CLASS_CONSTANT:
            case INTEGER_CONSTANT:
            case LONG_CONSTANT:
            case FLOAT_CONSTANT:
            case DOUBLE_CONSTANT:
            case METHOD_HANDLE_CONSTANT:
            case METHOD_TYPE_CONSTANT:
                ref->args[i] = k;
                break;
            default:
                VM_UNKNOWN_ERROR("unknown type. t = %d, index = %d\n", rtcp->pool[k].t, k);
        }
    }

    ref->nt = rtcp_get_name_and_type(rtcp, c->u.invoke_dynamic_constant.name_and_type_index);
    rtcp->pool[index] = (struct rtc) { INVOKE_DYNAMIC_CONSTANT, .v.p = ref };
}

struct rtcp* rtcp_create(const struct constant *cfcp, size_t count,
                         const struct bootstrap_methods_attribute *bootstrap_methods_attribute)
{
    assert(cfcp != NULL);

    VM_MALLOC(struct rtcp, rtcp);
    rtcp->count = count;
    rtcp->pool = calloc(sizeof(*(rtcp->pool)), count);
    CHECK_MALLOC_RESULT(rtcp->pool);

    rtcp->pool[0].t = 0; // todo 第0位该怎么处理。

    /*
     * build run-time constants 时应注意依赖关系，
     * 即 CONSTANT_A 依赖于 CONSTANT_B（如 STRING_CONSTANT 依赖于 UTF8_CONSTANT），
     * 则 CONSTANT_B 必须在 CONSTANT_A 之前 build。
     * 因此，将 all constants 分为四级，第0级没有依赖，第1级依赖于第0级，第2级依赖于第0级 或/和 第1级，以此类推。
     * 第0级: UTF8_CONSTANT, INTEGER_CONSTANT, FLOAT_CONSTANT, LONG_CONSTANT, DOUBLE_CONSTANT, PLACEHOLDER_CONSTANT.
     * 第1级: STRING_CONSTANT, CLASS_CONSTANT, NAME_AND_TYPE_CONSTANT, METHOD_TYPE_CONSTANT.
     * 第2级: FIELD_REF_CONSTANT, METHOD_REF_CONSTANT, INTERFACE_METHOD_REF_CONSTANT.
     * 第3级: METHOD_HANDLE_CONSTANT.
     * 第4级: INVOKE_DYNAMIC_CONSTANT.
     */
    // 遍历第0级
    for (size_t i = 1; i < count; i++) {
        u1 tag = cfcp[i].tag;

        if (tag == UTF8_CONSTANT)
            build_utf8_constant(rtcp, cfcp, i);
        else if (tag == INTEGER_CONSTANT)
            build_integer_constant(rtcp, cfcp, i);
        else if (tag == FLOAT_CONSTANT)
            build_float_constant(rtcp, cfcp, i);
        else if (tag == LONG_CONSTANT)
            build_long_constant(rtcp, cfcp, i);
        else if (tag == DOUBLE_CONSTANT)
            build_double_constant(rtcp, cfcp, i);
        else if (tag == PLACEHOLDER_CONSTANT)
            rtcp->pool[i] = (struct rtc) { PLACEHOLDER_CONSTANT, .v.p = NULL };
    }

    // 遍历第1级
    for (size_t i = 1; i < count; i++) {
        u1 tag = cfcp[i].tag;

        if (tag == CLASS_CONSTANT)
            build_class_constant(rtcp, cfcp, i);
        else if (tag == STRING_CONSTANT)
            build_string_constant(rtcp, cfcp, i);
        else if (tag == NAME_AND_TYPE_CONSTANT)
            build_name_and_type_constant(rtcp, cfcp, i);
        else if (tag == METHOD_TYPE_CONSTANT)
            build_method_type_constant(rtcp, cfcp, i);
    }

    // 遍历第2级
    for (size_t i = 1; i < count; i++) {
        u1 tag = cfcp[i].tag;

        if (tag == FIELD_REF_CONSTANT)
            build_field_ref_constant(rtcp, cfcp, i);
        else if (tag == METHOD_REF_CONSTANT)
            build_method_ref_constant(rtcp, cfcp, i);
        else if (tag == INTERFACE_METHOD_REF_CONSTANT)
            build_interface_method_ref_constant(rtcp, cfcp, i);
    }

    // 遍历第3级
    for (size_t i = 1; i < count; i++) {
        u1 tag = cfcp[i].tag;
        if (tag == METHOD_HANDLE_CONSTANT)
            build_method_handle_constant(rtcp, cfcp, i);
    }

    // 遍历第4级
    for (size_t i = 1; i < count; i++) {
        if (cfcp[i].tag == INVOKE_DYNAMIC_CONSTANT) {
            if (bootstrap_methods_attribute == NULL) {
                VM_UNKNOWN_ERROR(""); // todo
            } else {
                build_invoke_dynamic_constant(rtcp, cfcp, i, bootstrap_methods_attribute);
            }
        }
    }

    return rtcp;
}

void rtcp_destroy(struct rtcp *rtcp)
{
    assert(rtcp != NULL);
    free(rtcp->pool);
    free(rtcp);
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

struct slot rtc_to_slot(struct classloader *loader, const struct rtcp *rtcp, int index)
{
    assert(rtcp != NULL);

    struct rtc *rtc = rtcp->pool + index;

    struct jclass *c;
    switch (rtcp->pool[index].t) {
        case STRING_CONSTANT:
//            c = classloader_load_class(bootstrap_loader, "java/lang/String");
            return rslot(jstrobj_create(rtcp_get_str(rtcp, index)));
        case CLASS_CONSTANT:
            return rslot(jclsobj_create(classloader_load_class(loader, rtcp_get_class_name(rtcp, index))));
        case INTEGER_CONSTANT:
            return islot(rtcp_get_int(rtcp, index));
        case LONG_CONSTANT:
            return lslot(rtcp_get_long(rtcp, index));
        case FLOAT_CONSTANT:
            return fslot(rtcp_get_float(rtcp, index));
        case DOUBLE_CONSTANT:
            return dslot(rtcp_get_double(rtcp, index));
        case METHOD_HANDLE_CONSTANT: {
            struct method_handle *mh = rtcp_get_method_handle(rtcp, index);
            // todo
            break;
        }
        case METHOD_TYPE_CONSTANT: {
            // todo
            const char *descriptor = rtcp_get_method_type(rtcp, index);
            break;
        }
        default:
            VM_UNKNOWN_ERROR("unknown type. t = %d, index = %d\n", rtcp->pool[index].t, index);
            break;
    }
}
