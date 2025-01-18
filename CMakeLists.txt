cmake_minimum_required(VERSION 3.30)
project(COP290 C)

set(CMAKE_C_STANDARD 17)

add_executable(COP290 src/main.c
        src/backend.c
        src/backend.h
        src/utils.h
        src/vec.c
        src/vec.h
        src/parser.c
        src/parser.h
        src/frontend.c
        src/frontend.h
        src/functions.c
        src/functions.h)
