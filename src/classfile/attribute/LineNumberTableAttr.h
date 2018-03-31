/*
 * Author: Jia Yang
 */

#ifndef JVM_LINENUMBERTABLEATTR_H
#define JVM_LINENUMBERTABLEATTR_H

#include "Attribute.h"

/*
 * The LineNumberTable attribute is an optional variable-length attribute in the
attributes table of a Code attribute (§4.7.3). It may be used by debuggers to
determine which part of the code array corresponds to a given line number in the
original source file.
If multiple LineNumberTable attributes are present in the attributes table of a
Code attribute, then they may appear in any order.
There may be more than one LineNumberTable attribute per line of a source file
in the attributes table of a Code attribute. That is, LineNumberTable attributes
may together represent a given line of a source file, and need not be one-to-one
with source lines.
 */
struct LineNumberTableAttr: public Attribute  {
    u2 lineNumberTableLength;
    struct Table {
        /*
         * The value of the start_pc item must indicate the index into the code array
at which the code for a new line in the original source file begins.
The value of start_pc must be less than the value of the code_length
item of the Code attribute of which this LineNumberTable is an attribute.
         */
        u2 startPc;
        // The value of the line_number item must give the corresponding line number in the original source file.
        u2 lineNumber;
    };
    Table *lineNumberTables; //[line_number_table_length];

    explicit LineNumberTableAttr(BytecodeReader &reader): Attribute(reader) {
        lineNumberTableLength = reader.readu2();
        lineNumberTables = new Table[lineNumberTableLength];
        for (int i = 0; i < lineNumberTableLength; i++) {
            lineNumberTables[i].startPc = reader.readu2();
            lineNumberTables[i].lineNumber = reader.readu2();
        }
    }
};

#endif //JVM_LINENUMBERTABLEATTR_H
