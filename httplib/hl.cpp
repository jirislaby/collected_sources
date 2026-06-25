#include <iostream>
#include <string>
#include <httplib.h>

int main()
{
	httplib::Client cl("http://localhost:12780");

	std::string url("/31/3a08b67728257df5f2c65cb5aeb70b6eef1d17x");
	std::cerr << "url=" << url << '\n';
	auto res = cl.Get(url);
	std::cerr << "res=" << !!res << ' ' << res.error() << '\n';

	std::cerr << "url2=" << url << '\n';

	return 0;
}

