# 3.7   - ExternalProject_Add(SOURCE_SUBDIR)
# 3.14  - ExternalProject_Add(LOG_OUTPUT_ON_FAILURE)
# 3.20  - ExternalProject_Add(CONFIGURE_HANDLED_BY_BUILD)
#       - install(PROGRAMS RENAME)
cmake_minimum_required(VERSION 3.20)

project(clang_format_autoversion VERSION 0.1.0)
include(ExternalProject)
include(GNUInstallDirs)

# This can be overridden to a local path to dramatically speed up the clone step.
set(LLVM_REPO_URL "https://github.com/llvm/llvm-project" CACHE STRING "URL or path to LLVM git repository")

# Define how each clang-format version gets built.
macro(add_clang_format_version MAJOR COMMIT)
    ExternalProject_Add(
        clang_${MAJOR}
        GIT_REPOSITORY "${LLVM_REPO_URL}"
        GIT_TAG ${COMMIT}
        GIT_SHALLOW TRUE
        GIT_PROGRESS TRUE
        SOURCE_SUBDIR llvm
        CONFIGURE_HANDLED_BY_BUILD TRUE
        CMAKE_GENERATOR Ninja
        CMAKE_CACHE_ARGS -DLLVM_ENABLE_PROJECTS:STRING=clang -DCMAKE_BUILD_TYPE:STRING=MinSizeRel
        BUILD_COMMAND ${CMAKE_COMMAND} --build <BINARY_DIR> --target clang-format
        INSTALL_COMMAND ""
        LOG_CONFIGURE TRUE
        LOG_OUTPUT_ON_FAILURE TRUE
    )
    ExternalProject_Get_Property(clang_${MAJOR} BINARY_DIR)
    install(PROGRAMS "${BINARY_DIR}/bin/clang-format" TYPE BIN RENAME clang-format-av-${MAJOR})
endmacro()

# Define which clang-format versions get built.
add_clang_format_version(13 75e33f71c2dae584b13a7d1186ae0a038ba98838) # llvmorg-13.0.1
add_clang_format_version(14 f28c006a5895fc0e329fe15fead81e37457cb1d1) # llvmorg-14.0.6
add_clang_format_version(15 088f33605d8a61ff519c580a71b1dd57d16a03f8) # llvmorg-15.0.6

# When the launcher cannot find a .clang-format file, it defaults to this version.
set(DEFAULT_CLANG_FORMAT_VERSION 15)

# Define the "launcher" application
find_package(Boost REQUIRED)
find_package(Threads REQUIRED)
add_executable(clang_format_av clang_format_av.cpp)
set_target_properties(clang_format_av PROPERTIES OUTPUT_NAME "clang-format-av")
target_link_libraries(clang_format_av PRIVATE Boost::boost Threads::Threads)
target_compile_features(clang_format_av PRIVATE cxx_std_17)
target_compile_definitions(clang_format_av PRIVATE -DDEFAULT_CLANG_FORMAT_VERSION=\"${DEFAULT_CLANG_FORMAT_VERSION}\")
install(TARGETS clang_format_av)

# Variables common to all CPack generators
set(CPACK_PACKAGE_NAME clang-format-autoversion)
set(CPACK_PACKAGE_VENDOR "Sam Warring")
set(CPACK_PACKAGE_DESCRIPTION "Distribution of multiple clang-format versions with a launcher to auto-select the correct version")
set(CMAKE_PROJECT_HOMEPAGE_URL "https://github.com/samwarring/clang_format_autoversion")
set(CPACK_PACKAGE_CONTACT "Sam Warring <samwarring@gmail.com>")

# Variables specific to CPack Debian generator
set(CPACK_DEBIAN_FILE_NAME DEB-DEFAULT)
set(CPACK_DEBIAN_PACKAGE_CONTROL_STRICT_PERMISSION TRUE)
set(CPACK_DEBIAN_PACKAGE_DEPENDS "libc6, libtinfo6, libstdc++6, libgcc-s1")

include(CPack)