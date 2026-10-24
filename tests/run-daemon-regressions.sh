#!/usr/bin/env bash
set -euo pipefail
repo=$(cd "$(dirname "$0")/.." && pwd)
test_dir=$(mktemp -d)
trap 'rm -rf -- "$test_dir"' EXIT
dir="$test_dir/module with spaces"
mkdir -p "$dir/.resource-cache"
touch "$dir/.resource-cache/stale-idmap"
block=$(sed -n '/^emptyDir=/,/^mount -o bind/p' "$repo/zygisk/module/daemon")
[[ -n "$block" ]]

# Exercise the production block without mounting anything on the host.
mount() {
    [[ "$#" == 4 && "$1" == -o && "$2" == bind &&
       "$3" == "$dir/.resource-cache" && "$4" == /data/resource-cache ]]
    [[ -d "$3" && -z "$(ls -A "$3")" ]]
}
eval "$block"
touch "$dir/.resource-cache/previous-start"
eval "$block"

# A failed isolation mount must prevent daemon startup.
mount() { return 1; }
if (eval "$block"; touch "$test_dir/launched"); then
    echo 'Failed mount allowed startup' >&2
    exit 1
fi
[[ ! -e "$test_dir/launched" ]]
echo 'Daemon resource-cache regressions passed'
