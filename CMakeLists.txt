cmake_minimum_required(VERSION 3.10)
project(lvgl)
cmake_policy(SET CMP0079 NEW)

find_package(Python3 REQUIRED COMPONENTS Interpreter)

set(CMAKE_INTERPROCEDURAL_OPTIMIZATION TRUE)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_C_STANDARD 99)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra -Wpedantic -ggdb")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -pedantic -ggdb")

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

set(LV_BUILD_CONF_PATH
    "${CMAKE_BINARY_DIR}/lv_conf.h"
    CACHE PATH "path to lv_conf.h" FORCE)

set(LVGL_TEMPLATE_PATH "${CMAKE_SOURCE_DIR}/lvgl/lv_conf_template.h")

if(CONFIG STREQUAL "neon")
  set(LV_CONF_DEFAULTS_PATH "${CMAKE_SOURCE_DIR}/lv_conf_asm.defaults")
else()
  set(LV_CONF_DEFAULTS_PATH "${CMAKE_SOURCE_DIR}/lv_conf_sw.defaults")
endif()

set(GENERATE_SCRIPT_PATH "${CMAKE_SOURCE_DIR}/lvgl/scripts/generate_lv_conf.py")

configure_file(${LVGL_TEMPLATE_PATH} ${CMAKE_BINARY_DIR}/lv_conf_template.h
               COPYONLY)
configure_file(${LV_CONF_DEFAULTS_PATH} ${CMAKE_BINARY_DIR}/lv_conf.defaults
               COPYONLY)
configure_file(${GENERATE_SCRIPT_PATH} ${CMAKE_BINARY_DIR}/generate_lv_conf.py
               COPYONLY)

message(STATUS "Generating lv_conf.h from ${LV_CONF_DEFAULTS_PATH}")

execute_process(
  COMMAND
    ${Python3_EXECUTABLE} ${GENERATE_SCRIPT_PATH} --template
    ${LVGL_TEMPLATE_PATH} --defaults ${LV_CONF_DEFAULTS_PATH} --config
    ${LV_BUILD_CONF_PATH}
  RESULT_VARIABLE config_result
  OUTPUT_VARIABLE config_output
  ERROR_VARIABLE config_error)

if(NOT config_result EQUAL 0)
  message(
    FATAL_ERROR "Failed to generate lv_conf.h: ${config_output} ${config_error}"
  )
endif()

message(STATUS "lv_conf.h generated successfully")

if(ARCH STREQUAL "32b")
  set(CMAKE_C_FLAGS
      "${CMAKE_C_FLAGS} -march=armv7-a -mfloat-abi=hard -mfpu=neon -O3 -ggdb")
  set(CMAKE_CXX_FLAGS
      "${CMAKE_CXX_FLAGS} -march=armv7-a -mfloat-abi=hard -mfpu=neon -O3 -ggdb")
else()
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -march=armv8-a -O3")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -march=armv8-a -O3")
endif()

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra -Wpedantic")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wpedantic")

add_subdirectory(lvgl)

target_link_libraries(lvgl PUBLIC m pthread)

set(TARGET_NAME "${ARCH}_${CONFIG}")

set(EXECUTABLE_NAME "neon_benchmark_${TARGET_NAME}")

add_executable(${EXECUTABLE_NAME} src/main.c)
target_include_directories(${EXECUTABLE_NAME} PRIVATE lvgl/src lvgl/demos)
target_link_libraries(${EXECUTABLE_NAME} PRIVATE lvgl)

set(TEST_RENDER_NAME "${TARGET_NAME}_render_test")
add_executable(${TEST_RENDER_NAME} test/main.c)
target_include_directories(${TEST_RENDER_NAME} PRIVATE lvgl/demos)
target_link_libraries(${TEST_RENDER_NAME} PRIVATE lvgl lvgl_demos)

message(STATUS "=== Build Configuration Summary ===")
message(STATUS "Architecture: ${ARCH}")
message(STATUS "Configuration: ${CONFIG}")
message(STATUS "Target Name: ${TARGET_NAME}")
message(STATUS "Executable: ${EXECUTABLE_NAME}")
message(STATUS "===================================")
