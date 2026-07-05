cmake_minimum_required(VERSION 3.16)

project(load_balancer VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(nlohmann_json REQUIRED)

include_directories(include)

file(GLOB_RECURSE SOURCES
    src/*.cpp
)

add_executable(load_balancer ${SOURCES})

target_link_libraries(load_balancer
    PRIVATE
    nlohmann_json::nlohmann_json
)

target_compile_options(load_balancer PRIVATE
    -Wall
    -Wextra
    -Wpedantic
)