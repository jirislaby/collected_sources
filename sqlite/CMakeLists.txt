cmake_minimum_required(VERSION 3.24)
project(sqlite)

if (NOT CMAKE_BUILD_TYPE)
	message("Setting CMAKE_BUILD_TYPE to Debug")
	set(CMAKE_BUILD_TYPE "Debug")
endif()

include(CheckIPOSupported)
check_ipo_supported(RESULT IPO_SUPPORTED)
if(IPO_SUPPORTED)
	set(CMAKE_INTERPROCEDURAL_OPTIMIZATION TRUE)
endif()

set(CMAKE_POSITION_INDEPENDENT_CODE ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -Wextra -ggdb")

find_package(PkgConfig REQUIRED)

pkg_check_modules(SQLITE3 REQUIRED sqlite3)

add_executable(sqlite sqlite.cpp)

target_link_libraries(sqlite ${SQLITE3_LIBRARIES})
target_include_directories(sqlite PUBLIC ${SQLITE3_INCLUDE_DIRS})
target_compile_options(sqlite PUBLIC ${SQLITE3_CFLAGS})
