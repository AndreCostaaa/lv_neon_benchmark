#!/bin/sh

archs=("32b" "64b")
configs=("none" "neon")

for arch in "${archs[@]}"; do
    for config in "${configs[@]}"; do
        if [ "$arch" = "32b" ]; then
            toolchain="cmake_toolchain_files/arm-none-linux-gnueabihf.cmake"
        else
            toolchain="cmake_toolchain_files/aarch64-none-linux-gnu.cmake"
        fi
        
        cmake -B build_${arch}_${config} -DCMAKE_BUILD_TYPE=Release -DARCH=$arch -DCONFIG=$config -DCMAKE_TOOLCHAIN_FILE=$toolchain
    done
done

for arch in "${archs[@]}"; do
    for config in "${configs[@]}"; do
	cmake --build build_${arch}_${config} -j$(nproc)
    done
done
