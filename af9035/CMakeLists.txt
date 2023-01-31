cmake_minimum_required(VERSION 3.24)
project(pkt)

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
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall")
set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -Wextra -ggdb")

add_executable(pkt pkt.c)
add_executable(demux demux.c)
add_executable(pcm pcm.c)

if (USE_PCAP)
	add_compile_definitions(USE_PCAP)
	find_package(PkgConfig REQUIRED)
	pkg_check_modules(LIBPCAP REQUIRED IMPORTED_TARGET libpcap)

	target_link_libraries(pkt PUBLIC PkgConfig::LIBPCAP)
else()
	target_sources(pkt PRIVATE xxx.c xxx.h)
endif()
