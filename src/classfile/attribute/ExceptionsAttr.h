/*
 * Author: Jia Yang
 */

#ifndef JVM_EXCEPTIONSATTR_H
#define JVM_EXCEPTIONSATTR_H

#include "Attribute.h"

/*
 * The Exceptions attribute indicates which checked exceptions a method may throw.
 * There may be at most one Exceptions attribute in the attributes table of a method_info structure.
 */
struct ExceptionsAttr: public Attribute {
    u2 numberOfExceptions;
    /*
     * Each value in the exception_index_table array must be a valid index into
     * the constant_pool table.
     * The constant_pool entry at that index must be a CONSTANT_Class_info structure
     * representing a class type that this method is declared to throw.
     */
    u2 *exceptionIndexTable; // [number_of_exceptions];

    explicit ExceptionsAttr(BytecodeReader &reader): Attribute(reader) {
        numberOfExceptions = reader.readu2();
        exceptionIndexTable = new u2[numberOfExceptions];
        for (int i = 0; i < numberOfExceptions; i++) {
            exceptionIndexTable[i] = reader.readu2();
        }
    }
};

#endif //JVM_EXCEPTIONSATTR_H
