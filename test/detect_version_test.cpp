#include <catch2/catch_test_macros.hpp>
#include "detect_version.hpp"
#include <sstream>

int detect_version_string(const std::string& s) {
    std::istringstream sin(s);
    return detect_version(sin);
}

TEST_CASE("Detect version") {

    SECTION("Emtpy file") {
        REQUIRE(detect_version_string("") == -1);
    }

    SECTION("Explicit version string") {
        REQUIRE(detect_version_string("# clang-format version 15") == 15);
        REQUIRE(detect_version_string("   # clang-format version 15") == 15);
        REQUIRE(detect_version_string("   # clang-format version 15.0.0") == 15);
    }

    SECTION("Contains URL") {
        REQUIRE(detect_version_string("# https://releases.llvm.org/15.0.0/tools/clang/docs/ClangFormatStyleOptions.html") == 15);
    }
}