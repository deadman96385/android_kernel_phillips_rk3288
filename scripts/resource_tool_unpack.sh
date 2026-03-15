#!/usr/bin/env bash
set -euo pipefail

if [[ $# -lt 1 || $# -gt 2 ]]; then
	echo "usage: $0 <resource.img> [output_dir]" >&2
	exit 2
fi

script_dir=$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)
tool="$script_dir/resource_tool"
input_img=$(readlink -f "$1")
output_dir=${2:-out}

if [[ ! -f "$input_img" ]]; then
	echo "input image not found: $input_img" >&2
	exit 1
fi

workdir=$(mktemp -d)
cleanup() {
	rm -rf "$workdir"
}
trap cleanup EXIT

mkdir -p "$workdir/out"
cp "$input_img" "$workdir/resource.img"

(
	cd "$workdir"
	"$tool" --unpack >"$workdir/tool.log" 2>&1 &
	pid=$!

	# The vendor tool hardcodes "resource.img" and later tries to create
	# directories under the same path. Rename the input after it is opened.
	for _ in $(seq 1 500); do
		if ls -l "/proc/$pid/fd" 2>/dev/null | grep -Fq "$workdir/resource.img"; then
			mv "$workdir/resource.img" "$workdir/resource.img.src"
			break
		fi
		sleep 0.01
	done

	wait "$pid"
)

mkdir -p "$output_dir"
find "$workdir/out" -mindepth 1 -maxdepth 1 -type f -exec mv {} "$output_dir"/ \;

echo "unpacked to: $output_dir"
