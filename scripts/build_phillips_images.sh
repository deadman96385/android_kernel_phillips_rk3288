#!/usr/bin/env bash
set -euo pipefail

script_dir=$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)
repo_root=$(cd "$script_dir/.." && pwd)

arch=arm
board_target="rk3288-phillips-rk808.img"
board_dtb="rk3288-phillips-rk808.dtb"
defconfig="rockchip_phillips_defconfig"
output_dir="${OUTPUT_DIR:-$repo_root/out/phillips}"
jobs="${JOBS:-$(nproc)}"
cross_compile="${CROSS_COMPILE:-}"
toolchain_bin=""

usage() {
	cat <<EOF
Usage: $(basename "$0") [--out-dir DIR] [--cross-compile PREFIX_OR_DIR] [--jobs N|--workers N] [--no-defconfig]

Builds Rockchip images for the Phillips RK3288 device:
  - kernel.img
  - resource.img
  - boot.img
  - zboot.img

Environment overrides:
  OUTPUT_DIR
  CROSS_COMPILE
  JOBS
EOF
}

run_defconfig=1

while [[ $# -gt 0 ]]; do
	case "$1" in
		--out-dir)
			output_dir="$2"
			shift 2
			;;
		--cross-compile)
			cross_compile="$2"
			shift 2
			;;
		--jobs)
			jobs="$2"
			shift 2
			;;
		--workers)
			jobs="$2"
			shift 2
			;;
		--no-defconfig)
			run_defconfig=0
			shift
			;;
		-h|--help)
			usage
			exit 0
			;;
		*)
			echo "unknown argument: $1" >&2
			usage >&2
			exit 2
			;;
	esac
done

mkdir -p "$output_dir"

if [[ -n "$cross_compile" && -d "$cross_compile" ]]; then
	toolchain_bin=$(readlink -f "$cross_compile")
	gcc_candidates=("$toolchain_bin"/*gcc)
	if [[ ! -e "${gcc_candidates[0]}" ]]; then
		echo "no *gcc binary found in toolchain directory: $toolchain_bin" >&2
		exit 1
	fi
	gcc_name=$(basename "${gcc_candidates[0]}")
	cross_compile="${gcc_name%gcc}"
fi

make_args=(
	-C "$repo_root"
	"O=$output_dir"
	"ARCH=$arch"
)

if [[ -n "$cross_compile" ]]; then
	if [[ -n "$toolchain_bin" ]]; then
		export PATH="$toolchain_bin:$PATH"
	fi
	make_args+=("CROSS_COMPILE=$cross_compile")
fi

if [[ "$run_defconfig" -eq 1 ]]; then
	echo "==> Configuring $defconfig"
	make "${make_args[@]}" "$defconfig"
elif [[ ! -f "$output_dir/.config" ]]; then
	echo "missing $output_dir/.config; rerun without --no-defconfig or create a config first" >&2
	exit 1
fi

echo "==> Building kernel.img"
make "${make_args[@]}" -j"$jobs" Image zImage "$board_dtb"

echo "==> Packing kernel.img"
"$repo_root/scripts/mkkrnlimg" \
	"$output_dir/arch/arm/boot/zImage" \
	"$output_dir/kernel.img" >/dev/null

logo_args=()
for logo in logo.bmp logo_kernel.bmp; do
	if [[ -f "$repo_root/$logo" ]]; then
		cp -a "$repo_root/$logo" "$output_dir/"
		logo_args+=("$logo")
	fi
done

echo "==> Packing resource.img"
(
	cd "$output_dir"
	"$repo_root/scripts/resource_tool" \
		"$output_dir/arch/arm/boot/dts/$board_dtb" \
		"${logo_args[@]}"
)

echo "==> Packing boot.img and zboot.img"
"$repo_root/scripts/mkbootimg" \
	--kernel "$output_dir/arch/arm/boot/Image" \
	--second "$output_dir/resource.img" \
	-o "$output_dir/boot.img"

"$repo_root/scripts/mkbootimg" \
	--kernel "$output_dir/arch/arm/boot/zImage" \
	--second "$output_dir/resource.img" \
	-o "$output_dir/zboot.img"

echo "==> Build complete"
for artifact in kernel.img resource.img boot.img zboot.img; do
	if [[ -f "$output_dir/$artifact" ]]; then
		printf '  %s\n' "$output_dir/$artifact"
	fi
done
