cmake_minimum_required(VERSION 3.7.2)

project(cube-crawler VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_C_COMPILER "arm-linux-gnueabi-gcc")
set(CMAKE_CC_COMPILER "arm-linux-gnueabi-gcc")
set(CMAKE_CXX_COMPILER "arm-linux-gnueabi-g++")

# Add ev3dev-lang-cpp submodule as an externally imported lib
add_library(ev3dev STATIC IMPORTED)
set_target_properties(ev3dev PROPERTIES 
  IMPORTED_LOCATION ${CMAKE_SOURCE_DIR}/external/ev3dev-lang-cpp/build/libev3dev.a)

# The default target
add_executable(cube-crawler main.cpp rubiks.cpp worker.cpp device.cpp)
target_link_libraries(cube-crawler -static ev3dev)
target_include_directories(cube-crawler 
  PRIVATE "${CMAKE_SOURCE_DIR}/external/ev3dev-lang-cpp")
