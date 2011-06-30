#ifndef PLATFORM_H_FILE
#define PLATFORM_H_FILE

#ifdef __linux__
# include "platform/linux.h"
#else
# ifdef WIN32
#  include "platform/windows.h"
# else
#  error Unsupported platform
# endif
#endif

#endif
