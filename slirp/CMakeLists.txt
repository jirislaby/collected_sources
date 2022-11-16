cmake_minimum_required(VERSION 3.24)
project(slirp_test)

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
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wno-address-of-packed-member")
set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -Wextra -ggdb")

find_package(PkgConfig REQUIRED)
pkg_check_modules(SLIRP REQUIRED slirp)


add_executable(slirp_test slirp_test.c)

include_directories(${SLIRP_INCLUDE_DIRS})
target_compile_options(slirp_test PUBLIC ${SLIRP_CFLAGS_OTHER})
target_include_directories(slirp_test PUBLIC ${SLIRP_INCLUDE_DIRS})
target_link_libraries(slirp_test PUBLIC ${SLIRP_LIBRARIES})
