cmake_minimum_required(VERSION 2.8)
project(common)

set(CMAKE_CXX_FLAGS  "-g -Wall -std=c++11")

file(GLOB SRC "*.cpp")
file(GLOB HEADER "*.h")
add_library(libcommon SHARED ${SRC})
set_target_properties(libcommon PROPERTIES PREFIX "")
install(TARGETS libcommon DESTINATION /usr/local/lib)
install(FILES ${HEADER} DESTINATION include/common)
