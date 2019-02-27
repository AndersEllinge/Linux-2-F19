cmake_minimum_required(VERSION 2.8)

project(engine1)
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_FLAGS -pthread)
set(CMAKE_CONFIGURATION_TYPES "RELEASE" CACHE STRING "" FORCE)
add_executable(${PROJECT_NAME} main.cpp engine.cpp engine.h drmImpl.c drmImpl.h sprite.cpp sprite.h)
