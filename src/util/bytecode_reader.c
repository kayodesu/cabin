/*
 * Author: Jia Yang
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "convert.h"
#include "../jvm.h"

#include "bytecode_reader.h"

//    void reset() {
//        pc = 0;
//    }

bool bcr_has_more(const struct bytecode_reader *reader)
{
    return reader->pc < reader->len;
}

bool bcr_set_pc(struct bytecode_reader *reader, size_t new_pc)
{
    if (new_pc >= reader->len) {
        // todo error
        jvm_abort("error. new_pc = %zu, len = %zu\n", new_pc, reader->len);
    }
    reader->pc = new_pc;
    return true;
}

bool bcr_skip(struct bytecode_reader *reader, int offset)
{
    return bcr_set_pc(reader, reader->pc + offset);
}

bool bcr_skip_padding(struct bytecode_reader *reader)
{
    if (reader->pc % 4 != 0)
        reader->pc++;
}

void bcr_read_bytes(struct bytecode_reader *reader, u1 *buf, size_t len)
{
    if (buf == NULL) {
        jvm_abort(0, "error\n");
    }

    memcpy(buf, reader->bytecode + reader->pc, len);
    reader->pc += len;
}

s1 bcr_reads1(struct bytecode_reader *reader)
{
    return reader->bytecode[reader->pc++];
}

u1 bcr_readu1(struct bytecode_reader *reader)
{
    return (u1) reader->bytecode[reader->pc++];
}

u2 bcr_readu2(struct bytecode_reader *reader)
{
    u2 x = bcr_readu1(reader);
    u2 y = (u2) (bcr_readu1(reader) & 0x00ff);

    return x << 8 | y;
}

s2 bcr_reads2(struct bytecode_reader *reader)
{
    return bcr_readu2(reader);
}

u4 bcr_readu4(struct bytecode_reader *reader)
{
    u1 buf[4];
    bcr_read_bytes(reader, buf, 4);

    return (u4) bytes_to_int32(buf);  // should be bytesToUint32  todo
}

s4 bcr_reads4(struct bytecode_reader *reader)
{
    u1 buf[4];
    bcr_read_bytes(reader, buf, 4);

    return (s4) bytes_to_int32(buf);
}

/*
 * 读 @n 个s4数据到 @s4s 数组中
 */
void bcr_reads4s(struct bytecode_reader *reader, int n, s4 *s4s)
{
    for (int i = 0; i < n; i++) {
        u1 buf[4];
        bcr_read_bytes(reader, buf, 4);

        s4s[i] = (s4) bytes_to_int32(buf);
    }
}

void bcr_destroy(struct bytecode_reader *reader)
{
    free(reader);
}

struct bytecode_reader* bcr_create(const s1 *bytecode, size_t len)
{
    if (bytecode == NULL) {
        // todo
    }

    struct bytecode_reader *reader = malloc(sizeof(struct bytecode_reader));
    if (reader == NULL) {
        // todo
        return NULL;
    }

    reader->bytecode = bytecode;
    reader->len = len;
    reader->pc = 0;

//    reader->has_more = bytecode_reader_has_more;
//    reader->set_pc = bytecode_reader_set_pc;
//    reader->skip = bytecode_reader_skip;
//    reader->skip_padding = bytecode_reader_skip_padding;
//    reader->read_bytes = bytecode_reader_read_bytes;
//    reader->reads1 = bytecode_reader_reads1;
//    reader->readu1 = bytecode_reader_readu1;
//    reader->readu2 = bytecode_reader_readu2;
//    reader->reads2 = bytecode_reader_reads2;
//    reader->readu4 = bytecode_reader_readu4;
//    reader->reads4 = bytecode_reader_reads4;
//    reader->reads4s = bytecode_reader_reads4s;
//    reader->reads4s = bytecode_reader_reads4s;
//    reader->destroy = bytecode_reader_destroy;

    return reader;
}
