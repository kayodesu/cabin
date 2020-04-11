cmake_minimum_required(VERSION 3.9)
project(kayo)

#set(CMAKE_C_FLAGS "-Wall -m32 -std=c11")
#set(CMAKE_CXX_FLAGS "-Wall -pthread -lz -lminizip -m32 -std=c++17")

set(CMAKE_CXX_STANDARD 17)
#set(CMAKE_CXX_FLAGS "-Wall -pthread -lz -lminizip -m32")
set(CMAKE_CXX_FLAGS "-Wall -pthread")

#set(CMAKE_C_FLAGS "-std=c99 -Wall -pthread -lz -lminizip -m32")

add_subdirectory(zlib)


add_executable(kayo src/kayo.h src/classfile/constants.h src/vmdef.h src/objects/object.cpp src/objects/prims.h src/objects/object.h src/util/bytecode_reader.h src/util/convert.cpp src/util/convert.h src/classfile/attributes.cpp src/classfile/attributes.h src/main.cpp src/runtime/frame.cpp src/runtime/frame.h src/objects/class.cpp src/objects/class.h src/runtime/thread_info.cpp src/runtime/thread_info.h src/native/java/io/FileDescriptor.cpp src/native/java/io/FileInputStream.cpp src/native/java/io/FileOutputStream.cpp src/native/java/lang/Class.cpp src/native/java/lang/Double.cpp src/native/java/lang/Float.cpp src/native/java/lang/Object.cpp src/native/java/lang/String.cpp src/native/java/lang/System.cpp src/native/java/lang/Thread.cpp src/native/java/lang/Throwable.cpp src/native/java/security/AccessController.cpp src/native/sun/misc/Unsafe.cpp src/native/sun/misc/VM.cpp src/native/sun/reflect/Reflection.cpp src/interpreter/interpreter.cpp src/interpreter/interpreter.h src/native/sun/reflect/NativeConstructorAccessorImpl.cpp src/native/sun/reflect/NativeMethodAccessorImpl.cpp src/native/sun/reflect/ConstantPool.cpp src/util/endianness.h src/native/java/util/concurrent/atomic/AtomicLong.cpp src/native/java/io/WinNTFileSystem.cpp src/native/java/lang/ClassLoader.cpp src/native/java/lang/ClassLoader-NativeLibrary.cpp src/native/sun/misc/Signal.cpp src/native/sun/io/Win32ErrorMode.cpp src/output.cpp src/output.h src/native/java/lang/Runtime.cpp src/native/sun/misc/Version.cpp src/native/java/lang/reflect/Field.cpp src/native/java/lang/reflect/Executable.cpp src/native/java/nio/Bits.cpp src/memory/Heap.h src/symbol.cpp src/symbol.h src/config.h src/gc/gc.cpp src/gc/gc.h src/debug.h src/objects/throwables.cpp src/objects/throwables.h src/objects/class_loader.cpp src/objects/class_loader.h src/native/sun/misc/URLClassPath.cpp src/native/java/util/zip/ZipFile.cpp src/util/encoding.cpp src/util/encoding.h src/native/sun/misc/Perf.cpp src/native/java/lang/Package.cpp src/native/java/io/RandomAccessFile.cpp src/native/java/lang/invoke/MethodHandleNatives.cpp src/native/java/lang/reflect/Array.cpp src/native/java/lang/reflect/Proxy.cpp src/memory/MemMgr.cpp src/memory/MemMgr.h src/memory/Heap.cpp src/native/java/lang/invoke/MethodHandle.cpp src/objects/prims.cpp src/objects/prims.h src/objects/invoke.cpp src/objects/invoke.h src/objects/Modifier.h src/native/sun/management/VMManagementImpl.cpp src/native/sun/management/ThreadImpl.cpp src/runtime/Monitor.cpp src/runtime/Monitor.h src/native/jdk/internal/misc/VM.cpp src/native/jdk/internal/misc/Unsafe.cpp src/native/jdk/internal/util/SystemProps-Raw.cpp src/objects/method.cpp src/objects/method.h src/objects/field.cpp src/objects/field.h src/objects/constant_pool.cpp src/objects/constant_pool.h src/native/jni.h src/native/jni.cpp src/native/jni_inner.h src/objects/array_object.cpp src/objects/array_object.h src/objects/string_object.cpp src/objects/string_object.h src/objects/signature.cpp src/objects/signature.h src/slot.h src/native/jtypes.h src/native/jni_interface.h src/arch/x86_32.cpp)

#target_link_libraries (kayo libz)
#target_link_libraries (kayo libminizip)
target_link_libraries(kayo zlibsrc)