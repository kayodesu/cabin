/*
 * Author: Yo Ka
 */

#ifndef JVM_ENDIANNESS_H
#define JVM_ENDIANNESS_H

/*
 * 大端(big endian):低地址存放高字节
 * 小端(little endian):低字节存放低字节
 */

static const union { char c[4]; unsigned long l; } endian_test = { { 'l', '?', '?', 'b' } };

// Endianness(字节顺序)
#define ENDIANNESS ((char) endian_test.l)

static inline jbyte swapEndian(jbyte x)
{
    return x;
}

static inline jchar swapEndian(jchar x)
{
    return (jchar) ( ((x & 0xff00) >> 8) | ((x & 0x00ff) << 8) );
}

static inline jshort swapEndian(jshort x)
{
    return (jshort) ( ((x & 0xff00) >> 8) | ((x & 0x00ff) << 8) );
}

static inline jint swapEndian(jint x)
{
    return ((  x & 0xff000000) >> 24)
            | ((x & 0x00ff0000) >> 8)
            | ((x & 0x0000ff00) << 8)
            | ((x & 0x000000ff) << 24);
}

static inline jlong swapEndian(jlong x)
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

static inline jfloat swapEndian(jfloat x)
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

static inline jdouble swapEndian(jdouble x)
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

template <typename T>
static inline T changeToBigEndian(T x)
{
    return ENDIANNESS == 'b' ? x : swapEndian(x);
}

#endif //JVM_ENDIANNESS_H
