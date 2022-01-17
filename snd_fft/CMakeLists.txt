cmake_minimum_required(VERSION 3.18.0)

project(snd_fft VERSION 3.36.0 LANGUAGES C)

add_compile_options(-Wall)

find_package(PkgConfig REQUIRED)

set(CURSES_NEED_NCURSES true)
find_package(Curses REQUIRED)
find_package(SndFile REQUIRED)

pkg_check_modules(ALSA REQUIRED alsa)
pkg_check_modules(FFTW3 REQUIRED fftw3)

include_directories(${CURSES_INCLUDE_DIRS}
	${SNDFILE_INCLUDE_DIRS}
	${ALSA_INCLUDE_DIRS}
	${FFTW3_INCLUDE_DIRS})
add_definitions(${CURSES_CFLAGS}
	${SNDFILE_CFLAGS}
	${ALSA_CFLAGS}
	${FFTW3_CFLAGS})

add_executable(snd_fft
	snd_fft.c
	)

target_link_libraries(snd_fft m
	${CURSES_LIBRARIES}
	${CURSES_TINFO_LIBRARY_NAME}
	${SNDFILE_LIBRARIES}
	${ALSA_LIBRARIES}
	${FFTW3_LIBRARIES})
