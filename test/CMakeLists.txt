
add_executable(test_heapmgr test_heapmgr.cpp)
add_executable(test_exception test_exception.cpp)
add_executable(test_array test_array.cpp)

target_link_libraries(test_heapmgr vmlib)
target_link_libraries(test_exception vmlib)
target_link_libraries(test_array vmlib)