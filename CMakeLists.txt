cmake_minimum_required(VERSION 3.9)
project(kayo)

#set(CMAKE_C_FLAGS "-Wall -m32 -std=c11")
#set(CMAKE_CXX_FLAGS "-Wall -pthread -lz -lminizip -m32 -std=c++17")

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "-Wall -pthread -lz -lminizip -llibffi -m64") # lz: link zlip

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_FLAGS "-Wall -pthread -lz -lminizip -llibffi -m64")

#add_subdirectory(zlib)


add_executable(kayo src/kayo.h src/classfile/constants.h src/vmdef.h src/objects/object.cpp src/objects/prims.h src/objects/object.h src/util/bytecode_reader.h src/util/convert.c src/util/convert.h src/classfile/attributes.cpp src/classfile/attributes.h src/main.cpp src/runtime/frame.cpp src/runtime/frame.h src/objects/class.cpp src/objects/class.h src/runtime/thread_info.cpp src/runtime/thread_info.h src/native/java/io/FileDescriptor.c src/native/java/io/FileInputStream.c src/native/java/io/FileOutputStream.c src/native/java/lang/Class.cpp src/native/java/lang/Double.c src/native/java/lang/Float.c src/native/java/lang/Object.c src/native/java/lang/String.c src/native/java/lang/System.cpp src/native/java/lang/Thread.cpp src/native/java/lang/Throwable.cpp src/native/java/security/AccessController.c src/native/sun/misc/Unsafe.cpp src/native/sun/misc/VM.c src/native/sun/reflect/Reflection.cpp src/interpreter/interpreter.cpp src/interpreter/interpreter.h src/native/sun/reflect/NativeConstructorAccessorImpl.cpp src/native/sun/reflect/NativeMethodAccessorImpl.c src/native/sun/reflect/ConstantPool.cpp src/util/endianness.h src/native/java/util/concurrent/atomic/AtomicLong.c src/native/java/io/WinNTFileSystem.cpp src/native/java/lang/ClassLoader.c src/native/java/lang/ClassLoader-NativeLibrary.c src/native/sun/misc/Signal.c src/native/sun/io/Win32ErrorMode.c src/native/java/lang/Runtime.c src/native/sun/misc/Version.c src/native/java/lang/reflect/Field.c src/native/java/lang/reflect/Executable.c src/native/java/nio/Bits.c src/memory/Heap.h src/symbol.cpp src/symbol.h src/config.h src/gc/gc.cpp src/gc/gc.h src/debug.h src/objects/throwables.cpp src/objects/throwables.h src/objects/class_loader.cpp src/objects/class_loader.h src/native/sun/misc/URLClassPath.c src/native/java/util/zip/ZipFile.c src/util/encoding.cpp src/util/encoding.h src/native/sun/misc/Perf.c src/native/java/lang/Package.c src/native/java/io/RandomAccessFile.c src/native/java/lang/invoke/MethodHandleNatives.cpp src/native/java/lang/reflect/Array.c src/native/java/lang/reflect/Proxy.c src/memory/MemMgr.cpp src/memory/MemMgr.h src/memory/Heap.cpp src/native/java/lang/invoke/MethodHandle.cpp src/objects/prims.cpp src/objects/prims.h src/objects/invoke.cpp src/objects/invoke.h src/objects/Modifier.h src/native/sun/management/VMManagementImpl.cpp src/native/sun/management/ThreadImpl.cpp src/runtime/Monitor.cpp src/runtime/Monitor.h src/native/jdk/internal/misc/VM.c src/native/jdk/internal/misc/Unsafe.cpp src/native/jdk/internal/util/SystemProps-Raw.cpp src/objects/method.cpp src/objects/method.h src/objects/field.cpp src/objects/field.h src/objects/constant_pool.cpp src/objects/constant_pool.h src/native/jni.h src/native/jni.cpp src/native/jni_inner.h src/objects/array_object.cpp src/objects/array_object.h src/objects/string_object.cpp src/objects/string_object.h src/objects/signature.cpp src/objects/signature.h src/slot.h src/native/jtypes.h src/native/jni_interface.h src/native/jnidef.h)

target_link_libraries (kayo libz)
target_link_libraries (kayo libminizip)
target_link_libraries (kayo libffi)
#target_link_libraries(kayo zlibsrc)