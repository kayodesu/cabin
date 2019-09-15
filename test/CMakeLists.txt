
add_executable(test_heapmgr test_heapmgr.cpp)
add_executable(test_exception test_exception.cpp)
add_executable(test_array test_array.cpp)
add_executable(test_class_loader test_class_loader.cpp)
add_executable(test_tos_cache test_tos_cache.cpp)

target_link_libraries(test_heapmgr vmlib)
target_link_libraries(test_exception vmlib)
target_link_libraries(test_array vmlib)
target_link_libraries(test_class_loader vmlib)