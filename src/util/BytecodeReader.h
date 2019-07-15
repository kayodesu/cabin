/*
 * Author: Jia Yang
 */

#ifndef JVM_BYTECODE_READER_H
#define JVM_BYTECODE_READER_H

#include <cassert>
#include <cstring>
#include "../jtypes.h"
#include "../jvm.h"
#include "convert.h"

class BytecodeReader {
    const u1 *bytecode = nullptr;
    size_t len;  // bytecode len

public:
    size_t pc = 0;   // program count

    BytecodeReader(const u1 *bytecode, size_t len): bytecode(bytecode), len(len) { }

    const u1 *currPos()
    {
        return bytecode + pc;
    }

    bool hasMore()
    {
        return pc < len;
    }

    void skip(int offset)
    {
        pc += offset;
    }

    /*
     * todo 函数干什么用的
     */
    void align4()
    {
        while (pc % 4 != 0) {
            pc++;
        }
    }

    void readBytes(u1 *buf, size_t len)
    {
        assert(buf != nullptr);

        memcpy(buf, bytecode + pc, len);
        pc += len;
    }

    s1 reads1()
    {
        return bytecode[pc++];
    }

    u1 readu1()
    {
        return (u1) bytecode[pc++];
    }

    u2 readu2()
    {
        u2 x = readu1();
        u2 y = (u2) (readu1() & 0x00ff);

        return x << 8 | y;
    }

    u2 peeku2()
    {
        u2 data = readu2();
        pc -= 2;
        return data;
    }

    s2 reads2()
    {
        return readu2();
    }

    u4 readu4()
    {
        u1 buf[4];
        readBytes(buf, 4);

        return (u4) bytes_to_int32(buf);  // should be bytesToUint32  todo
    }

    u8 readu8()
    {
        const u1 *p = bytecode;
        u8 v = ((u8)(p)[0]<<56)
               |((u8)(p)[1]<<48)
               |((u8)(p)[2]<<40)
               |((u8)(p)[3]<<32)
               |((u8)(p)[4]<<24)
               |((u8)(p)[5]<<16)
               |((u8)(p)[6]<<8)
               |(u8)(p)[7];
        pc += 8;
        return v;
    }

    s4 reads4()
    {
        u1 buf[4];
        readBytes(buf, 4);

        return (s4) bytes_to_int32(buf);
    }

    /*
     * 读 @n 个s4数据到 @s4s 数组中
     */
    void reads4s(int n, s4 *s4s)
    {
        for (int i = 0; i < n; i++) {
            u1 buf[4];
            readBytes(buf, 4);

            s4s[i] = (s4) bytes_to_int32(buf);
        }
    }
};

#endif //JVM_BYTECODE_READER_H
