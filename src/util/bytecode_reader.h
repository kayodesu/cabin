/*
 * Author: Jia Yang
 */

#ifndef JVM_BYTECODE_READER_H
#define JVM_BYTECODE_READER_H

#include <stdbool.h>
#include <limits.h>
#include <stddef.h>
#include "../jtypes.h"

struct bytecode_reader {
    const s1 *bytecode;
    size_t len;  // bytecode len
    size_t pc;   // program count

//    bool (* has_more)(const struct bytecode_reader *reader);
//    bool (* set_pc)(struct bytecode_reader *reader, size_t newPc);
//    bool (* skip)(struct bytecode_reader *reader, int offset);
//    bool (* skip_padding)(struct bytecode_reader *reader);
//    void (* read_bytes)(struct bytecode_reader *reader, u1 *buf, size_t len);
//    s1 (* reads1)(struct bytecode_reader *reader);
//    u1 (* readu1)(struct bytecode_reader *reader);
//    u2 (* readu2)(struct bytecode_reader *reader);
//    s2 (* reads2)(struct bytecode_reader *reader);
//    u4 (* readu4)(struct bytecode_reader *reader);
//    s4 (* reads4)(struct bytecode_reader *reader);
//    void (* reads4s)(struct bytecode_reader *reader, int n, s4 *s4s);
//    void (* destroy)(struct bytecode_reader *reader);
};

/*
 * 'bcr' is short of 'bytecode reader'.
 */
struct bytecode_reader* bcr_create(const s1 *bytecode, size_t len);

void bcr_destroy(struct bytecode_reader *reader);

bool bcr_has_more(const struct bytecode_reader *reader);

bool bcr_set_pc(struct bytecode_reader *reader, size_t new_pc);

bool bcr_skip(struct bytecode_reader *reader, int offset);

/*
 * todo 函数干什么用的
 */
bool bcr_skip_padding(struct bytecode_reader *reader);

void bcr_read_bytes(struct bytecode_reader *reader, u1 *buf, size_t len);

s1 bcr_reads1(struct bytecode_reader *reader);

u1 bcr_readu1(struct bytecode_reader *reader);

u2 bcr_readu2(struct bytecode_reader *reader);

s2 bcr_reads2(struct bytecode_reader *reader);

u4 bcr_readu4(struct bytecode_reader *reader);

s4 bcr_reads4(struct bytecode_reader *reader);

/*
 * 读 @n 个s4数据到 @s4s 数组中
 */
void bcr_reads4s(struct bytecode_reader *reader, int n, s4 *s4s);


#endif //JVM_BYTECODE_READER_H
