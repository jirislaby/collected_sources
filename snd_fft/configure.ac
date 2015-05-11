AC_PREREQ(2.50)
AC_INIT([snd_fft], [3.36])
AM_INIT_AUTOMAKE([-Wall])

AC_LANG_C
AC_PROG_CC

AC_LANG_SOURCE

# ================ start WARNINGFLAGS

WARNINGFLAGS=""

AC_MSG_CHECKING([whether compiler supports -Wall])
orig_CFLAGS="$CFLAGS"
CFLAGS="$CFLAGS -Wall"
AC_COMPILE_IFELSE([AC_LANG_SOURCE([int a;])], [AC_MSG_RESULT([yes]); WARNINGFLAGS="$WARNINGFLAGS -Wall"], AC_MSG_RESULT([no]))
CFLAGS="$orig_CFLAGS"

AC_SUBST(WARNINGFLAGS)

# ================ end WARNINGFLAGS

AC_SEARCH_LIBS([cabs], [m])
AC_SEARCH_LIBS([initscr], [ncurses])
AC_SEARCH_LIBS([stdscr], [tinfo])

PKG_CHECK_MODULES([alsa], [alsa])
PKG_CHECK_MODULES([fftw3], [fftw3])
PKG_CHECK_MODULES([sndfile], [sndfile])

AC_CONFIG_FILES([Makefile])
AC_OUTPUT
