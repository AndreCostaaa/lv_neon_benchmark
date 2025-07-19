# LVGL Neon Benchmark

This repository is meant to benchmark LVGL SW renderer with and without NEON support.

## Getting Started

### Prerequisites

The necessary toolchains can be installed from 

If cross-compiling, please install the two toolchains from `arm`'s official [website](https://developer.arm.com/downloads/-/arm-gnu-toolchain-downloads):
- `arm-none-linux-gnueabihf`
- `aarch64-none-linux-gnu`

After that, make sure they're available in your `PATH`.

## Building

CMake will make sure it builds the correct version with the correct `lv_conf`. Simply pass `ARCH` and `CONFIG` when bootstraping it.
Also don't forget the `toolchain.cmake` based on the architecture.

Here's an example that will build the application for a 32 bit ARM architecture with LVGL's Neon feature.

```bash
cmake -B build_32b_neon -DARCH=32b -DCONFIG=neon -DCMAKE_TOOLCHAIN_FILE=cmake_toolchain_files/arm-none-linux-gnueabihf.cmake
```

Same example for the 64 bit ARM architecture.

```bash
cmake -B build_64b_neon -DARCH=64b -DCONFIG=neon -DCMAKE_TOOLCHAIN_FILE=cmake_toolchain_files/aarch64-none-linux-gnu.cmake
```

To use the normal `draw sw` implementation use `-DCONFIG=none`.


Then simply build your benchmark application:

```bash
cmake --build <build_folder> -j$(nproc)
```

## Tests 

Render tests are provided to make sure that the NEON implementation render the same thing as the standard SW renderer.

To run the tests, first use the normal SW renderer to generate the reference images, and then run the test with the NEON renderer.

Here's how:

1. Compile both versions

```bash
cmake -B build_64b_none -DARCH=64b -DCONFIG=none -DCMAKE_TOOLCHAIN_FILE=cmake_toolchain_files/aarch64-none-linux-gnu.cmake
cmake -B build_64b_neon -DARCH=64b -DCONFIG=neon -DCMAKE_TOOLCHAIN_FILE=cmake_toolchain_files/aarch64-none-linux-gnu.cmake
cmake --build build_64b_neon -j$(nproc)
cmake --build build_64b_none -j$(nproc)
```

2. Copy the compiled tests to the target board

```bash
export SSH_USER=changeme
export SSH_ADDR=changeme
scp ./build_64b_neon/bin/64b_neon_render_test ./build_64b_none/bin/64b_none_render_test $SSH_USER@$SSH_ADDR:~
```

3. Prepare the workspace

```bash
# Generate the reference images folder
ssh $SSH_USER@$SSH_ADDR "mkdir -p ref_images/rgb565"
```

4. Generate the reference images by running the test with the standard SW renderer
```bash
ssh $SSH_USER@$SSH_ADDR "./64b_none_render_test"
```

The warnings complaining about the reference images not existing and informing you that it's creating them are expected

```bash
[User]	(0.000, +0)	test_render: Rendering ref_images/rgb565/demo_render_conical_gradient_opa_128.png main.c:21
[Warn]	(0.000, +0)	read_png_file: error 78: failed to open file for reading
 lv_test_screenshot_compare.c:181
[Warn]	(0.000, +0)	screenshot_compare: ref_images/rgb565/demo_render_conical_gradient_opa_128.png was not found, creating it now from the rendered screen lv_test_screenshot_compare.c:118
```

5. Finally run the test with the NEON SW renderer implementation

```bash
ssh $SSH_USER@$SSH_ADDR "./64b_neon_render_test"
```
