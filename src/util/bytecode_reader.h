/*
 * Author: Jia Yang
 */

#ifndef JVM_BYTECODE_READER_H
#define JVM_BYTECODE_READER_H

#include <stdbool.h>
#include <stddef.h>
#include <assert.h>
#include <string.h>
#include "../jtypes.h"
#include "../jvm.h"
#include "convert.h"

typedef struct bytecode_reader {
    const u1 *bytecode;
    size_t len;  // bytecode len
    size_t pc;   // program count
} BytecodeReader;

static inline void bcr_init(BytecodeReader *reader, const u1 *bytecode, size_t len)
{
    assert(reader != NULL);
    assert(bytecode != NULL);

    reader->bytecode = bytecode;
    reader->len = len;
    reader->pc = 0;
}

static inline const u1* bcr_curr_pos(BytecodeReader *reader)
{
    assert(reader != NULL);
    return  reader->bytecode + reader->pc;
}

static inline bool bcr_has_more(const BytecodeReader *reader)
{
    assert(reader != NULL);
    return reader->pc < reader->len;
}

static inline void bcr_skip(BytecodeReader *reader, int offset)
{
    assert(reader != NULL);
    reader->pc += offset;
}

/*
 * todo 函数干什么用的
 */
static inline void bcr_align4(BytecodeReader *reader)
{
    assert(reader != NULL);
    while (reader->pc % 4 != 0) {
        reader->pc++;
    }
}

static inline void bcr_read_bytes(BytecodeReader *reader, u1 *buf, size_t len)
{
    assert(reader != NULL);
    assert(buf != NULL);

    memcpy(buf, reader->bytecode + reader->pc, len);
    reader->pc += len;
}

static inline s1 bcr_reads1(BytecodeReader *reader)
{
    assert(reader != NULL);
    return reader->bytecode[reader->pc++];
}

static inline u1 bcr_readu1(BytecodeReader *reader)
{
    assert(reader != NULL);
    return (u1) reader->bytecode[reader->pc++];
}

static inline u2 bcr_readu2(BytecodeReader *reader)
{
    assert(reader != NULL);
    u2 x = bcr_readu1(reader);
    u2 y = (u2) (bcr_readu1(reader) & 0x00ff);

    return x << 8 | y;
}

static inline u2 bcr_peeku2(BytecodeReader *reader)
{
    u2 data = bcr_readu2(reader);
    reader->pc -= 2;
    return data;
}

static inline s2 bcr_reads2(BytecodeReader *reader)
{
    assert(reader != NULL);
    return bcr_readu2(reader);
}

static inline u4 bcr_readu4(BytecodeReader *reader)
{
    assert(reader != NULL);
    u1 buf[4];
    bcr_read_bytes(reader, buf, 4);

    return (u4) bytes_to_int32(buf);  // should be bytesToUint32  todo
}

static inline u8 bcr_readu8(BytecodeReader *reader)
{
    assert(reader != NULL);
    const u1 *p = reader->bytecode;
    u8 v = ((u8)(p)[0]<<56)
           |((u8)(p)[1]<<48)
           |((u8)(p)[2]<<40)
           |((u8)(p)[3]<<32)
           |((u8)(p)[4]<<24)
           |((u8)(p)[5]<<16)
           |((u8)(p)[6]<<8)
           |(u8)(p)[7];
    reader->pc += 8;
    return v;
}

static inline s4 bcr_reads4(BytecodeReader *reader)
{
    assert(reader != NULL);
    u1 buf[4];
    bcr_read_bytes(reader, buf, 4);

    return (s4) bytes_to_int32(buf);
}

/*
 * 读 @n 个s4数据到 @s4s 数组中
 */
static inline void bcr_reads4s(BytecodeReader *reader, int n, s4 *s4s)
{
    assert(reader != NULL);
    for (int i = 0; i < n; i++) {
        u1 buf[4];
        bcr_read_bytes(reader, buf, 4);

        s4s[i] = (s4) bytes_to_int32(buf);
    }
}

static inline void bcr_destroy(BytecodeReader *reader)
{
    free(reader);
}


#endif //JVM_BYTECODE_READER_H
