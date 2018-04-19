cmake_minimum_required(VERSION 3.6)
project(jvm)

set(CMAKE_CXX_STANDARD 17)

add_subdirectory(lib/zlib/minizip)

add_library(libz0 STATIC IMPORTED)
set_target_properties(libz0 PROPERTIES IMPORTED_LOCATION ${CMAKE_CURRENT_SOURCE_DIR}/lib/zlib/libz0.a)

set(SOURCE_FILES src/main.cpp
        src/classfile/ClassFile.cpp src/classfile/ClassFile.h src/classfile/Constant.cpp src/classfile/Constant.h
        src/instruction/comparisons.h src/instruction/constants.h src/instruction/controls.h src/instruction/loads.h src/instruction/maths.h src/instruction/references.h src/instruction/stacks.h src/instruction/stores.h src/instruction/instruction.cpp
        src/interpreter/interpreter.cpp src/interpreter/StackFrame.cpp src/interpreter/StackFrame.h
        src/interpreter/Slot.cpp src/interpreter/Slot.h src/rtda/thread/Jthread.cpp src/rtda/thread/Jthread.h src/rtda/heap/methodarea/Jclass.cpp src/rtda/heap/methodarea/Jclass.h src/rtda/heap/objectarea/Jobject.cpp src/rtda/heap/objectarea/Jobject.h src/loader/ClassLoader.cpp src/loader/ClassLoader.h src/rtda/heap/heap.cpp src/rtda/heap/heap.h src/interpreter/interpreter.h
        src/rtda/heap/objectarea/JarrayObj.cpp src/rtda/heap/objectarea/JarrayObj.h src/BytecodeReader.h src/rtda/heap/methodarea/Jmember.cpp src/rtda/heap/methodarea/Jmember.h src/rtda/heap/methodarea/RTCP.cpp src/rtda/heap/methodarea/RTCP.h src/rtda/heap/methodarea/AccessPermission.h src/util/encoding.h src/rtda/heap/methodarea/resolver.h src/util/convert.cpp src/util/convert.h
        src/classfile/attribute/Attribute.cpp src/classfile/attribute/Attribute.h src/classfile/attribute/BootstrapMethodsAttr.cpp src/classfile/attribute/BootstrapMethodsAttr.h src/classfile/attribute/CodeAttr.cpp src/classfile/attribute/CodeAttr.h src/classfile/attribute/ConstantValueAttr.h src/classfile/attribute/DeprecatedAttr.h src/classfile/attribute/EnclosingMethodAttr.h src/classfile/attribute/ExceptionsAttr.h src/classfile/attribute/InnerClassesAttr.h src/classfile/attribute/LineNumberTableAttr.h src/classfile/attribute/LocalVariableTableAttr.h src/classfile/attribute/LocalVariableTypeTableAttr.h src/classfile/attribute/MethodParametersAttr.h src/classfile/attribute/SignatureAttr.h src/classfile/attribute/SourceDebugExtensionAttr.h src/classfile/attribute/SourceFileAttr.h src/classfile/attribute/StackMapTableAttr.h src/classfile/attribute/SyntheticAttr.h src/classfile/attribute/ElementValue.h src/classfile/attribute/Annotation.h src/classfile/attribute/AnnotationDefaultAttr.h src/classfile/attribute/RuntimeAnnotationsAttr.h src/classfile/attribute/RuntimeParameterAnnotationsAttr.h src/classfile/attribute/UnknownAttr.h src/classfile/attribute/TypeAnnotation.h src/classfile/attribute/ElementValue.cpp src/classfile/attribute/Annotation.cpp src/native/java/lang/Throwable.cpp src/rtda/heap/methodarea/Jmethod.cpp src/rtda/heap/methodarea/Jmethod.h src/rtda/heap/methodarea/Jfield.cpp src/rtda/heap/methodarea/Jfield.h src/native/java/io/FileOutputStream.cpp src/jvm.cpp src/rtda/heap/methodarea/PrimitiveType.h src/instruction/extended.h

        src/native/registry.cpp src/native/registry.h src/native/java/lang/Class.cpp src/native/java/lang/Float.cpp src/native/java/lang/Object.cpp src/native/java/lang/System.cpp src/native/java/lang/String.cpp src/rtda/heap/objectarea/strpool.cpp src/rtda/heap/objectarea/strpool.h src/rtda/heap/objectarea/JclassObj.cpp src/rtda/heap/objectarea/JclassObj.h src/native/java/lang/Double.cpp src/rtda/heap/objectarea/JstringObj.cpp src/rtda/heap/objectarea/JstringObj.h src/native/sun/misc/VM.cpp src/native/java/io/FileInputStream.cpp src/native/java/io/FileDescriptor.cpp src/native/sun/misc/Unsafe.cpp src/native/sun/reflect/Reflection.cpp src/native/java/security/AccessController.cpp src/native/java/lang/Thread.cpp src/jvm.h src/classpath/ClassReader.cpp src/classpath/ClassReader.h src/Base.cpp src/Base.h src/loader/BootstrapClassLoader.cpp src/loader/BootstrapClassLoader.h src/exception/IoException.cpp src/exception/IoException.h src/exception/ClassNotFindException.h)
add_executable(jvm ${SOURCE_FILES})
target_link_libraries(jvm libz0)
target_link_libraries(jvm minizip)