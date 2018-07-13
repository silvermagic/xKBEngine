cmake_minimum_required(VERSION 2.8)
project(network)

set(CMAKE_CXX_FLAGS  "-g -Wall -std=c++11")
add_definitions(-DBOOST_LOG_DYN_LINK)

file(GLOB SRC "*.cpp")
file(GLOB HEADER "*.h")
add_library(libnetwork SHARED ${SRC})
set_target_properties(libnetwork PROPERTIES PREFIX "")
install(TARGETS libnetwork DESTINATION /usr/local/lib)
install(FILES ${HEADER} DESTINATION include/network)
