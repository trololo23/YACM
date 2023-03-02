cmake_minimum_required(VERSION 3.22)

project(shared)

set(SRC_LIST open.c ../errhandler.c)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fPIC")

add_library(${PROJECT_NAME} SHARED ${SRC_LIST})