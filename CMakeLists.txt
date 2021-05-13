cmake_minimum_required(VERSION 3.9)
project(cabin)

set(CMAKE_C_FLAGS "-std=c11 -m64 -Wall -lz -lminizip -llibffi") # lz: link zlip

# Windows 下生成 jvm.dll
add_library(jvm SHARED  src/init.c src/jvm.c src/jni.c src/natives.c
                src/interpreter.c src/descriptor.c
                src/util/encoding.c src/util/convert.c src/attributes.c
                src/frame.c src/thread.c
                src/heap.c src/gc.c src/util/hash.c src/dll.c
                src/symbol.c src/os/win.c src/os/linux.c
                src/method.c src/field.c src/constant_pool.c src/util/dynstr.c
                src/class_loader.c src/prims.c src/mh.c
                src/object.c src/class.c src/classpath.c src/exception.c
                src/bytecode_reader.c)

SET_TARGET_PROPERTIES(jvm PROPERTIES OUTPUT_NAME "jvm" PREFIX "")

target_link_libraries(jvm libz)
target_link_libraries(jvm libminizip)
target_link_libraries(jvm libffi)

# 生成用于测试的可执行文件
add_executable(cabin src/cabin.c)
target_link_libraries(cabin jvm)