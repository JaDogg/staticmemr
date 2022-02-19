cmake_minimum_required(VERSION 3.21)
project(staticmemr C)

set(CMAKE_C_STANDARD 99)

add_executable(staticmemr main.c stmmr.h)
