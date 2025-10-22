#include <cxxopts.hpp>
#include <iostream>
#include <set>
#include <string>
#include <vector>

int main(int argc, char **argv)
{
	cxxopts::Options options {argv[0], "cxx" };
	struct gm {
		bool cxx = true;
		std::string str = "xycht";
		unsigned val = ~0U;
		int ival = -1;
		std::filesystem::path path = "/dev/null";
		std::set<std::string> set;
		std::vector<std::string> vec;
	} gm;

	std::vector<std::string> set;
	options.add_options()
		("h,help", "help")
		("b,bool", "boolean", cxxopts::value(gm.cxx)->default_value("0"))
		("s,str", "string", cxxopts::value(gm.str)->default_value("nic"))
		("p,path", "path", cxxopts::value(gm.path)->default_value("/dev/zero"))
		("v,vec", "vector", cxxopts::value(gm.vec))
		("S,set", "set", cxxopts::value(set))
	;
	options.add_options("numeric")
		("i,signed", "signed int", cxxopts::value(gm.ival)->default_value("-100"))
		("u,unsigned", "unsigned int", cxxopts::value(gm.val)->default_value("200"))
	;

	std::cout << options.help() << '\n';

	auto opts = options.parse(argc, argv);

	gm.set.insert(std::make_move_iterator(set.begin()), std::make_move_iterator(set.end()));

	std::cout << "bool=" << gm.cxx << '\n';
	std::cout << "str=" << gm.str << '\n';
	std::cout << "path=" << gm.path << '\n';

	std::cout << "signed=" << gm.ival << '\n';
	std::cout << "unsigned=" << gm.val << '\n';

	std::cout << "vec:\n";
	for (const auto &e: gm.vec)
		std::cout << '\t' << e << '\n';

	std::cout << "set (" << gm.set.size() << "):\n";
	for (const auto &e: gm.set)
		std::cout << '\t' << e << '\n';

	return 0;
}

