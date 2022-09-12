cmake_minimum_required(VERSION 3.23 FATAL_ERROR)

project(Sender)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

add_executable(sender 
	main.cpp
	input.h
	input.cpp)

if(WIN32)
	target_link_libraries(sender wsock32 ws2_32)
endif()