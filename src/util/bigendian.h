/*
 * Author: Jia Yang
 */

#ifndef JVM_BIGENDIAN_H
#define JVM_BIGENDIAN_H

/*
 * 大端(big endian):低地址存放高字节
 * 小端(little endian):低字节存放低字节
 */

static const union { char c[4]; unsigned long l; } endian_test = { { 'l', '?', '?', 'b' } };

// Endianness(字节顺序)
#define ENDIANNESS ((char) endian_test.l)

static inline jbyte swap_jbyte_endian(jbyte x)
{
    return x;
}

static inline jchar swap_jchar_endian(jchar x)
{
    return (jchar) ( ((x & 0xff00) >> 8) | ((x & 0x00ff) << 8) );
}

static inline jshort swap_jshort_endian(jshort x)
{
    return (jshort) ( ((x & 0xff00) >> 8) | ((x & 0x00ff) << 8) );
}

static inline jint swap_jint_endian(jint x)
{
    return ((  x & 0xff000000) >> 24)
            | ((x & 0x00ff0000) >> 8)
            | ((x & 0x0000ff00) << 8)
            | ((x & 0x000000ff) << 24);
}

static inline jlong swap_jlong_endian(jlong x)
{
    return ((  x & 0x00000000000000ff) << 56)
            | ((x & 0x000000000000ff00) << 40)
            | ((x & 0x0000000000ff0000) << 24)
            | ((x & 0x00000000ff000000) << 8)
            | ((x & 0x000000ff00000000) >> 8)
            | ((x & 0x0000ff0000000000) >> 24)
            | ((x & 0x00ff000000000000) >> 40)
            | ((x & 0xff00000000000000) >> 56);
}

static inline jfloat swap_jfloat_endian(jfloat x)
{
    /*
     * float不适用于操作符 '&'
     * 所以用下面的方式来交换字节
     */
    static union {
        jfloat f;
        char c[4];
    } t1, t2;

    t1.f = x;
    t2.c[0] = t1.c[3];
    t2.c[1] = t1.c[2];
    t2.c[2] = t1.c[1];
    t2.c[3] = t1.c[0];

    return t2.f;
}

static inline jdouble swap_jdouble_endian(jdouble x)
{
    static union {
        jdouble d;
        char c[8];
    } t1, t2;

    t1.d = x;
    t2.c[0] = t1.c[7];
    t2.c[1] = t1.c[6];
    t2.c[2] = t1.c[5];
    t2.c[3] = t1.c[4];
    t2.c[4] = t1.c[3];
    t2.c[5] = t1.c[2];
    t2.c[6] = t1.c[1];
    t2.c[7] = t1.c[0];

    return t2.d;
}


#define BIGENDIAN(x) ( ENDIANNESS == 'b' ? (x) : _Generic(x, \
                                                jbyte: swap_jbyte_endian, \
                                                jchar: swap_jchar_endian, \
                                                jshort: swap_jshort_endian, \
                                                jint: swap_jint_endian, \
                                                jlong: swap_jlong_endian, \
                                                jfloat: swap_jfloat_endian, \
                                                jdouble: swap_jdouble_endian)(x) )

#endif //JVM_BIGENDIAN_H
