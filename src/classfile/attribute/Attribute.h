/*
 * Author: Jia Yang
 */

#ifndef JVM_ATTRIBUTE_H
#define JVM_ATTRIBUTE_H


#include "../../jvmtype.h"
#include "../../BytecodeReader.h"
#include "../Constant.h"


// attribute name
#define Code "Code"                                                                  //                 method
#define ConstantValue "ConstantValue"                                                //           field
#define Signature "Signature"                                                        // ClassFile field method
#define Synthetic "Synthetic"                                                        // ClassFile
#define Deprecated "Deprecated"                                                      // ClassFile field method
#define LineNumberTable "LineNumberTable"                                            //                        Code
#define StackMapTable "StackMapTable"                                                //                        Code
#define LocalVariableTable "LocalVariableTable"                                      //                        Code
#define LocalVariableTypeTable "LocalVariableTypeTable"                              //                        Code
#define Exceptions "Exceptions"                                                      //                 method
#define AnnotationDefault "AnnotationDefault"                                        //                 method
#define SourceFile "SourceFile"                                                      // ClassFile
#define InnerClasses "InnerClasses"                                                  // ClassFile
#define EnclosingMethod "EnclosingMethod"                                            // ClassFile
#define SourceDebugExtension "SourceDebugExtension"                                  // ClassFile
#define BootstrapMethods "BootstrapMethods"                                          // ClassFile
#define MethodParameters "MethodParameters"                                          //                 method
#define RuntimeVisibleAnnotations "RuntimeVisibleAnnotations"                        // ClassFile field method
#define RuntimeInvisibleAnnotations "RuntimeInvisibleAnnotations"                    // ClassFile field method
#define RuntimeVisibleParameterAnnotations "RuntimeVisibleParameterAnnotations"      //                 method
#define RuntimeInvisibleParameterAnnotations "RuntimeInvisibleParameterAnnotations"  //                 method


struct Attribute {
    u2 attributeNameIndex;
    u4 attributeLength;

    explicit Attribute(BytecodeReader &reader) {
        attributeLength = reader.readu4();
    }

    static Attribute* parseAttr(BytecodeReader &reader, Constant **constantPool);
};


#endif //JVM_ATTRIBUTE_H
