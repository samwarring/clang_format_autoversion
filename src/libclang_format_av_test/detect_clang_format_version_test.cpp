#include "detect_clang_format_version.hpp"
#include "dump_configs.hpp"
#include <catch2/catch_test_macros.hpp>

TEST_CASE("detect clang format version") {
  SECTION("explicit version in comment") {
    auto vers = detect_clang_format_version("# clang-format version 15");
    REQUIRE(vers.size() == 1);
    REQUIRE(vers[0] == 15);
  }
  SECTION("explicit https URL") {
    auto vers =
        detect_clang_format_version("# "
                                    "https://releases.llvm.org/14.0.0/tools/"
                                    "clang/docs/ClangFormatStyleOptions.html");
    REQUIRE(vers.size() == 1);
    REQUIRE(vers[0] == 14);
  }
  SECTION("explicit http URL") {
    auto vers =
        detect_clang_format_version("# "
                                    "http://releases.llvm.org/14.0.0/tools/"
                                    "clang/docs/ClangFormatStyleOptions.html");
    REQUIRE(vers.size() == 1);
    REQUIRE(vers[0] == 14);
  }
  SECTION("dump config 13") {
    auto vers = detect_clang_format_version(std::string(DUMP_CONFIG_13));
    REQUIRE(vers.size() > 0);
    REQUIRE(vers[0] == 13);
  }
}