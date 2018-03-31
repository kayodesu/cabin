/*
 * Author: Jia Yang
 */

#ifndef JVM_STACKMAPTABLEATTR_H
#define JVM_STACKMAPTABLEATTR_H

#include "Attribute.h"

struct StackMapTableAttr: public Attribute {
    u2 numberOfEntries;
//    stack_map_frame entries[number_of_entries];   // todo

    explicit StackMapTableAttr(BytecodeReader &reader): Attribute(reader) {
        numberOfEntries = reader.readu2();

//    jvm_printf("error. not parse attr: stack_map_table\n");  // todo
        // 跳过剩下的部分，先不处理。  todo
        reader.skip((int) attributeLength - 2);
//        bytecode_reader_readbytes(reader, NULL, (int) attribute_length - 2);
    }
};

#endif //JVM_STACKMAPTABLEATTR_H
