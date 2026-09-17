cmake_minimum_required(VERSION 4.3)
project(kehl_vault)

set(CMAKE_CXX_STANDARD 20)

add_executable(kehl_vault main.cpp
        entry.c
        entry.h)
