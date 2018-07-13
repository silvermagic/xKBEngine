cmake_minimum_required(VERSION 2.8)
project(common)

set(CMAKE_CXX_FLAGS  "-Wall -std=c++11")

add_executable(memorystream memorystream.cpp)
target_link_libraries(memorystream common)

add_executable(task task.cpp)
target_link_libraries(task common)

add_executable(timer timer.cpp)
target_link_libraries(timer common)

add_executable(connectionpool connectionpool.cpp)
include_directories(/usr/local/include)
link_directories(/usr/local/lib64)
target_link_libraries(connectionpool mysqlcppconn8)
