cmake_minimum_required(VERSION 2.8)
project(threadpool)

set(CMAKE_CXX_FLAGS  "-g -Wall -std=c++11")
add_definitions(-DBOOST_LOG_DYN_LINK)

add_executable(tp main.cpp)
target_link_libraries(tp common threadpool boost_system boost_log boost_log_setup pthread)
