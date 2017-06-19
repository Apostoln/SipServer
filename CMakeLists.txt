cmake_minimum_required(VERSION 3.5)
project(SipServer)

set(CMAKE_VERBOSE_MAKEFILE ON)

set(CMAKE_CXX_STANDARD 14)
set(GCC_COVERAGE_COMPILE_FLAGS "-DASIO_STANDALONE -pthread")

add_library(asio INTERFACE )
target_include_directories(asio INTERFACE lib/asio)

include_directories(src)

set(SOURCE_FILES main.cpp src/SipServer.hpp src/SipServer.cpp)
add_executable(${PROJECT_NAME} ${SOURCE_FILES})
target_link_libraries(${PROJECT_NAME} asio pthread)