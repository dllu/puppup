cmake_minimum_required(VERSION 2.8)
project(puppup)
list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")

find_package(Eigen3 REQUIRED)

set(SCRABBLE_INCLUDE_DIRS
	${PROJECT_SOURCE_DIR}/src
	${PROJECT_SOURCE_DIR}/third-party
    )

################################################################

# avx_sincos segfaults without -mavx2
set(CMAKE_CXX_FLAGS         "${CMAKE_CXX_FLAGS} -std=c++14 -march=native -Wall -Wpedantic -Werror")
set(CMAKE_CXX_FLAGS_DEBUG   "${CMAKE_CXX_FLAGS_DEBUG} -O0 -g")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -Ofast")
set(LLVM_OPT_IR_FLAGS "-inline-threshold=10000")

################################################################

include_directories(${SCRABBLE_INCLUDE_DIRS})

add_executable(
    puppup
    src/main.cpp
    src/movegen.cpp
)
