cmake_minimum_required(VERSION 3.20)
project(HMG)
set(CMAKE_CXX_STANDARD 20)



set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Ofast -march=native -mtune=native -flto")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Ofast -march=native -mtune=native -flto")

set(FASTNOISE2_NOISETOOL OFF CACHE BOOL "Build Noise Tool" FORCE)



add_subdirectory(lib/FastNoise2)

add_subdirectory(lib/libpng)
add_subdirectory(lib/zlib)



include_directories(lib/FastNoise2/include)

include_directories(lib/libpng)
include_directories(lib/zlib)

include_directories(lib/opencla/include/api)
include_directories(lib/opencla/include/cpp)
include_directories(lib/opencla/include/utils)



add_executable(HMG src/main.cpp)



target_link_libraries(HMG FastNoise2 png_static zlibstatic OpenCLHeaders OpenCLICDLoader OpenCLHeadersCpp)
