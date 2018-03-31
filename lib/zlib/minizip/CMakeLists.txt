

ADD_DEFINITIONS(-DIOAPI_NO_64)

add_library(libz0 STATIC IMPORTED)
set_target_properties(libz0 PROPERTIES IMPORTED_LOCATION ${CMAKE_CURRENT_SOURCE_DIR}/lib/zlib/libz0.a)

set(SOURCE_FILES crypt.h ioapi.c ioapi.h iowin32.c iowin32.h mztools.c mztools.h unzip.c unzip.h zip.c zip.h)
add_library(minizip ${SOURCE_FILES})
target_link_libraries(minizip libz0)
