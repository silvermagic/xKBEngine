cmake_minimum_required(VERSION 2.8)
project(pyscrpit)

set(CMAKE_CXX_FLAGS  "-g -Wall -std=c++11")
add_definitions(-DBOOST_LOG_DYN_LINK)

file(GLOB SRC "*.cpp")
add_executable(scrpit ${SRC})

INCLUDE(FindPkgConfig)
pkg_check_modules(PYTHON REQUIRED python3)
include_directories(${PYTHON_INCLUDE_DIRS})
link_directories(${PYTHON_LIBRARY_DIRS})
target_link_libraries(scrpit common pyscript boost_system boost_log boost_log_setup pthread ${PYTHON_LIBRARIES})
