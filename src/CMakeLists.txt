cmake_minimum_required(VERSION 3.15)

set(CMAKE_C_STANDARD 11)

include_directories(${PROJECT_SOURCE_DIR}/include)

# 添加对gdb的支持
SET(CMAKE_BUILD_TYPE "Debug")
SET(CMAKE_C_FLAGS_DEBUG "$ENV{CFLAGS} -O0 -Wall -g -D_FILE_OFFSET_BITS=64 -ggdb")
SET(CMAKE_C_FLAGS_RELEASE "$ENV{CFLAGS} -O3 -Wall")
SET(CMAKE_C_FLAGS "-D_FILE_OFFSET_BITS=64 -lfuse3")

file(GLOB_RECURSE native_srcs ./*.c)

add_library(Bfs STATIC ${native_srcs})

target_link_libraries(Bfs -lrt -lfuse)
