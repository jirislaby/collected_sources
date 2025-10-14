#include <cxxopts.hpp>
#include <iostream>
#include <string>

int main(int argc, char **argv)
{
	cxxopts::Options options {argv[0], "cxx" };
	struct gm {
		bool cxx = true;
		std::string str = "xycht";
		unsigned val = ~0U;
		int ival = -1;
		std::filesystem::path path = "/dev/null";
	} gm;

	options.add_options()
		("b,bool", "boolean", cxxopts::value(gm.cxx)->default_value("0"))
		("s,str", "string", cxxopts::value(gm.str)->default_value("nic"))
		("p,path", "path", cxxopts::value(gm.path)->default_value("/dev/zero"))
		("h,help", "help")
	;
	options.add_options("numeric")
		("i,signed", "signed int", cxxopts::value(gm.ival)->default_value("-100"))
		("u,unsigned", "unsigned int", cxxopts::value(gm.val)->default_value("200"))
	;

	auto opts = options.parse(argc, argv);

	std::cout << "bool=" << gm.cxx << '\n';
	std::cout << "str=" << gm.str << '\n';
	std::cout << "path=" << gm.path << '\n';

	std::cout << "signed=" << gm.ival << '\n';
	std::cout << "unsigned=" << gm.val << '\n';

	std::cout << options.help();

	return 0;
}

