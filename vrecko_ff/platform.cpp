#ifdef __linux__
# include "platform/linux.cpp"
#else
# ifdef WIN32
#  include "platform/windows.cpp"
# else
#  error Unsupported platform
# endif
#endif
