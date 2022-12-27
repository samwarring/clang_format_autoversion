# ~~~
# 3.7   - ExternalProject_Add(SOURCE_SUBDIR)
# 3.14  - ExternalProject_Add(LOG_OUTPUT_ON_FAILURE)
# 3.20  - ExternalProject_Add(CONFIGURE_HANDLED_BY_BUILD)
#       - install(PROGRAMS RENAME)
# 3.24  - ExternalProject_Add(DOWNLOAD_EXTRACT_TIMESTAMP)
# ~~~
cmake_minimum_required(VERSION 3.24)

project(clang_format_autoversion VERSION 0.1.0)
include(ExternalProject)
include(GNUInstallDirs)

if (WIN32)
    set(EXE ".exe")
endif ()

# Define how each clang-format version gets built.
add_custom_target(clang_0)
function (add_clang_format_version)
    set(OPTIONS)
    set(ONE_VALUE_ARGS
        MAJOR
        COMMIT
        DEPENDS_ON
        MD5)
    set(MULTIVALUE_ARGS)
    cmake_parse_arguments(
        arg
        "${OPTIONS}"
        "${ONE_VALUE_ARGS}"
        "${MULTIVALUE_ARGS}"
        ${ARGN})

    ExternalProject_Add(
        # Target name
        clang_${arg_MAJOR}
        # Download options
        DOWNLOAD_DIR "${CMAKE_CURRENT_SOURCE_DIR}/.downloads"
        URL https://github.com/llvm/llvm-project/archive/${arg_COMMIT}.zip
        DOWNLOAD_NAME "clang-${arg_MAJOR}-${arg_COMMIT}.zip"
        DOWNLOAD_EXTRACT_TIMESTAMP TRUE
        URL_MD5 ${arg_MD5}
        # Configure options
        SOURCE_SUBDIR llvm
        CONFIGURE_HANDLED_BY_BUILD TRUE
        CMAKE_GENERATOR Ninja
        CMAKE_CACHE_ARGS -DLLVM_ENABLE_PROJECTS:STRING=clang -DCMAKE_BUILD_TYPE:STRING=MinSizeRel
        # Build options
        BUILD_COMMAND ${CMAKE_COMMAND} --build <BINARY_DIR> --target clang-format
        # Install options
        INSTALL_COMMAND ""
        # Logging options
        LOG_DOWNLOAD TRUE
        LOG_BUILD TRUE
        LOG_CONFIGURE TRUE
        LOG_OUTPUT_ON_FAILURE TRUE)

    add_dependencies(clang_${arg_MAJOR} ${arg_DEPENDS_ON})
    ExternalProject_Get_Property(clang_${arg_MAJOR} BINARY_DIR)
    install(PROGRAMS "${BINARY_DIR}/bin/clang-format${EXE}" TYPE BIN
            RENAME clang-format-av-${arg_MAJOR}${EXE})
endfunction ()

# llvmorg-13.0.1
add_clang_format_version(
    MAJOR 13
    COMMIT 75e33f71c2dae584b13a7d1186ae0a038ba98838
    MD5 faea4392a33c11ddf4c170a746f0b22b
    DEPENDS_ON clang_0)
# llvmorg-14.0.6
add_clang_format_version(
    MAJOR 14
    COMMIT f28c006a5895fc0e329fe15fead81e37457cb1d1
    MD5 583b4463f0522c7d9439b06f0f2bb94f
    DEPENDS_ON clang_13)
# llvmorg-15.0.6
add_clang_format_version(
    MAJOR 15
    COMMIT 088f33605d8a61ff519c580a71b1dd57d16a03f8
    MD5 6b58de0ea7b3424ee087622b327d3836
    DEPENDS_ON clang_14)

# When the launcher cannot find a .clang-format file, it defaults to this version.
set(DEFAULT_CLANG_FORMAT_VERSION 15)

# Define the "launcher" application
set(SOURCES src/clang_format_av.cpp)
if (WIN32)
    list(APPEND SOURCES src/os_helpers_windows.cpp)
else ()
    list(APPEND SOURCES src/os_helpers_linux.cpp)
endif ()
add_executable(clang_format_av ${SOURCES})
set_target_properties(clang_format_av PROPERTIES OUTPUT_NAME "clang-format-av")
target_include_directories(clang_format_av PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/src")
target_compile_features(clang_format_av PRIVATE cxx_std_17)
target_compile_definitions(
    clang_format_av PRIVATE -DDEFAULT_CLANG_FORMAT_VERSION=\"${DEFAULT_CLANG_FORMAT_VERSION}\"
                            -DEXE_EXTENSION=\"${EXE}\")
install(TARGETS clang_format_av)

# Variables common to all CPack generators
set(CPACK_PACKAGE_NAME clang-format-autoversion)
set(CPACK_PACKAGE_VENDOR "Sam Warring")
set(CPACK_PACKAGE_DESCRIPTION
    "Distribution of multiple clang-format versions with a launcher to auto-select the correct version"
)
set(CMAKE_PROJECT_HOMEPAGE_URL "https://github.com/samwarring/clang_format_autoversion")
set(CPACK_PACKAGE_CONTACT "Sam Warring <samwarring@gmail.com>")

# Variables specific to CPack Debian generator
set(CPACK_DEBIAN_FILE_NAME DEB-DEFAULT)
set(CPACK_DEBIAN_PACKAGE_CONTROL_STRICT_PERMISSION TRUE)
set(CPACK_DEBIAN_PACKAGE_DEPENDS "libc6, libtinfo6, libstdc++6, libgcc-s1")

include(CPack)
