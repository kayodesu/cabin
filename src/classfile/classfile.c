/*
 * Author: Jia Yang
 */

#include <stdlib.h>
#include <assert.h>
#include "classfile.h"
#include "../util/bytecode_reader.h"
#include "../jvm.h"
#include "constant.h"
#include "attribute.h"

static void parse_member_info(struct bytecode_reader *reader,
                              struct constant *constant_pool, u2 constant_pool_count, struct member_info *info)
{
    info->access_flags = bcr_readu2(reader);
    info->name_index = bcr_readu2(reader);
    info->descriptor_index = bcr_readu2(reader);
    info->attributes_count = bcr_readu2(reader);
    info->attributes = malloc(sizeof(void *) * info->attributes_count);
    for (int i = 0; i < info->attributes_count; i++) {
        info->attributes[i] = parse_attribute(reader, constant_pool, constant_pool_count);
    }
}

//static const struct placeholder_constant placeholder_constant = (struct placeholder_constant){ PLACEHOLDER_CONSTANT };

struct classfile* classfile_create(s1 *bytecode, size_t len)
{
    assert(bytecode != NULL);

    struct bytecode_reader *reader = bcr_create(bytecode, len);

    VM_MALLOC(struct classfile, cf);
    cf->magic = bcr_readu4(reader);
    cf->minor_version = bcr_readu2(reader);
    cf->major_version = bcr_readu2(reader);

    // parse constant pool
    cf->constant_pool_count = bcr_readu2(reader);
    cf->constant_pool = malloc(sizeof(struct constant) * cf->constant_pool_count);
    CHECK_MALLOC_RESULT(cf->constant_pool);

    // constant pool 从 1 开始计数，第0位无效
    cf->constant_pool[0] = (struct constant) { .tag = INVALID_CONSTANT };
    for (int i = 1; i < cf->constant_pool_count; i++) {
        struct constant *c = cf->constant_pool + i;

        c->tag = bcr_readu1(reader);
        switch (c->tag) {
            case CLASS_CONSTANT:
                c->u.class_name_index = bcr_readu2(reader);
                break;
            case FIELD_REF_CONSTANT:
            case METHOD_REF_CONSTANT:
            case INTERFACE_METHOD_REF_CONSTANT:
                c->u.ref_constant.class_index = bcr_readu2(reader);
                c->u.ref_constant.name_and_type_index = bcr_readu2(reader);
                break;
            case STRING_CONSTANT:
                c->u.string_index = bcr_readu2(reader);
                break;
            case INTEGER_CONSTANT:
            case FLOAT_CONSTANT:
                bcr_read_bytes(reader, c->u.bytes4, 4);
                break;
            case LONG_CONSTANT:
            case DOUBLE_CONSTANT:
                bcr_read_bytes(reader, c->u.bytes8, 8);
                // 在Class文件的常量池中，所有的8字节的常量都占两个表成员（项）的空间。
                // 如果一个 CONSTANT_Long_info 或 CONSTANT_Double_info 结构的项在常量池中的索引为 n，
                // 则常量池中下一个有效的项的索引为 n + 2，此时常量池中索引为n+1的项有效但必须被认为不可用
                i++;
                assert(i < cf->constant_pool_count);
                cf->constant_pool[i] = (struct constant) { .tag = PLACEHOLDER_CONSTANT };
                break;
            case NAME_AND_TYPE_CONSTANT:
                c->u.name_and_type_constant.name_index = bcr_readu2(reader);
                c->u.name_and_type_constant.descriptor_index = bcr_readu2(reader);
                break;
            case UTF8_CONSTANT:
                c->u.utf8_constant.length = bcr_readu2(reader);
                c->u.utf8_constant.bytes = malloc(c->u.utf8_constant.length);
                CHECK_MALLOC_RESULT(c->u.utf8_constant.bytes);
                bcr_read_bytes(reader, c->u.utf8_constant.bytes, c->u.utf8_constant.length);
                break;
            case METHOD_HANDLE_CONSTANT:
                c->u.method_handle_constant.reference_kind = bcr_readu1(reader);
                c->u.method_handle_constant.reference_index = bcr_readu2(reader);
                break;
            case METHOD_TYPE_CONSTANT:
                c->u.method_descriptor_index = bcr_readu2(reader);
                break;
            case INVOKE_DYNAMIC_CONSTANT:
                c->u.invoke_dynamic_constant.bootstrap_method_attr_index = bcr_readu2(reader);
                c->u.invoke_dynamic_constant.name_and_type_index = bcr_readu2(reader);
                break;
            default:
                jvm_abort("error. unknown constant tag: %d\n", c->tag);
        }
    }

    cf->access_flags = bcr_readu2(reader);
    cf->this_class = bcr_readu2(reader);
    cf->super_class = bcr_readu2(reader);

    // parse interfaces
    cf->interfaces_count = bcr_readu2(reader);
    cf->interfaces = malloc(sizeof(u2) * cf->interfaces_count);
    CHECK_MALLOC_RESULT(cf->interfaces);
    for (int i = 0; i < cf->interfaces_count; i++) {
        cf->interfaces[i] = bcr_readu2(reader);
    }

    // parse fields
    cf->fields_count = bcr_readu2(reader);
    cf->fields = malloc(sizeof(struct member_info) * cf->fields_count);
    CHECK_MALLOC_RESULT(cf->fields);
    for (int i = 0; i < cf->fields_count; i++) {
        parse_member_info(reader, cf->constant_pool, cf->constant_pool_count, cf->fields + i);
    }

    // parse methods
    cf->methods_count = bcr_readu2(reader);
    cf->methods = malloc(sizeof(struct member_info) * cf->methods_count);\
    CHECK_MALLOC_RESULT(cf->methods);
    for (int i = 0; i < cf->methods_count; i++) {
        parse_member_info(reader, cf->constant_pool, cf->constant_pool_count, cf->methods + i);
    }

    // parse class attributes
    cf->attributes_count = bcr_readu2(reader);
    cf->attributes = malloc(sizeof(void **) * cf->attributes_count);
    CHECK_MALLOC_RESULT(cf->attributes);
    for (int i = 0; i < cf->attributes_count; i++) {
        cf->attributes[i] = parse_attribute(reader, cf->constant_pool, cf->constant_pool_count);
    }

    bcr_destroy(reader);
    return cf;
}

void classfile_destroy(struct classfile* cf)
{
    // todo
}
