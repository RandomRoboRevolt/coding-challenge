cmake_minimum_required(VERSION 3.5)
project(EmbeddedDevelopment)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_subdirectory(motordriver)
add_subdirectory(master)
