cmake_minimum_required(VERSION 3.22)
project(tai_assignment2_group8)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "-Wall -O3")

add_executable(what_the_music
        src/what_the_music.cpp
        src/util/csv_logger.cpp
        src/util/csv_logger.h
        src/util/file_reader.cpp
        src/util/file_reader.h
        src/util/program_arguments.cpp
        src/util/program_arguments.h)
