cmake_minimum_required(VERSION 4.3)
project(kehl_vault LANGUAGES C CXX)
set(CMAKE_C_STANDARD 17)
set(CMAKE_C_STANDARD_REQUIRED ON)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})

add_executable(kehl_vault main.cpp
        entry.c
        entry.h
        password.c
        password.h
        storage.c
        storage.h
        crypto.c
        crypto.h
        clipboard.c
        clipboard.h
        audit.c
        audit.h
        impex.c
        impex.h
)

add_executable(test_vault
        tests/test_vault.cpp
        entry.c
        entry.h
        password.c
        password.h
        storage.c
        storage.h
        crypto.c
        crypto.h
        clipboard.c
        clipboard.h
        audit.c
        audit.h
        impex.c
        impex.h
)

if (WIN32)
    target_link_libraries(kehl_vault bcrypt)
    target_link_libraries(test_vault bcrypt)
endif ()
