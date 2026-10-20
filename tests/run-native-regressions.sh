#!/usr/bin/env bash
set -euo pipefail
repo=$(cd "$(dirname "$0")/.." && pwd)
test_dir=$(mktemp -d)
trap 'rm -rf -- "$test_dir"' EXIT
compiler=${CXX:-clang++}
flags=(-std=c++20 -O1 -g -Wall -Wextra -Werror -fsanitize=address,undefined -fno-omit-frame-pointer)
"$compiler" "${flags[@]}" -DNDEBUG -DFMT_HEADER_ONLY \
    -I"$repo/tests/native/include" -I"$repo/native/include" -I"$repo/external/fmt/include" \
    "$repo/tests/native/logging_test.cpp" -o "$test_dir/logging_test"
"$test_dir/logging_test"
"$compiler" "${flags[@]}" -I"$repo/zygisk/src/main/cpp/include" \
    "$repo/tests/native/trace_marker_fds_test.cpp" -o "$test_dir/trace_marker_fds_test"
"$test_dir/trace_marker_fds_test"
