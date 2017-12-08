cmake_minimum_required(VERSION 3.3)
project(zeromq_examples)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/Modules/")
include_directories(.)
add_subdirectory(hwm)
add_subdirectory(libevent-integration)


