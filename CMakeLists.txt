cmake_minimum_required(VERSION 3.24)
project(monke_cpp)

add_compile_options(-Wall -Wextra -pedantic)
set(CMAKE_CXX_STANDARD 17)
include_directories(src)
add_library(monke_core STATIC src/token.h src/lexer.h src/lexer.cpp src/token.cpp)

add_executable(monke_cpp src/main.cpp)
target_link_libraries(monke_cpp monke_core)

add_subdirectory(test)