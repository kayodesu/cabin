/*
 * Author: Yo Ka
 */

#ifndef JVM_CONVERT_H
#define JVM_CONVERT_H

#include "../jtypes.h"

/*
 * 将字节数组转换为32位整形.
 * 字节数组bytes按大端存储，长度4.
 */
int32_t bytes_to_int32(const uint8_t bytes[4]);

/*
 * 将字节数组转换为64位整形.
 * 字节数组bytes按大端存储，长度8.
 */
int64_t bytes_to_int64(const uint8_t bytes[8]);

jfloat int_bits_to_float(jint i);

/*
 * 将字节数组转换为32位浮点数.
 * 字节数组bytes按大端存储，长度4.
 */
jfloat bytes_to_float(const uint8_t bytes[4]);

jdouble long_bits_to_double(jlong l);

/*
 * 将字节数组转换为64位浮点数.
 * 字节数组bytes按大端存储，长度8.
 */
jdouble bytes_to_double(const uint8_t bytes[8]);

jint float_to_raw_int_bits(jfloat f);

jlong double_to_raw_long_bits(jdouble d);

#endif //JVM_CONVERT_H
