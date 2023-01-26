find_package(ctre REQUIRED)
find_package(ryml REQUIRED)
add_library(libclang_format_av)
set_target_properties(libclang_format_av PROPERTIES OUTPUT_NAME clang_format_av)
target_compile_features(libclang_format_av PUBLIC cxx_std_20)
target_include_directories(libclang_format_av PUBLIC include)
target_link_libraries(libclang_format_av PRIVATE ctre::ctre ryml::ryml)
target_sources(
    libclang_format_av
    PRIVATE detect_clang_format_version.cpp detect_version_from_yaml.cpp
            detect_version_from_comments.cpp)
