cmake_minimum_required(VERSION 3.16)
project(Chip-8 CXX)

set(CMAKE_CXX_STANDARD 23)

find_package(SDL3 REQUIRED)

add_compile_options(-Wall -pedantic-errors -Weffc++ -Wextra -Wconversion -Wsign-conversion)

if (WIN32)
    add_executable(${PROJECT_NAME} WIN32)
else()
    add_executable(${PROJECT_NAME})
endif()

set(IM_GUI_SOURCES 
    source/ImGui/imgui_draw.cpp
    source/ImGui/imgui_impl_sdl3.cpp
    source/ImGui/imgui_tables.cpp
    source/ImGui/imgui_widgets.cpp
    source/ImGui/imgui.cpp
    source/ImGui/imgui_impl_sdlrenderer3.cpp)

add_subdirectory(source/nativefiledialog-extended)

target_sources(${PROJECT_NAME} PRIVATE
                ${IM_GUI_SOURCES}         
                source/main.cpp
                source/Chip-8.cpp
                source/SDL.cpp
                source/Gui.cpp)

target_link_libraries(${PROJECT_NAME} PRIVATE SDL3::SDL3 nfd)