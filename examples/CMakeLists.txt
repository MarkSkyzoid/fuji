cmake_minimum_required (VERSION 3.8)

project(examples)

file(GLOB examples_SRC
    "*.hpp"
    "*.cpp"
)

add_executable(examples ${examples_SRC})