cmake_minimum_required(VERSION 3.16)
project(FileSystem)

set(CMAKE_C_STANARD 11)

# 添加对gdb的支持
SET(CMAKE_BUILD_TYPE "Debug")
SET(CMAKE_CXX_FLAGS_DEBUG "$ENV{CFLAGS} -O0 -Wall -g -ggdb")
SET(CMAKE_CXX_FLAGS_RELEASE "$ENV{CFLAGS} -O3 -Wall")
SET(CMAKE_C_FLAGS "-D_FILE_OFFSET_BITS=64 -lfuse3")

add_subdirectory(src)

include_directories(./include)

set(EXECUTABLE_OUTPUT_PATH ./bin)

add_executable(BBFS ./main/bbfs.c)

target_link_libraries(BBFS Bfs)
