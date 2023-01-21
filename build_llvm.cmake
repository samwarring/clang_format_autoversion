cmake_minimum_required(VERSION 3.19)

if(NOT LLVM_MAJOR_VERSION)
    message(
        FATAL_ERROR
            "Invalid command line!\n"
            "Usage: cmake [parameters] -P build_llvm.cmake\n"
            "  Parameters:\n"
            "    -DLLVM_MAJOR_VERSION=<ver>   (Required)\n"
            "    -DREBUILD=<bool>             (Optional)\n"
            "    -DPREFIX_DIR=<dir>           (Optional)\n"
            "    -DTARGET_ARCH=<x64|arm64>    (Optional)"
            )
endif()

# Load variables from llvm_X.cmake file
set(LLVM_VARS_FILE "${CMAKE_CURRENT_LIST_DIR}/llvm_versions/llvm_${LLVM_MAJOR_VERSION}.cmake")
include("${LLVM_VARS_FILE}" OPTIONAL RESULT_VARIABLE FOUND_VARS_FILE)
if(NOT FOUND_VARS_FILE)
    message(FATAL_ERROR "File not found: ${LLVM_VARS_FILE}")
endif()

# Load target architecture
if (NOT TARGET_ARCH)
    set(TARGET_ARCH "x64")
endif()
if (NOT TARGET_ARCH STREQUAL "x64")
    if (NOT TARGET_ARCH STREQUAL "arm64")
        message(FATAL_ERROR "Invalid TARGET_ARCH '${TARGET_ARCH}' (should be 'x64' or 'arm64')")
    endif()
endif()

# Welcome banner
message("--------------------------------------------------")
message("                build_llvm.cmake                  ")
message("--------------------------------------------------")
message("Major Ver:    ${LLVM_MAJOR_VERSION}")
message("Tag:          ${TAG}")
message("Commit:       ${COMMIT}")
message("Architecture: ${TARGET_ARCH}")
message("--------------------------------------------------")

# Get paths to directories
if(WIN32)
    set(EXE .exe)
endif()
if(NOT PREFIX_DIR)
    set(PREFIX_DIR "${CMAKE_CURRENT_LIST_DIR}/.llvm_cache/${TAG}-${TARGET_ARCH}")
endif()
cmake_path(ABSOLUTE_PATH PREFIX_DIR NORMALIZE OUTPUT_VARIABLE PREFIX_DIR)
set(ARCHIVE_FILE "${PREFIX_DIR}/src.zip")
set(SOURCE_DIR "${PREFIX_DIR}/src")
set(BINARY_DIR "${PREFIX_DIR}/build")
set(OUTPUT_DIR "${CMAKE_CURRENT_LIST_DIR}/.llvm_cache/bin/")
set(OUTPUT_FILE "${OUTPUT_DIR}/clang-format-${MAJOR}-${TARGET_ARCH}-${COMMIT}${EXE}")

# If rebuilding, nuke old directories.
if(REBUILD)
    message("Deleting ${PREFIX_DIR} ...")
    file(REMOVE_RECURSE "${PREFIX_DIR}")
endif()

# Download the source archive.
set(ARCHIVE_URL "https://github.com/llvm/llvm-project/archive/${COMMIT}.zip")
message("Downloading ${ARCHIVE_URL} ...")
file(
    DOWNLOAD "${ARCHIVE_URL}" "${ARCHIVE_FILE}"
    EXPECTED_MD5 "${MD5}"
    SHOW_PROGRESS
    STATUS DOWNLOAD_RESULT)
list(POP_FRONT DOWNLOAD_RESULT DOWNLOAD_ERROR_CODE)
list(POP_FRONT DOWNLOAD_RESULT DOWNLOAD_ERROR_MESSAGE)
if(NOT "${DOWNLOAD_ERROR_CODE}" STREQUAL "0")
    message(FATAL_ERROR "Download failed with error:\n${DOWNLOAD_ERROR_MESSAGE}")
endif()

# Extract the archive.
if (NOT EXISTS "${SOURCE_DIR}")
    message("Extracting to ${SOURCE_DIR} ...")
    file(ARCHIVE_EXTRACT INPUT "${ARCHIVE_FILE}" DESTINATION "${PREFIX_DIR}")
    file(RENAME "${PREFIX_DIR}/llvm-project-${COMMIT}" "${SOURCE_DIR}")
endif()

# Configure the project.
# cmake-format: off
message("Configuring build system in ${BINARY_DIR}")
file(MAKE_DIRECTORY "${BINARY_DIR}")
if(WIN32)
    set(CONFIGURE_ARGS -DCMAKE_C_COMPILER=cl -DCMAKE_CXX_COMPILER=cl)
elseif(TARGET_ARCH STREQUAL "arm64")
    # Before cross-compiling, first need to build some tools for host arch (x64)
    # because they will be invoked as part of the build.
    file(MAKE_DIRECTORY "${BINARY_DIR}-native")
    execute_process(
        COMMAND "${CMAKE_COMMAND}" -G Ninja -S "${SOURCE_DIR}/llvm" -B "${BINARY_DIR}-native"
                -DLLVM_ENABLE_PROJECTS=clang -DCMAKE_BUILD_TYPE=Release
        COMMAND_ERROR_IS_FATAL ANY)
    execute_process(
        COMMAND "${CMAKE_COMMAND}" --build "${BINARY_DIR}-native" --target llvm-tblgen clang-tblgen
        COMMAND_ERROR_IS_FATAL ANY)
    
    # Pass these flags when compiling for the target arch (arm64)
    set(CONFIGURE_ARGS
        -DCMAKE_C_COMPILER=aarch64-linux-gnu-gcc
        -DCMAKE_CXX_COMPILER=aarch64-linux-gnu-g++
        -DLLVM_TABLEGEN=${BINARY_DIR}-native/bin/llvm-tblgen
        -DCLANG_TABLEGEN=${BINARY_DIR}-native/bin/clang-tblgen)
endif()
execute_process(
    COMMAND
        "${CMAKE_COMMAND}" -G Ninja -S "${SOURCE_DIR}/llvm" -B "${BINARY_DIR}"
        -DLLVM_ENABLE_PROJECTS=clang -DCMAKE_BUILD_TYPE=MinSizeRel ${CONFIGURE_ARGS}
    COMMAND_ERROR_IS_FATAL ANY)
# cmake-format: on

# Build the project.
message("Building ${TAG} ...")
# cmake-format: off
execute_process(COMMAND "${CMAKE_COMMAND}" --build "${BINARY_DIR}" --target clang-format
                COMMAND_ERROR_IS_FATAL ANY)
# cmake-format: on

# Copy output binary to the cache
message("Copying output to ${OUTPUT_FILE}")
file(MAKE_DIRECTORY "${OUTPUT_DIR}")
file(COPY_FILE "${BINARY_DIR}/bin/clang-format${EXE}" "${OUTPUT_FILE}")
