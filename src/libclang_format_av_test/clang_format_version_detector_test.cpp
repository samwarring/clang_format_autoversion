#include "clang_format_version_detector.hpp"
#include <catch2/catch_test_macros.hpp>

TEST_CASE("clang format version detector") {
  clang_format_version_detector det;

  SECTION("read configuration") {

    SECTION("explicit version in comment") {
      det.read_configuration("# clang-format version 15");
      REQUIRE(det.detected_version() == 15);
    }

    SECTION("explicit https URL") {
      det.read_configuration("# "
                             "https://releases.llvm.org/14.0.0/tools/clang/"
                             "docs/ClangFormatStyleOptions.html");
      REQUIRE(det.detected_version() == 14);
    }

    SECTION("explicit http URL") {
      det.read_configuration("# "
                             "http://releases.llvm.org/14.0.0/tools/clang/"
                             "docs/ClangFormatStyleOptions.html");
      REQUIRE(det.detected_version() == 14);
    }
  }
}