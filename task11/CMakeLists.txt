cmake_minimum_required(VERSION 3.5)

project(opencvtest LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package( OpenCV REQUIRED )
include_directories( ${OpenCV_INCLUDE_DIRS} )

find_package(MPI  REQUIRED )
include_directories( ${MPI_INCLUDE_PATH}} )

add_executable(opencvtest main.cpp)

install(TARGETS opencvtest
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})

target_link_libraries(opencvtest ${OpenCV_LIBS} MPI::MPI_CXX)
