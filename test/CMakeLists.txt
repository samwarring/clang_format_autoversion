include(FetchContent)
FetchContent_Declare(
  Catch2
  GIT_REPOSITORY https://github.com/catchorg/Catch2.git
  GIT_TAG        c837cb4a8aaa2064f63e67ed0a7f7055dbf159c5 # v3.3.0
)
FetchContent_MakeAvailable(Catch2)

set(APP_SOURCES
    "${PROJECT_SOURCE_DIR}/src/detect_version.cpp")
set(TEST_SOURCES
    detect_version_test.cpp)

add_executable(clang_format_av_test ${APP_SOURCES} ${TEST_SOURCES})
target_include_directories(clang_format_av_test PRIVATE "${PROJECT_SOURCE_DIR}/src")
target_link_libraries(clang_format_av_test PRIVATE Catch2::Catch2WithMain)
target_compile_features(clang_format_av_test PRIVATE cxx_std_17)

include(Catch)
catch_discover_tests(clang_format_av_test)