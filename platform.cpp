#ifdef __linux__
# include "platform/linux.cpp"
#else
# ifdef WIN32
#  include "platform/windows.cpp"
#  error Unsupported platform
# endif
#endif
