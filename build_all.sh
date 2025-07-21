#!/bin/sh

archs=("32b" "64b")
configs=("none" "neon")
neon_versions=("asm" "intrinsics")

for arch in "${archs[@]}"; do
    for config in "${configs[@]}"; do
        if [ "$arch" = "32b" ]; then
            toolchain="cmake_toolchain_files/arm-none-linux-gnueabihf.cmake"
        else
            toolchain="cmake_toolchain_files/aarch64-none-linux-gnu.cmake"
        fi

        if [ "$config" = "neon" ]; then
            for neon_version in "${neon_versions[@]}"; do
                # Skip 64b+asm combination
                if [ "$arch" = "64b" ] && [ "$neon_version" = "asm" ]; then
                    continue
                fi
                cmake -B build_${arch}_${config}_${neon_version} \
                      -DCMAKE_BUILD_TYPE=Release \
                      -DARCH=$arch \
                      -DCONFIG=$config \
                      -DNEON_VERSION=$neon_version \
                      -DCMAKE_TOOLCHAIN_FILE=$toolchain
            done
        else
            cmake -B build_${arch}_${config} \
                  -DCMAKE_BUILD_TYPE=Release \
                  -DARCH=$arch \
                  -DCONFIG=$config \
                  -DCMAKE_TOOLCHAIN_FILE=$toolchain
        fi
    done
done

for arch in "${archs[@]}"; do
    for config in "${configs[@]}"; do
        if [ "$config" = "neon" ]; then
            for neon_version in "${neon_versions[@]}"; do
                if [ "$arch" = "64b" ] && [ "$neon_version" = "asm" ]; then
                    continue
                fi
                cmake --build build_${arch}_${config}_${neon_version} -j$(nproc)
            done
        else
            cmake --build build_${arch}_${config} -j$(nproc)
        fi
    done
done
