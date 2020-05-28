cmake_minimum_required(VERSION 3.0.0)
project(example)

find_package(ImageMagick COMPONENTS Magick++)

add_executable(example main.cpp)

target_sources(example PRIVATE README.md) 
set_target_properties(example PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED ON
)