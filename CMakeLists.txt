cmake_minimum_required(VERSION 3.9)
project(kayo)

set(CMAKE_CXX_FLAGS "-std=c++17 -m64 -Wall -lz -lminizip -llibffi") # lz: link zlip

add_executable(kayo
        src/main.cpp
        src/interpreter/interpreter.cpp
        src/util/encoding.cpp src/util/convert.cpp src/classfile/attributes.cpp
        src/runtime/frame.cpp  src/runtime/thread_info.cpp src/runtime/Monitor.cpp
        src/memory/MemMgr.cpp src/memory/Heap.cpp
        src/native/java/io/FileDescriptor.cpp src/native/java/io/FileInputStream.cpp
        src/native/java/io/FileOutputStream.cpp src/native/java/lang/Class.cpp
        src/native/java/lang/Double.cpp src/native/java/lang/Float.cpp
        src/native/java/lang/Object.cpp src/native/java/lang/String.cpp
        src/native/java/lang/System.cpp src/native/java/lang/Thread.cpp
        src/native/java/lang/Throwable.cpp src/native/java/security/AccessController.cpp
        src/native/sun/misc/Unsafe.cpp src/native/sun/misc/VM.cpp
        src/native/sun/reflect/Reflection.cpp
        src/native/sun/reflect/NativeConstructorAccessorImpl.cpp
        src/native/sun/reflect/NativeMethodAccessorImpl.cpp
        src/native/sun/reflect/ConstantPool.cpp
        src/native/java/util/concurrent/atomic/AtomicLong.cpp src/native/java/io/WinNTFileSystem.cpp
        src/native/java/lang/ClassLoader.cpp src/native/java/lang/ClassLoader-NativeLibrary.cpp
        src/native/sun/misc/Signal.cpp src/native/sun/io/Win32ErrorMode.cpp
        src/native/java/lang/Runtime.cpp src/native/sun/misc/Version.cpp
        src/native/java/lang/reflect/Field.cpp src/native/java/lang/reflect/Executable.cpp
        src/native/java/nio/Bits.cpp src/symbol.cpp src/gc/gc.cpp
        src/native/sun/misc/URLClassPath.cpp src/native/java/util/zip/ZipFile.cpp
        src/native/sun/misc/Perf.cpp src/native/java/lang/Package.cpp
        src/native/java/io/RandomAccessFile.cpp src/native/java/lang/invoke/MethodHandleNatives.cpp
        src/native/java/lang/reflect/Array.cpp src/native/java/lang/reflect/Proxy.cpp
        src/native/java/lang/invoke/MethodHandle.cpp
        src/native/sun/management/VMManagementImpl.cpp src/native/sun/management/ThreadImpl.cpp
        src/native/jdk/internal/misc/VM.cpp
        src/native/jdk/internal/misc/Unsafe.cpp src/native/jdk/internal/util/SystemProps-Raw.cpp
        src/objects/method.cpp src/objects/field.cpp src/objects/constant_pool.cpp src/native/jni.cpp
        src/objects/array_object.cpp src/objects/string_object.cpp src/objects/throwables.cpp
        src/objects/class_loader.cpp src/objects/prims.cpp src/objects/invoke.cpp
        src/objects/object.cpp src/objects/class.cpp src/interpreter/instructions/invokedynamic.cpp)

target_link_libraries (kayo libz)
target_link_libraries (kayo libminizip)
target_link_libraries (kayo libffi)