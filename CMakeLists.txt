# 3.7   - ExternalProject_Add(SOURCE_SUBDIR)
# 3.14  - ExternalProject_Add(LOG_OUTPUT_ON_FAILURE)
# 3.20  - install(PROGRAMS RENAME)
cmake_minimum_required(VERSION 3.20)

project(clang_format_autoversion)

set(LLVM_REPO_URL "https://github.com/llvm/llvm-project" CACHE STRING "URL or path to LLVM git repository")

include(ExternalProject)
include(GNUInstallDirs)

function(add_clang_format_version MAJOR COMMIT)
    ExternalProject_Add(
        clang_${MAJOR}
        GIT_REPOSITORY "${LLVM_REPO_URL}"
        GIT_TAG ${COMMIT}
        GIT_SHALLOW TRUE
        GIT_PROGRESS TRUE
        SOURCE_SUBDIR llvm
        CMAKE_GENERATOR Ninja
        CMAKE_CACHE_ARGS -DLLVM_ENABLE_PROJECTS:STRING=clang -DCMAKE_BUILD_TYPE:STRING=MinSizeRel
        BUILD_COMMAND ${CMAKE_COMMAND} --build <BINARY_DIR> --target clang-format
        INSTALL_COMMAND ""
        LOG_CONFIGURE TRUE
        LOG_OUTPUT_ON_FAILURE TRUE
    )
    ExternalProject_Get_Property(clang_${MAJOR} BINARY_DIR)
    install(PROGRAMS "${BINARY_DIR}/bin/clang-format" TYPE BIN RENAME clang-format-${MAJOR})
endfunction()

add_clang_format_version(13 75e33f71c2dae584b13a7d1186ae0a038ba98838) # llvmorg-13.0.1
add_clang_format_version(14 f28c006a5895fc0e329fe15fead81e37457cb1d1) # llvmorg-14.0.6
add_clang_format_version(15 088f33605d8a61ff519c580a71b1dd57d16a03f8) # llvmorg-15.0.6