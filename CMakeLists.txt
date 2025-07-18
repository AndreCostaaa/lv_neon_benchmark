cmake_minimum_required(VERSION 3.10)
project(lvgl)
cmake_policy(SET CMP0079 NEW)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_C_STANDARD 99)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR}/bin)

set(ARCH
    "64b"
    CACHE STRING "Target architecture (32b or 64b)")
set_property(CACHE ARCH PROPERTY STRINGS "32b" "64b")

set(CONFIG
    "none"
    CACHE STRING "Configuration type (none or neon)")
set_property(CACHE CONFIG PROPERTY STRINGS "none" "neon")

if(NOT ARCH MATCHES "^(32b|64b)$")
  message(FATAL_ERROR "Invalid ARCH value: ${ARCH}. Must be '32b' or '64b'")
endif()

if(NOT CONFIG MATCHES "^(none|neon)$")
  message(
    FATAL_ERROR "Invalid CONFIG value: ${CONFIG}. Must be 'none' or 'neon'")
endif()

message(STATUS "Architecture: ${ARCH}")
message(STATUS "Configuration: ${CONFIG}")

if(ARCH STREQUAL "32b")
  set(CMAKE_C_FLAGS
      "${CMAKE_C_FLAGS} -march=armv7-a -mfloat-abi=hard -mfpu=neon -O3 -ggdb")
  set(CMAKE_CXX_FLAGS
      "${CMAKE_CXX_FLAGS} -march=armv7-a -mfloat-abi=hard -mfpu=neon -O3 -ggdb")
else()
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -march=armv8-a")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -march=armv8-a")
endif()

if(CONFIG STREQUAL "none")
  message(STATUS "Building with standard draw sw render")

  set(LV_BUILD_CONF_PATH
      "${CMAKE_CURRENT_SOURCE_DIR}/lv_conf_draw_asm_none.h"
      CACHE PATH "" FORCE)
elseif(CONFIG STREQUAL "neon")
  message(STATUS "Building with NEON configuration")
  if(ARCH STREQUAL "32b")
    message(STATUS "Building with NEON 32b configuration")
    set(LV_BUILD_CONF_PATH
        "${CMAKE_CURRENT_SOURCE_DIR}/lv_conf_draw_asm_neon.h"
        CACHE PATH "" FORCE)

  elseif(ARCH STREQUAL "64b")

    set(LV_BUILD_CONF_PATH
        "${CMAKE_CURRENT_SOURCE_DIR}/lv_conf_draw_asm_neon64.h"
        CACHE PATH "" FORCE)

  endif()
endif()

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra -Wpedantic")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wpedantic")

if(CONFIG STREQUAL "neon" AND ARCH STREQUAL "64b")
  add_subdirectory(lvgl_neon64)
else()
  add_subdirectory(lvgl)
endif()

target_link_libraries(lvgl PUBLIC m pthread)

set(TARGET_NAME "${ARCH}_${CONFIG}")
set(EXECUTABLE_NAME "neon_benchmark_${TARGET_NAME}")
add_executable(${EXECUTABLE_NAME} src/main.c)
target_include_directories(${EXECUTABLE_NAME} PRIVATE lvgl/src lvgl/demos)
target_link_libraries(${EXECUTABLE_NAME} PRIVATE lvgl)

message(STATUS "=== Build Configuration Summary ===")
message(STATUS "Architecture: ${ARCH}")
message(STATUS "Configuration: ${CONFIG}")
message(STATUS "Target Name: ${TARGET_NAME}")
message(STATUS "Executable: ${EXECUTABLE_NAME}")
message(STATUS "===================================")
