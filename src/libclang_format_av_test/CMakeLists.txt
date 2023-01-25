find_package(Catch2 REQUIRED)
add_executable(libclang_format_av_test)
target_link_libraries(libclang_format_av_test PRIVATE libclang_format_av
                                                      Catch2::Catch2WithMain)
target_sources(libclang_format_av_test
               PRIVATE detect_clang_format_version_test.cpp)
include(Catch)
catch_discover_tests(libclang_format_av_test)
