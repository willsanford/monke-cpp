cmake_minimum_required(VERSION 3.21)

project(monke_cpp)

add_compile_options(-Wall -Wextra -pedantic)
# SET(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -stdlib=libc++")
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
include_directories(src)
add_library(monke_core STATIC 
  src/token.h 
  src/token.cpp 
  src/lexer.h 
  src/lexer.cpp 
  src/ast.h 
  src/ast.cpp 
  src/parser.cpp 
  src/parser.h
        src/utils.h)

add_executable(monke_cpp src/main.cpp)
target_link_libraries(monke_cpp monke_core)

add_subdirectory(test)
