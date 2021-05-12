cmake_minimum_required(VERSION 3.0)
project(Archiver_src)

set(
    SOURCES
        encoder/encoder.cpp
        collecter/collecter.cpp
        zipper.cpp
)

set(
    HEADERS
        zipper.hpp
        encoder/encoder.hpp
        support/Istreams.hpp
        encoder/tree/tree.hpp
        collecter/collecter.hpp
)

add_library(Archiver_src SHARED ${SOURCES} ${HEADERS})