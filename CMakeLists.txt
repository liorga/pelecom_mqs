cmake_minimum_required(VERSION 3.17)
project(pelecom_mqs C)

set(CMAKE_C_STANDARD 11)

add_executable(pelecom_mqs main.c pelecom.h pelecom.c stopwatch.c stopwatch.h random.c random.h entrance.c)