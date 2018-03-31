/*
 * Author: Jia Yang
 */

#include "convert.h"

/*
 * 将字节数组转换为32位整形.
 * 字节数组bytes按大端存储，长度4.
 */
int32_t jvm::convert::bytesToInt32(const uint8_t bytes[4]) {
    int32_t high = bytes[0] << 24;
    int32_t mid_high = (bytes[1] << 16) & 0x00ff0000;
    int32_t mid_low = (bytes[2] << 8) & 0x0000ff00;
    int32_t low = bytes[3] & 0x000000ff;
    return high | mid_high | mid_low | low;
}

/*
 * 将字节数组转换为64位整形.
 * 字节数组bytes按大端存储，长度8.
 */
int64_t jvm::convert::bytesToInt64(const uint8_t bytes[8]) {
    int64_t high = ((int64_t) bytesToInt32(bytes)) << 32;
    int64_t low = bytesToInt32(bytes + 4) & 0x00000000ffffffff;
    return high | low;
}

/*
 *
 float intBitsToFloat( int bits ) {
    // s 为符号（sign）；e 为指数（exponent）；m 为有效位数（mantissa）
int
        s = ( bits >> 31 ) == 0 ? 1 : -1,
        e = ( bits >> 23 ) & 0xff,
        m = ( e == 0 ) ?
            ( bits & 0x7fffff ) << 1 :
            ( bits & 0x7fffff ) | 0x800000;
return s * m * ( float ) pow( 2, e - 150 );
}
 */

jfloat jvm::convert::intBitsToFloat(jint i) {
    jvm::convert::FI fi;
    fi.i = i;
    return fi.f;   // todo
}

/*
 * 将字节数组转换为32位浮点数.
 * 字节数组bytes按大端存储，长度4.
 */
jfloat jvm::convert::bytesToFloat(const uint8_t bytes[4]) {
    return intBitsToFloat(bytesToInt32(bytes));
}

jdouble jvm::convert::longBitsToDouble(jlong l) {
    jvm::convert::DL dl;
    dl.l = l;
    return dl.d;   // todo
}

/*
 * 将字节数组转换为64位浮点数.
 * 字节数组bytes按大端存储，长度8.
 */
jdouble jvm::convert::bytesToDouble(const uint8_t bytes[8]) {
    return longBitsToDouble(bytesToInt64(bytes));
}

jint jvm::convert::floatToRawIntBits(jfloat f) {
    jvm::convert::FI fi;
    fi.f = f;
    return fi.i;  // todo
}

jlong jvm::convert::doubleToRawLongBits(jdouble d) {
    jvm::convert::DL dl;
    dl.d = d;
    return dl.l;  // todo
}