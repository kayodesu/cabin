#ifndef CABIN_BYTECODE_READER_H
#define CABIN_BYTECODE_READER_H

#include <assert.h>
#include <string.h>
#include "cabin.h"
#include "convert.h"

typedef struct bytecode_reader {
    u1 *bytecode;
    size_t len; // bytecode len

    size_t pc; // program count
} BytecodeReader;

static inline void bcr_init(BytecodeReader *r, u1 *bytecode, size_t len)
{
    assert(r != NULL && bytecode != NULL);

    r->bytecode = bytecode;
    r->len = len;
    r->pc = 0;
}

static inline u1 *bcr_curr_pos(BytecodeReader *r)
{
    assert(r != NULL && r->pc < r->len);
    return r->bytecode + r->pc;
}

// @offset: 相对于当前位置的偏移
static inline void bcr_setu1(BytecodeReader *r, int offset, u1 value)
{
    assert(r != NULL);
    size_t pc0 = r->pc + offset;
    assert(pc0 < r->len);
    r->bytecode[pc0] = value;
}

// @offset: 相对于当前位置的偏移
// 注意，setu2方法要和readu2方法相配套。
static inline void bcr_setu2(BytecodeReader *r, int offset, u2 value)
{
    assert(r != NULL);
    size_t pc0 = r->pc + offset;
    assert(pc0 + 1 < r->len);
    r->bytecode[pc0] = (u1) (value >> 8);
    r->bytecode[pc0 + 1] = (u1) value;
}

static inline bool bcr_has_more(BytecodeReader *r)
{
    assert(r != NULL);
    return r->pc < r->len;
}

static inline void bcr_skip(BytecodeReader *r, int offset)
{
    assert(r != NULL);
    r->pc += offset;
    assert(r->pc < r->len);
}

/*
 * todo 函数干什么用的
 */
static inline void bcr_align4(BytecodeReader *r)
{
    assert(r != NULL);
    while (r->pc % 4 != 0) {
        r->pc++;
    }
    assert(r->pc < r->len);
}

static inline void bcr_read_bytes(BytecodeReader *r, u1 *buf, size_t len)
{
    assert(r != NULL && buf != NULL);

    memcpy(buf, r->bytecode + r->pc, len);
    r->pc += len;
}

static inline s1 bcr_reads1(BytecodeReader *r)
{
    assert(r != NULL && r->pc < r->len);
    return r->bytecode[r->pc++];
}

static inline u1 bcr_readu1(BytecodeReader *r)
{
    assert(r != NULL && r->pc < r->len);
    return (u1) r->bytecode[r->pc++];
}

static inline u2 bcr_readu2(BytecodeReader *r)
{
    assert(r != NULL && r->pc < r->len);
    u2 x = bcr_readu1(r);
    u2 y = (u2) (bcr_readu1(r) & 0x00ff);

    return x << 8 | y;
}

static inline u2 bcr_peeku2(BytecodeReader *r)
{
    assert(r != NULL && r->pc < r->len);
    u2 data = bcr_readu2(r);
    r->pc -= 2;
    return data;
}

static inline s2 bcr_reads2(BytecodeReader *r)
{
    assert(r != NULL && r->pc < r->len);
    return bcr_readu2(r);
}

static inline u4 bcr_readu4(BytecodeReader *r)
{
    assert(r != NULL && r->pc < r->len);
    u1 buf[4];
    bcr_read_bytes(r, buf, 4);

    return (u4) bytes_to_int32(buf);  // should be bytesToUint32  todo
}

static inline u8 bcr_readu8(BytecodeReader *r)
{
    assert(r != NULL && r->pc < r->len);
    const u1 *p = r->bytecode;
    u8 v = ((u8)(p)[0]<<56)
           |((u8)(p)[1]<<48)
           |((u8)(p)[2]<<40)
           |((u8)(p)[3]<<32)
           |((u8)(p)[4]<<24)
           |((u8)(p)[5]<<16)
           |((u8)(p)[6]<<8)
           |(u8)(p)[7];
    r->pc += 8;
    return v;
}

static inline s4 bcr_reads4(BytecodeReader *r)
{
    assert(r != NULL && r->pc < r->len);
    u1 buf[4];
    bcr_read_bytes(r, buf, 4);

    return (s4) bytes_to_int32(buf);
}

/*
 * 读 @n 个s4数据到 @s4s 数组中
 */
static inline void bcr_reads4s(BytecodeReader *r, int n, s4 *s4s)
{
    assert(r != NULL && r->pc < r->len);
    for (int i = 0; i < n; i++) {
        u1 buf[4];
        bcr_read_bytes(r, buf, 4);

        s4s[i] = (s4) bytes_to_int32(buf);
    }
}

#endif // CABIN_BYTECODE_READER_H