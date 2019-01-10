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
#include "../heap/object.h"
#include "../heap/strobj.h"
#include "../heap/clsobj.h"

/*
 * RunTime Constant Pool
 */
struct rtcp {
    struct {
        u1 t; // type of constant, defined in 'constant.h'

        union {
            jint i;
            jfloat f;
            jlong l;
            jdouble d;
            void *p;

            struct name_and_type nt;
            struct field_ref fr;
            struct method_ref mr;

            u2 u2s[2];
        } v;
    } *pool;

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
    u2 len = cfcp[index].u.utf8_constant.length;
    VM_MALLOCS(char, len + 1, utf8);

    rtcp->pool[index].t = UTF8_CONSTANT;
    rtcp->pool[index].v.p = decode_mutf8(cfcp[index].u.utf8_constant.bytes, len, utf8);
}

static void build_string_constant(struct rtcp *rtcp, const struct constant *cfcp, size_t index)
{
    rtcp->pool[index].t = STRING_CONSTANT;
    rtcp->pool[index].v.p = (void *) rtcp_get_str(rtcp, cfcp[index].u.string_index);
}

static void build_class_constant(struct rtcp *rtcp, const struct constant *cfcp, size_t index)
{
    rtcp->pool[index].t = CLASS_CONSTANT;
    rtcp->pool[index].v.p = (void *) rtcp_get_str(rtcp, cfcp[index].u.class_name_index);
}

static void build_name_and_type_constant(struct rtcp *rtcp, const struct constant *cfcp, size_t index)
{
    rtcp->pool[index].t = NAME_AND_TYPE_CONSTANT;
    rtcp->pool[index].v.nt.name = rtcp_get_str(rtcp, cfcp[index].u.name_and_type_constant.name_index);
    rtcp->pool[index].v.nt.descriptor = rtcp_get_str(rtcp, cfcp[index].u.name_and_type_constant.descriptor_index);
}

static void build_field_ref_constant(struct rtcp *rtcp, const struct constant *cfcp, size_t index)
{
    const struct constant *c = cfcp + index;

    rtcp->pool[index].t = FIELD_REF_CONSTANT;
    rtcp->pool[index].v.fr.class_name = rtcp_get_class_name(rtcp, c->u.field_ref_constant.class_index);
    const struct name_and_type *nt = rtcp_get_name_and_type(rtcp, c->u.field_ref_constant.name_and_type_index);
    rtcp->pool[index].v.fr.name = nt->name;
    rtcp->pool[index].v.fr.descriptor = nt->descriptor;
    rtcp->pool[index].v.fr.resolved_class = NULL;
    rtcp->pool[index].v.fr.resolved_field = NULL;
}

static void build_method_ref_constant(struct rtcp *rtcp, const struct constant *cfcp, size_t index)
{
    const struct constant *c = cfcp + index;

    rtcp->pool[index].t = METHOD_REF_CONSTANT;
    rtcp->pool[index].v.mr.class_name = rtcp_get_class_name(rtcp, c->u.method_ref_constant.class_index);
    const struct name_and_type *nt = rtcp_get_name_and_type(rtcp, c->u.method_ref_constant.name_and_type_index);
    rtcp->pool[index].v.mr.name = nt->name;
    rtcp->pool[index].v.mr.descriptor = nt->descriptor;
    rtcp->pool[index].v.mr.resolved_class = NULL;
    rtcp->pool[index].v.mr.resolved_method = NULL;
}

static void build_interface_method_ref_constant(struct rtcp *rtcp, const struct constant *cfcp, size_t index)
{
    const struct constant *c = cfcp + index;

    rtcp->pool[index].t = INTERFACE_METHOD_REF_CONSTANT;
    rtcp->pool[index].v.mr.class_name = rtcp_get_class_name(rtcp, c->u.interface_method_ref_constant.class_index);
    const struct name_and_type *nt = rtcp_get_name_and_type(rtcp, c->u.interface_method_ref_constant.name_and_type_index);
    rtcp->pool[index].v.mr.name = nt->name;
    rtcp->pool[index].v.mr.descriptor = nt->descriptor;
    rtcp->pool[index].v.mr.resolved_class = NULL;
    rtcp->pool[index].v.mr.resolved_method = NULL;
}

static void build_integer_constant(struct rtcp *rtcp, const struct constant *cfcp, size_t index)
{  // todo 大小端
    rtcp->pool[index].t = INTEGER_CONSTANT;
    rtcp->pool[index].v.i = bytes_to_int32(cfcp[index].u.bytes4); // todo int32_t to jint ???????????
}

static void build_float_constant(struct rtcp *rtcp, const struct constant *cfcp, size_t index)
{
    rtcp->pool[index].t = FLOAT_CONSTANT;
    rtcp->pool[index].v.f = bytes_to_float(cfcp[index].u.bytes4);
}

static void build_long_constant(struct rtcp *rtcp, const struct constant *cfcp, size_t index)
{
    rtcp->pool[index].t = LONG_CONSTANT;
    rtcp->pool[index].v.l = bytes_to_int64(cfcp[index].u.bytes8); // todo int64_t to jlong ???????????
}

static void build_double_constant(struct rtcp *rtcp, const struct constant *cfcp, size_t index)
{
    rtcp->pool[index].t = DOUBLE_CONSTANT;
    rtcp->pool[index].v.d =  bytes_to_double(cfcp[index].u.bytes8);
}

static void build_method_handle_constant(struct rtcp *rtcp, const struct constant *cfcp, size_t index)
{
    VM_MALLOC(struct method_handle, handle);
    handle->kind = cfcp[index].u.method_handle_constant.reference_kind;
    const u2 reference_index = cfcp[index].u.method_handle_constant.reference_index;
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

    rtcp->pool[index].t = METHOD_HANDLE_CONSTANT;
    rtcp->pool[index].v.p = handle;
}

static void build_method_type_constant(struct rtcp *rtcp, const struct constant *cfcp, size_t index)
{
    rtcp->pool[index].t = METHOD_TYPE_CONSTANT;
    rtcp->pool[index].v.p = (void *) rtcp_get_str(rtcp, cfcp[index].u.method_descriptor_index);
}

void rtcp_build_invoke_dynamic_constant(struct rtcp *rtcp, struct bootstrap_method *bsms)
{
    for (int i = 0; i < rtcp->count; i++) {
        if (rtcp->pool[i].t == INVOKE_DYNAMIC_CONSTANT) {
            u2 bootstrap_method_index = rtcp->pool[i].v.u2s[0];
            u2 name_and_type_index = rtcp->pool[i].v.u2s[1];
            struct bootstrap_method *bm = bsms + bootstrap_method_index;

            VM_MALLOC_EXT(struct invoke_dynamic_ref, 1, sizeof(int) * bm->num_bootstrap_arguments, ref);
            ref->argc = bm->num_bootstrap_arguments;
            ref->handle = rtcp_get_method_handle(rtcp, bm->bootstrap_method_ref);
            if (ref->handle->kind != REF_KIND_INVOKE_STATIC && ref->handle->kind != REF_KIND_NEW_INVOKE_SPECIAL) {
                VM_UNKNOWN_ERROR("handle kind. %d", ref->handle->kind); // todo
                return;
            }

            for (int j = 0; j < ref->argc; j++) {
                u2 k = bm->bootstrap_arguments[j]; // 在常量池（rtcp）中的索引
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
                        ref->args[j] = k;
                        break;
                    default:
                        VM_UNKNOWN_ERROR("unknown type. t = %d, index = %d\n", rtcp->pool[k].t, k);
                }
            }

            ref->nt = rtcp_get_name_and_type(rtcp, name_and_type_index);

            rtcp->pool[i].v.p = ref;
        }
    }
}

static void build_invoke_dynamic_constant(struct rtcp *rtcp, const struct constant *cfcp, size_t index)
{
//    if (cfcp[index].u.invoke_dynamic_constant.bootstrap_method_attr_index >= bootstrap_methods_attribute->u.bootstrap_methods.num) {
//        jvm_abort("error\n"); // todo
//        return;
//    }
//
//    struct bootstrap_method *bm
//            = bootstrap_methods_attribute->u.bootstrap_methods.methods + cfcp[index].u.invoke_dynamic_constant.bootstrap_method_attr_index;

    // 由于此时 bootstrap_methods 表还没有建立，所以这里只记录下在 bootstrap_methods 表中的索引，及name_and_type的索引。
    rtcp->pool[index].t = INVOKE_DYNAMIC_CONSTANT;
    rtcp->pool[index].v.u2s[0] = cfcp[index].u.invoke_dynamic_constant.bootstrap_method_attr_index;
    rtcp->pool[index].v.u2s[1] = cfcp[index].u.invoke_dynamic_constant.name_and_type_index;
}

struct rtcp* rtcp_create(const struct constant *cfcp, size_t count)
{
    assert(cfcp != NULL);

    VM_MALLOC(struct rtcp, rtcp);
    rtcp->count = count;
    rtcp->pool = vm_calloc(sizeof(*(rtcp->pool)), count);

    rtcp->pool[0].t = 0; // todo 第0位该怎么处理。

    /*
     * build run-time constants 时应注意依赖关系，
     * 即 CONSTANT_A 依赖于 CONSTANT_B（如 STRING_CONSTANT 依赖于 UTF8_CONSTANT），
     * 则 CONSTANT_B 必须在 CONSTANT_A 之前 build。
     * 因此，将 all constants 分为四级，第0级没有依赖，第1级依赖于第0级，第2级依赖于第0级 或/和 第1级，以此类推。
     * 第0级: UTF8_CONSTANT, INTEGER_CONSTANT, FLOAT_CONSTANT, LONG_CONSTANT, DOUBLE_CONSTANT, PLACEHOLDER_CONSTANT, INVOKE_DYNAMIC_CONSTANT
     * 第1级: STRING_CONSTANT, CLASS_CONSTANT, NAME_AND_TYPE_CONSTANT, METHOD_TYPE_CONSTANT.
     * 第2级: FIELD_REF_CONSTANT, METHOD_REF_CONSTANT, INTERFACE_METHOD_REF_CONSTANT.
     * 第3级: METHOD_HANDLE_CONSTANT.
     */
    // 遍历第0级
    for (size_t i = 1; i < count; i++) {
        u1 tag = cfcp[i].tag;

        if (tag == UTF8_CONSTANT) {
            build_utf8_constant(rtcp, cfcp, i);
        } else if (tag == INTEGER_CONSTANT) {
            build_integer_constant(rtcp, cfcp, i);
        } else if (tag == FLOAT_CONSTANT) {
            build_float_constant(rtcp, cfcp, i);
        } else if (tag == LONG_CONSTANT) {
            build_long_constant(rtcp, cfcp, i);
        } else if (tag == DOUBLE_CONSTANT) {
            build_double_constant(rtcp, cfcp, i);
        } else if (tag == PLACEHOLDER_CONSTANT) {
            rtcp->pool[i].t = PLACEHOLDER_CONSTANT;
        } else if (tag == INVOKE_DYNAMIC_CONSTANT) {
            build_invoke_dynamic_constant(rtcp, cfcp, i);
        }
    }

    // 遍历第1级
    for (size_t i = 1; i < count; i++) {
        u1 tag = cfcp[i].tag;

        if (tag == CLASS_CONSTANT) {
            build_class_constant(rtcp, cfcp, i);
        } else if (tag == STRING_CONSTANT) {
            build_string_constant(rtcp, cfcp, i);
        } else if (tag == NAME_AND_TYPE_CONSTANT) {
            build_name_and_type_constant(rtcp, cfcp, i);
        } else if (tag == METHOD_TYPE_CONSTANT) {
            build_method_type_constant(rtcp, cfcp, i);
        }
    }

    // 遍历第2级
    for (size_t i = 1; i < count; i++) {
        u1 tag = cfcp[i].tag;

        if (tag == FIELD_REF_CONSTANT) {
            build_field_ref_constant(rtcp, cfcp, i);
        } else if (tag == METHOD_REF_CONSTANT) {
            build_method_ref_constant(rtcp, cfcp, i);
        } else if (tag == INTERFACE_METHOD_REF_CONSTANT) {
            build_interface_method_ref_constant(rtcp, cfcp, i);
        }
    }

    // 遍历第3级
    for (size_t i = 1; i < count; i++) {
        u1 tag = cfcp[i].tag;
        if (tag == METHOD_HANDLE_CONSTANT)
            build_method_handle_constant(rtcp, cfcp, i);
    }

    return rtcp;
}

void rtcp_destroy(struct rtcp *rtcp)
{
    assert(rtcp != NULL);
    // todo

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
    return &(rtcp->pool[index].v.nt);
}

struct field_ref* rtcp_get_field_ref(const struct rtcp *rtcp, int index)
{
    check(rtcp, index, FIELD_REF_CONSTANT);
    return &(rtcp->pool[index].v.fr);
}

struct method_ref* rtcp_get_method_ref(const struct rtcp *rtcp, int index)
{
    check(rtcp, index, METHOD_REF_CONSTANT);
    return &(rtcp->pool[index].v.mr);
}

struct method_ref* rtcp_get_interface_method_ref(const struct rtcp *rtcp, int index)
{
    check(rtcp, index, INTERFACE_METHOD_REF_CONSTANT);
    return &(rtcp->pool[index].v.mr);
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

    switch (rtcp->pool[index].t) {
        case STRING_CONSTANT:
            return rslot(strobj_create(rtcp_get_str(rtcp, index)));
        case CLASS_CONSTANT:
            return rslot(clsobj_create(classloader_load_class(loader, rtcp_get_class_name(rtcp, index))));
        case INTEGER_CONSTANT:
            return islot(rtcp_get_int(rtcp, index));
        case LONG_CONSTANT:
            return lslot(rtcp_get_long(rtcp, index));
        case FLOAT_CONSTANT:
            return fslot(rtcp_get_float(rtcp, index));
        case DOUBLE_CONSTANT:
            return dslot(rtcp_get_double(rtcp, index));
        case METHOD_HANDLE_CONSTANT: {
//            CONSTANT_MethodHandle_info {
//                u1 tag;
//                u1 reference_kind;
//                u2 reference_index;
//            }
            // https://docs.oracle.com/javase/7/docs/api/java/lang/invoke/MethodHandle.html
            // MethodHandle MethodHandle.lookup().findStatic(Class<?> refc, String name, MethodType type)
            //                                    findVirtual ... 等等一推find
            struct method_handle *mh = rtcp_get_method_handle(rtcp, index);

            // todo
            jvm_abort("");
            break;
        }
        case METHOD_TYPE_CONSTANT: {
            const char *descriptor = rtcp_get_method_type(rtcp, index);
            // todo
            jvm_abort("");
            break;
        }
        default:
            VM_UNKNOWN_ERROR("unknown type. t = %d, index = %d\n", rtcp->pool[index].t, index);
            break;
    }
}
