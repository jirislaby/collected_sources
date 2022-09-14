cmake_minimum_required(VERSION 3.24)
project(kfifo-test)

include(CheckIPOSupported)
check_ipo_supported(RESULT IPO_SUPPORTED)
if(IPO_SUPPORTED)
	set(CMAKE_INTERPROCEDURAL_OPTIMIZATION TRUE)
endif()

set(CMAKE_POSITION_INDEPENDENT_CODE ON)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra")

add_executable(kfifo-test kfifo-test.c)
