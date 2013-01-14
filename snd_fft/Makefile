CFLAGS=-g -O2 -Wall $(shell pkg-config --cflags alsa fftw3 sndfile)
LDFLAGS=$(shell pkg-config --libs alsa fftw3 sndfile) -lncurses -ltinfo

all: snd_fft

clean:
	@rm -f snd_fft
