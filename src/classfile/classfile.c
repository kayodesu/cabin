/*
 * Author: Jia Yang
 */

#include <limits.h>
#include <stdlib.h>
#include <assert.h>
#include "classfile.h"
#include "../util/bytecode_reader.h"
#include "../jvm.h"
#include "constant.h"
#include "attribute.h"

static void parse_member_info(struct bytecode_reader *reader,
                            void **constant_pool, u2 constant_pool_count, struct member_info *info)
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

static const struct placeholder_constant placeholder_constant = (struct placeholder_constant){ PLACEHOLDER_CONSTANT };

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
    cf->constant_pool = malloc(sizeof(void *) * cf->constant_pool_count);
    cf->constant_pool[0] = NULL; // constant pool 从 1 开始计数
    for (int i = 1; i < cf->constant_pool_count; i++) {
        cf->constant_pool[i] = parse_constant(reader);

        // 在Class文件的常量池中，所有的8字节的常量都占两个表成员（项）的空间。
        // 如果一个CONSTANT_Long_info或CONSTANT_Double_info结构的项在常量池中的索引为n，
        // 则常量池中下一个有效的项的索引为n+2，此时常量池中索引为n+1的项有效但必须被认为不可用
        u1 tag = CONSTANT_TAG(cf->constant_pool[i]);
        if (tag == LONG_CONSTANT || tag == DOUBLE_CONSTANT) {
            i++;
            cf->constant_pool[i] = (void *) &placeholder_constant;  // todo
        }
    }

    cf->access_flags = bcr_readu2(reader);
    cf->this_class = bcr_readu2(reader);
    cf->super_class = bcr_readu2(reader);

    // parse interfaces
    cf->interfaces_count = bcr_readu2(reader);
    cf->interfaces = malloc(sizeof(u2) * cf->interfaces_count);
    for (int i = 0; i < cf->interfaces_count; i++) {
        cf->interfaces[i] = bcr_readu2(reader);
    }

    // parse fields
    cf->fields_count = bcr_readu2(reader);
    cf->fields = malloc(sizeof(struct member_info) * cf->fields_count);
    for (int i = 0; i < cf->fields_count; i++) {
        parse_member_info(reader, cf->constant_pool, cf->constant_pool_count, cf->fields + i);
    }

    // parse methods
    cf->methods_count = bcr_readu2(reader);
    cf->methods = malloc(sizeof(struct member_info) * cf->methods_count);
    for (int i = 0; i < cf->methods_count; i++) {
        parse_member_info(reader, cf->constant_pool, cf->constant_pool_count, cf->methods + i);
    }

    // parse class attributes
    cf->attributes_count = bcr_readu2(reader);
    cf->attributes = malloc(sizeof(void **) * cf->attributes_count);
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