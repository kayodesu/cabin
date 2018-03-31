/*
 * Author: Jia Yang
 */

#ifndef JVM_BYTECODEREADER_H
#define JVM_BYTECODEREADER_H


#include <cstdio>
#include <cstdlib>
#include "jvmdef.h"
#include "util/convert.h"
#include "jvmtype.h"

class BytecodeReader {
public:
    const s1 *bytecode;
    size_t len;  // bytecode len
    size_t pc;   // program count

public:

    BytecodeReader(const s1 *bytecode0, size_t len0): bytecode(bytecode0), len(len0), pc(0) {}

//    void reset() {
//        pc = 0;
//    }

    bool hasMore() {
        return pc < len;
    }

    bool setPc(size_t new_pc) {
        if (new_pc >= len) {
            // todo error
            jvmAbort("error. new_pc = %zu, len = %zu\n", new_pc, len);
        }
        pc = new_pc;
        return true;
    }

    bool skip(int offset) {
        return setPc(pc + offset);
    }

    void readbytes(u1 *buf, size_t len) {
        if (buf == nullptr) {
            jvmAbort(0, "error\n");
        }

        memcpy(buf, bytecode + pc, len);
        pc += len;
    }

    s1 reads1() {
        return bytecode[pc++];
    }

    u1 readu1() {
        return (u1) bytecode[pc++];
    }

    u2 readu2() {
        u2 x = readu1();
        u2 y = (u2) (readu1() & 0x00ff);

        return x << 8 | y;
    }

    s2 reads2() {
        return readu2();
    }

    u4 readu4() {
        u1 buf[4];
        readbytes(buf, 4);

        return (u4) jvm::convert::bytesToInt32(buf);
    }
};


#endif //JVM_BYTECODEREADER_H
