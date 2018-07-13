cmake_minimum_required(VERSION 2.8)
project(tcp)

set(CMAKE_CXX_FLAGS  "-g -Wall -std=c++11")
add_definitions(-DBOOST_LOG_DYN_LINK)

add_executable(srv server.cpp)
add_executable(cli client.cpp)
target_link_libraries(srv common network boost_system boost_log boost_log_setup pthread)
target_link_libraries(cli common network boost_system boost_log boost_log_setup pthread)
