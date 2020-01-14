/*
 * Author: kayo
 */

#ifndef JVM_PRIM_H
#define JVM_PRIM_H

#include "../jtypes.h"

/*
 * 基本类型的名称，描述符，等等
 */
class Prims {
    struct Info {
        const utf8_t *className;
        utf8_t descriptor;
        const utf8_t *arrayClassName;
        const utf8_t *wrapperClassName;
    };
    static Info prims[9];

public:
    static void init();

    static bool isPrimClassName(const utf8_t *className);
    static bool isPrimDescriptor(utf8_t descriptor);

    static const utf8_t *getArrayClassName(const utf8_t *className);

    static const utf8_t *descriptor2className(utf8_t descriptor);
};
#endif // JVM_PRIM_H