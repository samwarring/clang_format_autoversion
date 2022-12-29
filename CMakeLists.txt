# ~~~
# 3.20  - install(PROGRAMS RENAME)
# ~~~
cmake_minimum_required(VERSION 3.20)

project(clang_format_autoversion VERSION 0.1.0)

set(DEFAULT_CLANG_FORMAT_VERSION
    15
    CACHE STRING "Default clang-format version when none specified in .clang-format file")
set(LLVM_MAJOR_VERSIONS
    13;14;15
    CACHE STRING "List of clang-format major versions to package into the installer")

if(WIN32)
    set(EXE ".exe")
endif()

# Define the "launcher" application
set(SOURCES src/clang_format_av.cpp)
if(WIN32)
    list(APPEND SOURCES src/os_helpers_windows.cpp)
else()
    list(APPEND SOURCES src/os_helpers_linux.cpp)
endif()
add_executable(clang_format_av ${SOURCES})
set_target_properties(clang_format_av PROPERTIES OUTPUT_NAME "clang-format-av")
target_include_directories(clang_format_av PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/src")
target_compile_features(clang_format_av PRIVATE cxx_std_17)
target_compile_definitions(
    clang_format_av PRIVATE -DDEFAULT_CLANG_FORMAT_VERSION=\"${DEFAULT_CLANG_FORMAT_VERSION}\"
                            -DEXE_EXTENSION=\"${EXE}\")
install(TARGETS clang_format_av)

# Locate the desired clang-format binaries and mark them for installation.
foreach(LLVM_MAJOR_VERSION IN LISTS LLVM_MAJOR_VERSIONS)
    include("${CMAKE_CURRENT_SOURCE_DIR}/llvm_versions/llvm_${LLVM_MAJOR_VERSION}.cmake" OPTIONAL
            RESULT_VARIABLE FOUND_VERSION)
    if(NOT FOUND_VERSION)
        message(FATAL_ERROR "Failed to load llvm_${LLVM_MAJOR_VERSION}.cmake")
    endif()
    set(CLANG_FORMAT_BINARY_NAME "clang-format-${LLVM_MAJOR_VERSION}-${COMMIT}${EXE}")
    set(CLANG_FORMAT_BINARY_PATH
        "${CMAKE_CURRENT_SOURCE_DIR}/.llvm_cache/bin/${CLANG_FORMAT_BINARY_NAME}")
    if(NOT EXISTS "${CLANG_FORMAT_BINARY_PATH}")
        message(FATAL_ERROR "Missing clang-format binary: ${CLANG_FORMAT_BINARY_PATH}")
    endif()
    message(STATUS "Adding to package: ${CLANG_FORMAT_BINARY_NAME} (${TAG})")
    install(
        PROGRAMS "${CLANG_FORMAT_BINARY_PATH}"
        TYPE BIN
        RENAME "clang-format-av-${LLVM_MAJOR_VERSION}${EXE}")
endforeach()

add_subdirectory(pkg)
