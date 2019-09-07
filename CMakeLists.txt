cmake_minimum_required(VERSION 3.9)
project(kayovm)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "-Wall -pthread")

add_subdirectory(zlib)


add_executable(kayovm src/kayo.h src/jtypes.h src/rtda/heap/Object.cpp src/rtda/heap/Object.h src/classfile/constant.h src/util/BytecodeReader.h src/util/convert.cpp src/util/convert.h src/classfile/Attribute.cpp src/classfile/Attribute.h src/util/encoding.h src/kayo.cpp src/native/registry.cpp src/native/registry.h src/rtda/thread/Frame.cpp src/rtda/thread/Frame.h src/slot.h src/rtda/ma/Member.cpp src/rtda/ma/Member.h src/rtda/ma/Method.cpp src/rtda/ma/Method.h src/rtda/ma/Class.cpp src/rtda/ma/Class.h src/rtda/thread/Thread.cpp src/rtda/thread/Thread.h src/rtda/ma/Access.h src/rtda/ma/Field.cpp src/rtda/ma/Field.h src/loader/ClassLoader.cpp src/loader/ClassLoader.h src/native/java/io/FileDescriptor.cpp src/native/java/io/FileInputStream.cpp src/native/java/io/FileOutputStream.cpp src/native/java/lang/Class.cpp src/native/java/lang/Double.cpp src/native/java/lang/Float.cpp src/native/java/lang/Object.cpp src/native/java/lang/String.cpp src/native/java/lang/System.cpp src/native/java/lang/Thread.cpp src/native/java/lang/Throwable.cpp src/native/java/security/AccessController.cpp src/native/sun/misc/Unsafe.cpp src/native/sun/misc/VM.cpp src/native/sun/reflect/Reflection.cpp src/interpreter/interpreter.cpp src/interpreter/interpreter.h src/rtda/heap/StrPool.h src/util/encoding.cpp src/native/sun/reflect/NativeConstructorAccessorImpl.cpp src/native/sun/reflect/NativeMethodAccessorImpl.cpp src/native/sun/reflect/ConstantPool.cpp src/rtda/heap/ArrayObject.cpp src/rtda/heap/StringObject.cpp src/rtda/heap/ClassObject.cpp src/rtda/primitive_types.cpp src/rtda/primitive_types.h src/util/endianness.h src/native/java/util/concurrent/atomic/AtomicLong.cpp src/native/java/io/WinNTFileSystem.cpp src/native/java/lang/ClassLoader.cpp src/native/java/lang/ClassLoader-NativeLibrary.cpp src/native/sun/misc/Signal.cpp src/native/sun/io/Win32ErrorMode.cpp src/output.cpp src/output.h src/native/java/lang/Runtime.cpp src/native/sun/misc/Version.cpp src/native/java/lang/reflect/Field.cpp src/native/java/lang/reflect/Executable.cpp src/native/java/nio/Bits.cpp src/rtda/heap/ArrayObject.h src/rtda/heap/ClassObject.h src/rtda/heap/StringObject.h src/heapmgr/HeapMgr.cpp src/heapmgr/HeapMgr.h src/symbol.cpp src/symbol.h src/utf8.cpp src/utf8.h src/rtda/ma/resolve.cpp src/rtda/ma/resolve.h src/config.h src/heapmgr/gc.cpp src/heapmgr/gc.h src/debug.h src/loader/bootstrap_class_loader.cpp src/loader/bootstrap_class_loader.h src/rtda/ma/ConstantPool.h src/rtda/ma/ArrayClass.cpp src/rtda/ma/ArrayClass.h src/rtda/ma/PrimitiveClass.h src/exceptions.cpp src/exceptions.h)

#target_link_libraries (kayovm libz)
#target_link_libraries (kayovm libminizip)


target_link_libraries(kayovm zlibsrc)
