cmake_minimum_required(VERSION 3.23 FATAL_ERROR)

project(Handler)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

add_executable(handler main.cpp)

if(WIN32)
	target_link_libraries(handler wsock32 ws2_32)
endif()