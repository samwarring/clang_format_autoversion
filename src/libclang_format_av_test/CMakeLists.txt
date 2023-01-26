find_package(Catch2 REQUIRED)
add_executable(libclang_format_av_test)
target_include_directories(libclang_format_av_test
                           PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}")
target_link_libraries(libclang_format_av_test PRIVATE libclang_format_av
                                                      Catch2::Catch2WithMain)
target_sources(libclang_format_av_test PRIVATE detect_version_test.cpp
                                               dump_config_13.cpp)
include(Catch)
catch_discover_tests(libclang_format_av_test)
