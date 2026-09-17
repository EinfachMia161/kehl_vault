cmake_minimum_required(VERSION 4.3)
project(kehl_vault LANGUAGES C CXX)
set(CMAKE_C_STANDARD 17)
set(CMAKE_C_STANDARD_REQUIRED ON)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
add_executable(kehl_vault main.cpp
        entry.c
        entry.h
        password.c
        password.h
)
