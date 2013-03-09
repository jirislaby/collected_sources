bin_PROGRAMS = snd_fft

snd_fft_SOURCES = snd_fft.c

snd_fft_LDADD = $(alsa_LIBS) $(fftw3_LIBS) $(sndfile_LIBS)

AM_CFLAGS = $(alsa_CFLAGS) $(fftw3_CFLAGS) $(sndfile_CFLAGS) $(WARNINGFLAGS)
