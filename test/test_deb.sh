#!/bin/sh -e

if [ "$1" = "" ]; then
    echo "Usage: test_deb.sh <path to .deb file>"
    exit 1
fi

deb_path=$(realpath "$1")
set -x
apt-get install "$deb_path"

# Check that clang-format-av can be run
clang-format-av --version

# Test running clang-format-av for different clang-format versions
set +x
test_dir=$(realpath $(dirname $0))
set -x
(cd "$test_dir/example_13" && clang-format-av --version) | grep "clang-format version 13"
(cd "$test_dir/example_14" && clang-format-av --version) | grep "clang-format version 14"
(cd "$test_dir/example_15" && clang-format-av --version) | grep "clang-format version 15"

set +x
echo "All tests passed!"