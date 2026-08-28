cmake_minimum_required(VERSION 3.20)
project(Automotive_CAN_OBD2_Core C)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)

include_directories(include)

file(GLOB_RECURSE SOURCES "src/*.c")

add_executable(obd_diag_core ${SOURCES})
target_compile_options(obd_diag_core PRIVATE -Wall -Wextra -O2)
