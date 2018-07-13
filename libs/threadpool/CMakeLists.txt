cmake_minimum_required(VERSION 2.8)
project(threadpool)

set(CMAKE_CXX_FLAGS  "-g -Wall -std=c++11")
add_definitions(-DBOOST_LOG_DYN_LINK)

file(GLOB SRC "*.cpp")
file(GLOB HEADER "*.h")
add_library(libthreadpool SHARED ${SRC})
set_target_properties(libthreadpool PROPERTIES PREFIX "")
install(TARGETS libthreadpool DESTINATION /usr/local/lib)
install(FILES ${HEADER} DESTINATION include/threadpool)
