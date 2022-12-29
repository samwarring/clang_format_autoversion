cmake_minimum_required(VERSION 3.19)

if(NOT LLVM_MAJOR_VERSION)
    message(
        FATAL_ERROR
            "Invalid command line!\n"
            "Usage: cmake [parameters] -P build_llvm.cmake\n"
            "  Parameters:\n"
            "    -DLLVM_MAJOR_VERSION=<ver>   (Required)\n"
            "    -DREBUILD=<bool>             (Optional)\n"
            "    -DPREFIX_DIR=<dir>           (Optional)")
endif()

# Load variables from llvm_X.cmake file
set(LLVM_VARS_FILE "${CMAKE_CURRENT_LIST_DIR}/llvm_versions/llvm_${LLVM_MAJOR_VERSION}.cmake")
include("${LLVM_VARS_FILE}" OPTIONAL RESULT_VARIABLE FOUND_VARS_FILE)
if(NOT FOUND_VARS_FILE)
    message(FATAL_ERROR "File not found: ${LLVM_VARS_FILE}")
endif()

# Confirm version being built.
message("Tag: ${TAG}")

# Get paths to directories
if(NOT PREFIX_DIR)
    set(PREFIX_DIR "${CMAKE_CURRENT_LIST_DIR}/.llvm_cache/${TAG}")
endif()
if(WIN32)
    set(EXE .exe)
endif()
cmake_path(ABSOLUTE_PATH PREFIX_DIR NORMALIZE OUTPUT_VARIABLE PREFIX_DIR)
set(ARCHIVE_FILE "${PREFIX_DIR}/src.zip")
set(SOURCE_DIR "${PREFIX_DIR}/src")
set(BINARY_DIR "${PREFIX_DIR}/build")
set(OUTPUT_DIR "${CMAKE_CURRENT_LIST_DIR}/.llvm_cache/bin/")
set(OUTPUT_FILE "${OUTPUT_DIR}/clang-format-${MAJOR}-${COMMIT}${EXE}")

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
message("Extracting to ${SOURCE_DIR} ...")
file(ARCHIVE_EXTRACT INPUT "${ARCHIVE_FILE}" DESTINATION "${PREFIX_DIR}")
file(RENAME "${PREFIX_DIR}/llvm-project-${COMMIT}" "${SOURCE_DIR}")

# Configure the project.
message("Configuring build system in ${BINARY_DIR}")
file(MAKE_DIRECTORY "${BINARY_DIR}")
if(WIN32)
    set(COMPILER_ARGS -DCMAKE_C_COMPILER=cl -DCMAKE_CXX_COMPILER=cl)
endif()
# cmake-format: off
execute_process(
    COMMAND
        "${CMAKE_COMMAND}" -G Ninja -S "${SOURCE_DIR}/llvm" -B "${BINARY_DIR}"
        -DLLVM_ENABLE_PROJECTS=clang -DCMAKE_BUILD_TYPE=MinSizeRel ${COMPILER_ARGS}
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
