cmake_minimum_required(VERSION 3.16)
project(Chip-8 CXX)

find_package(SDL3 REQUIRED)

add_executable(${PROJECT_NAME})
target_sources(${PROJECT_NAME} PRIVATE
                source/main.cpp
                source/Chip-8.cpp
                source/Graphics.cpp)

target_link_libraries(${PROJECT_NAME} PRIVATE SDL3::SDL3)