cmake_minimum_required (VERSION 3.8)

project(fuji)

find_package(Vulkan REQUIRED)

add_subdirectory(ext)
add_subdirectory(examples)
add_subdirectory(fuji)