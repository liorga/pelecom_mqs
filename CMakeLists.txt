cmake_minimum_required(VERSION 3.17)
project(pelecom_mqs C)

set(CMAKE_C_STANDARD 11)
set(GCC_COVERAGE_COMPILE_FLAGS "-lm")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${GCC_COVERAGE_COMPILE_FLAGS}" )

add_executable(pelecom_mqs main.c stop.c pelecom.h stopwatch.c stopwatch.h random.c random.h)
target_link_libraries(pelecom_mqs m)
