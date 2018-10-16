/*
 * Author: Jia Yang
 */

#include <stdio.h>
#include <stdlib.h>
#include "constant.h"
#include "../jvm.h"

void* parse_constant(struct bytecode_reader *reader)
{
    u1 tag = bcr_readu1(reader);
//    printvm("tag = %d\n", tag); /////////////////////////////////////////////       todo
    switch (tag) {
        case CLASS_CONSTANT: {
            VM_MALLOC(struct class_constant, c);
            c->tag = tag;
            c->name_index = bcr_readu2(reader);
            return c;
        }
        case FIELD_REF_CONSTANT:
        case METHOD_REF_CONSTANT:
        case INTERFACE_METHOD_REF_CONSTANT: {
            VM_MALLOC(struct member_ref_constant, c);
            c->tag = tag;
            c->class_index = bcr_readu2(reader);
            c->name_and_type_index = bcr_readu2(reader);
            return c;
        }
        case STRING_CONSTANT: {
            VM_MALLOC(struct string_constant, c);
            c->tag = tag;
            c->string_index = bcr_readu2(reader);
            return c;
        }
        case INTEGER_CONSTANT:
        case FLOAT_CONSTANT: {
            VM_MALLOC(struct four_bytes_num_constant, c);
            c->tag = tag;
            bcr_read_bytes(reader, c->bytes, 4);
            return c;
        }
        case LONG_CONSTANT:
        case DOUBLE_CONSTANT: {
            VM_MALLOC(struct eight_bytes_num_constant, c);
            c->tag = tag;
            bcr_read_bytes(reader, c->bytes, 8);
            return c;
        }
        case NAME_AND_TYPE_CONSTANT: {
            VM_MALLOC(struct name_and_type_constant, c);
            c->tag = tag;
            c->name_index = bcr_readu2(reader);
            c->descriptor_index = bcr_readu2(reader);
            return c;
        }
        case UTF8_CONSTANT: {
            u2 length = bcr_readu2(reader);
            struct utf8_constant *c = malloc(sizeof(struct utf8_constant) + length);
            c->tag = tag;
            c->length = length;
            bcr_read_bytes(reader, c->bytes, length);
            return c;
        }
        case METHOD_HANDLE_CONSTANT: {
            VM_MALLOC(struct method_handle_constant, c);
            c->tag = tag;
            c->reference_kind = bcr_readu1(reader);
            c->reference_index = bcr_readu2(reader);
            return c;
        }
        case METHOD_TYPE_CONSTANT: {
            VM_MALLOC(struct method_type_constant, c);
            c->tag = tag;
            c->descriptor_index = bcr_readu2(reader);
            return c;
        }
        case INVOKE_DYNAMIC_CONSTANT: {
            VM_MALLOC(struct invoke_dynamic_constant, c);
            c->tag = tag;
            c->bootstrap_method_attr_index = bcr_readu2(reader);
            c->name_and_type_index = bcr_readu2(reader);
            return c;
        }
        default:
            jvm_abort("error. unknown tag = %d\n", tag);  // todo
            return NULL;
    }
}
