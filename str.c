#include <cstdio>
#include <string>
#include <dlfcn.h>

typedef std::string moje_t();

int main()
{
    moje_t* moje = (moje_t*)dlsym((void*)"a", "ble");
    std::string s = moje();
}
