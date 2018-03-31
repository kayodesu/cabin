/*
 * Author: Jia Yang
 */

#ifndef JVM_LOCALVARIABLETABLEATTR_H
#define JVM_LOCALVARIABLETABLEATTR_H

#include "Attribute.h"

/*
 * The LocalVariableTable attribute is an optional variable-length attribute in the
attributes table of a Code attribute (§4.7.3). It may be used by debuggers to
determine the value of a given local variable during the execution of a method.
If multiple LocalVariableTable attributes are present in the attributes table of
a Code attribute, then they may appear in any order.
There may be no more than one LocalVariableTable attribute per local variable
in the attributes table of a Code attribute.
 */
struct LocalVariableTableAttr: public Attribute   {
    u2 localVariableTableLength;
    struct Table {
        u2 startPc;
        u2 length;
        u2 nameIndex;
        u2 descriptorIndex;
        u2 index;
    }; // [local_variable_table_length];

    Table *localVariableTables;

    explicit LocalVariableTableAttr(BytecodeReader &reader): Attribute(reader) {
        localVariableTableLength = reader.readu2();
        localVariableTables = new Table[localVariableTableLength];
        for (int i = 0; i < localVariableTableLength; i++) {
            localVariableTables[i].startPc = reader.readu2();
            localVariableTables[i].length = reader.readu2();;
            localVariableTables[i].nameIndex = reader.readu2();;
            localVariableTables[i].descriptorIndex = reader.readu2();
            localVariableTables[i].index = reader.readu2();
        }
    }
};

#endif //JVM_LOCALVARIABLETABLEATTR_H
