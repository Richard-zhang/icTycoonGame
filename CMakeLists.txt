cmake_minimum_required(VERSION 3.5)
project(finalTrial)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=c11 -lSDL2_ttf -lSDL2 -lSDL2_image")

set(SOURCE_FILES main.c main.h)
add_executable(finalTrial ${SOURCE_FILES})