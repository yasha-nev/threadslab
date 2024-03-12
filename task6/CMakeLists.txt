cmake_minimum_required(VERSION 3.13)

project(openmppractice VERSION 1.0)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++20")

find_package(OpenMP REQUIRED)

add_executable(openmppractice main.cpp)

target_link_libraries(openmppractice PRIVATE OpenMP::OpenMP_CXX)




