/*
 * Author: Jia Yang
 */

#include "convert.h"

union fi {
    jint i;
    jfloat f;

//    explicit FI(jint i): i(i) { }
//    explicit FI(jfloat f): f(f) { }
};

union dl {
    jlong l;
    jdouble d;

//    explicit DL(jlong l): l(l) { }
//    explicit DL(jdouble d): d(d) { }
};

int32_t bytes_to_int32(const uint8_t bytes[4])
{
    int32_t high = bytes[0] << 24;
    int32_t mid_high = (bytes[1] << 16) & 0x00ff0000;
    int32_t mid_low = (bytes[2] << 8) & 0x0000ff00;
    int32_t low = bytes[3] & 0x000000ff;
    return high | mid_high | mid_low | low;
}

int64_t bytes_to_int64(const uint8_t bytes[8])
{
    int64_t high = ((int64_t) bytes_to_int32(bytes)) << 32;
    int64_t low = bytes_to_int32(bytes + 4) & 0x00000000ffffffff;
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

jfloat int_bits_to_float(jint i)
{
    union fi fi;
    fi.i = i;
    return fi.f;   // todo
}

jfloat bytes_to_float(const uint8_t bytes[4])
{
    return int_bits_to_float(bytes_to_int32(bytes));
}

jdouble long_bits_to_double(jlong l)
{
    union dl dl;
    dl.l = l;
    return dl.d;   // todo
}

jdouble bytes_to_double(const uint8_t bytes[8])
{
    return long_bits_to_double(bytes_to_int64(bytes));
}

jint float_to_raw_int_bits(jfloat f)
{
    union fi fi;
    fi.f = f;
    return fi.i;  // todo
}

jlong double_to_raw_long_bits(jdouble d)
{
    union dl dl;
    dl.d = d;
    return dl.l;  // todo
}