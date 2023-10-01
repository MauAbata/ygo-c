cmake_minimum_required(VERSION 3.15)
project(ygo-c C)

set(CMAKE_C_STANDARD 11)

project(ygo-c VERSION 0.0.1 DESCRIPTION "YuGiOh Cards, Programmatically.")
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)
add_library(ygo-c STATIC ${CMAKE_CURRENT_SOURCE_DIR}/src/ygo_card.c)
