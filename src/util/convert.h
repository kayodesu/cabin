/*
 * Author: Jia Yang
 */

#ifndef JVM_CONVERT_H
#define JVM_CONVERT_H

#include <cstdint>
#include "../jvmtype.h"

namespace jvm {
    namespace convert {
        union FI {
            jint i;
            jfloat f;
        };

        union DL {
            jlong l;
            jdouble d;
        };

        /*
         * 将字节数组转换为32位整形.
         * 字节数组bytes按大端存储，长度4.
         */
        int32_t bytesToInt32(const uint8_t bytes[4]);

        /*
         * 将字节数组转换为64位整形.
         * 字节数组bytes按大端存储，长度8.
         */
        int64_t bytesToInt64(const uint8_t bytes[8]);

        jfloat intBitsToFloat(jint i);
        /*
         * 将字节数组转换为32位浮点数.
         * 字节数组bytes按大端存储，长度4.
         */
        jfloat bytesToFloat(const uint8_t bytes[4]);

        jdouble longBitsToDouble(jlong l);
        /*
         * 将字节数组转换为64位浮点数.
         * 字节数组bytes按大端存储，长度8.
         */
        jdouble bytesToDouble(const uint8_t bytes[8]);

        jint floatToRawIntBits(jfloat f);

        jlong doubleToRawLongBits(jdouble d);
    }
}

#endif //JVM_CONVERT_H
