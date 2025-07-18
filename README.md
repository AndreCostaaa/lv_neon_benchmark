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

