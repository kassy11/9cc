cmake_minimum_required(VERSION 3.16)
project(9cc C)

set(CMAKE_C_STANDARD 99)

add_executable(9cc main.c container.c 9cc.h parse.c codegen.c tokenize.c)