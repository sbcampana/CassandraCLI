cmake_minimum_required(VERSION 3.8)
project(CassandraCLI)

set(CMAKE_CXX_STANDARD 11)

set(SOURCE_FILES
        config.h
        lex.c
        main.c
        Makefile)

add_executable(CassandraCLI ${SOURCE_FILES})