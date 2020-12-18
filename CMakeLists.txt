cmake_minimum_required(VERSION 3.17)
project(pelecom_mqs C)

set(CMAKE_C_STANDARD 11)

add_executable(pelecom_mqs main.c new.c sorter.c stop.c repair.c upgrade.c pelecom.h stopwatch.c stopwatch.h random.c random.h)