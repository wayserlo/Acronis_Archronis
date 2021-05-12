cmake_minimum_required(VERSION 3.1)

project(Archiver)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17")
set(CMAKE_CXX_STANDARD_REQUIRED on)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${Archiver_SOURCE_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${Archiver_SOURCE_DIR}/lib)

include_directories(.)

add_subdirectory(src)

add_executable(Archiver main.cpp)

target_link_libraries(Archiver 
                      Archiver_src)

include_directories(${CMAKE_CURRENT_BINARY_DIR})