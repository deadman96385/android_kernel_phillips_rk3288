# Phillips RK3288 Build Notes

This tree can build Rockchip boot images for the Phillips RK3288 device defined by:

- defconfig: `rockchip_phillips_defconfig`
- board DTS target: `rk3288-phillips-rk808.img`

## Build

Use the helper script:

```bash
scripts/build_phillips_images.sh --cross-compile arm-linux-gnueabihf-
```

You can also point `--cross-compile` at a toolchain `bin` directory. The script will detect the `*gcc` binary there, prepend that directory to `PATH`, and derive the correct prefix automatically.

Example:

```bash
scripts/build_phillips_images.sh --cross-compile /opt/gcc-linaro/bin
```

Optional flags:

- `--out-dir DIR`: build output directory
- `--cross-compile PREFIX_OR_DIR`: toolchain prefix or toolchain `bin` directory
- `--jobs N`: parallel make jobs
- `--workers N`: alias for `--jobs`
- `--no-defconfig`: reuse the existing `.config` in the output directory

By default the script builds into `out/phillips`.

## Artifacts

The script produces these files when the build succeeds:

- `kernel.img`
- `resource.img`
- `boot.img`
- `zboot.img`

The packaging flow in this tree is:

1. Build `Image`, `zImage`, and the board DTB.
2. Wrap `zImage` into `kernel.img` with `scripts/mkkrnlimg`.
3. Pack the DTB and optional logos into `resource.img` with `scripts/resource_tool`.
4. Build `boot.img` and `zboot.img` with `scripts/mkbootimg`, using `resource.img` as the Android boot image `second` payload.

The helper script does not use the vendor `kernel.img` / `%.img` make targets directly. Instead it:

1. builds `Image`, `zImage`, and `rk3288-phillips-rk808.dtb`
2. runs `scripts/mkkrnlimg` itself
3. runs `scripts/resource_tool` itself
4. runs `scripts/mkbootimg` itself

That avoids two vendor build issues seen in `O=` builds on this environment:

- the `kernel.img` target recursively calling `make modules`
- the board `%.img` target depending on that broken path

## Logos

For this target, logos are enabled because the defconfig sets `CONFIG_FB=y`, and the tree contains:

- `logo.bmp`
- `logo_kernel.bmp`

If those files exist, they are included in `resource.img` together with the board DTB.

Current unpacked contents of the repo's `resource.img` are:

- `rk-kernel.dtb`
- `logo.bmp`
- `logo_kernel.bmp`

Typical role of each file:

- `logo.bmp`: early boot splash
- `logo_kernel.bmp`: later kernel-stage splash

To change the displayed logos, replace those BMP files and rebuild `resource.img` / `boot.img`.

## Unpacking `resource.img`

The bundled `scripts/resource_tool` has path handling bugs on this environment. Use the wrapper instead:

```bash
scripts/resource_tool_unpack.sh resource.img /tmp/resource_unpack
```

That extracts the resource payload into the chosen directory.
