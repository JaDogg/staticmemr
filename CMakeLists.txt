cmake_minimum_required(VERSION 3.21)
project(staticmemr C)

set(CMAKE_C_STANDARD 99)

add_executable(test_stack_allocated test_stack_allocated.c stmmr.h)
add_executable(test_wrap_custom_pool test_wrap_custom_pool.c stmmr.h)
