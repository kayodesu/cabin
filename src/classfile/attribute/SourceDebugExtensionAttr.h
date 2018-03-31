/*
 * Author: Jia Yang
 */

#ifndef JVM_SOURCEDEBUGEXTENSIONATTR_H
#define JVM_SOURCEDEBUGEXTENSIONATTR_H

#include "Attribute.h"


/*
 * The SourceDebugExtension attribute is an optional attribute in the attributes
table of a ClassFile structure (§4.1).
There may be at most one SourceDebugExtension attribute in the attributes
table of a ClassFile structure.
 */
struct SourceDebugExtensionAttr: public Attribute {
    /*
     * The debug_extension array holds extended debugging information which has
no semantic effect on the Java Virtual Machine. The information is represented
using a modified UTF-8 string (§4.4.7) with no terminating zero byte.

Note that the debug_extension array may denote a string longer than that which can be
represented with an instance of class String.
     */
    u1 *debugExtension; //[attribute_length];

    explicit SourceDebugExtensionAttr(BytecodeReader &reader): Attribute(reader) {
        debugExtension = new u1[attributeLength];//
        for (int i = 0; i < attributeLength; i++) {
            debugExtension[i] = reader.readu1();
        }
    }
};

#endif //JVM_SOURCEDEBUGEXTENSIONATTR_H
