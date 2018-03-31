/*
 * Author: Jia Yang
 */

#ifndef JVM_LOCALVARIABLETYPETABLEATTR_H
#define JVM_LOCALVARIABLETYPETABLEATTR_H

#include "Attribute.h"

/*
 *The LocalVariableTypeTable attribute is an optional variable-length attribute in
the attributes table of a Code attribute (§4.7.3). It may be used by debuggers to
determine the value of a given local variable during the execution of a method.
If multiple LocalVariableTypeTable attributes are present in the attributes
table of a given Code attribute, then they may appear in any order.
There may be no more than one LocalVariableTypeTable attribute per local
variable in the attributes table of a Code attribute.
The LocalVariableTypeTable attribute differs from the LocalVariableTable
attribute (§4.7.13) in that it provides signature information rather than descriptor
information. This difference is only significant for variables whose type uses a type variable
or parameterized type. Such variables will appear in both tables, while variables of other
types will appear only in LocalVariableTable.
 */
struct LocalVariableTypeTableAttr: public Attribute  {
    u2 localVariableTypeTableLength;
    struct Table{
        u2 startPc;
        u2 length;
        u2 nameIndex;
        u2 signatureIndex;
        u2 index;
    };
    
    Table *localVariableTypeTables;

    explicit LocalVariableTypeTableAttr(BytecodeReader &reader): Attribute(reader) {
        localVariableTypeTableLength = reader.readu2();
        localVariableTypeTables = new Table[localVariableTypeTableLength];//malloc(sizeof(*local_variable_type_table) * local_variable_type_table_length);
        for (int i = 0; i < localVariableTypeTableLength; i++) {
            localVariableTypeTables[i].startPc = reader.readu2();
            localVariableTypeTables[i].length = reader.readu2();
            localVariableTypeTables[i].nameIndex = reader.readu2();
            localVariableTypeTables[i].signatureIndex = reader.readu2();
            localVariableTypeTables[i].index = reader.readu2();
        }
    }
};

#endif //JVM_LOCALVARIABLETYPETABLEATTR_H
