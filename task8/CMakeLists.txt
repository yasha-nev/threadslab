cmake_minimum_required(VERSION 3.5)

project(livegame LANGUAGES CXX)

#//opt/homebrew/Cellar/libomp/18.1.1/lib/* /opt/homebrew/Cellar/libomp/18.1.1/include/*

find_package(OpenMP COMPONENTS CXX REQUIRED)

include_directories(Terminalcanvas)

add_executable(livegame main.cpp)

add_subdirectory(Terminalcanvas)	

target_link_libraries(livegame canvas )