#include <cassert>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

#include <fnmatch.h>
#include <git2.h>

namespace {

std::vector<std::string> read_lines(const std::filesystem::path &path)
{
	std::vector<std::string> ret;
	std::ifstream is(path);

	for (std::string line; std::getline(is, line);)
		ret.push_back(line);

	return ret;
}

bool git_match(const std::string &p, const std::string &path)
{
	std::string pattern{p};

	if (!pattern.empty() && pattern.back() == '/' && pattern.find_first_of('*') != std::string::npos)
		pattern.push_back('*');

	const char *ptr = pattern.c_str();
	const git_strarray array{.strings=const_cast<char **>(&ptr), .count=1};
	git_pathspec *m_pathspec;
	if (git_pathspec_new(&m_pathspec, &array)) {
		std::cerr << git_error_last()->message << '\n';
		exit(1);
	}

	auto ret = git_pathspec_matches_path(m_pathspec, GIT_PATHSPEC_DEFAULT, path.c_str()) == 1;
	git_pathspec_free(m_pathspec);
	return ret;
}

bool fn_match(const std::string &p, const std::string &path)
{
	std::string pattern{p};

#if 0
	if (!pattern.empty() && pattern.back() == '/')
		pattern[pattern.size() - 1] = '*';
	else
#endif
		pattern.push_back('*');

	return !fnmatch(pattern.c_str(), path.c_str(), 0);
}

void test(const std::string &prefix, const std::vector<std::string> &find,
	  const std::vector<std::string> &data)
{
	std::cout << "===" << prefix << "===\n";
	for (const auto &pattern: data) {
		//std::cout << '\t' << pattern << '\n';
		for (const auto &path : find) {
			bool gm = git_match(pattern, path);
			bool fm = fn_match(pattern, path);
			if (gm != fm)
				std::cout << "MISHIT: " << prefix <<
					     " gm=" << gm << " != fm=" << fm <<
					     " pattern=" << pattern << " path=" << path << '\n';
		}
	}
}

}

int main(int argc, char **argv)
{
	git_libgit2_init();

	if (argc < 4)
		return 1;
	const auto our = read_lines(argv[1]);
	const auto upstream = read_lines(argv[2]);
	const auto find = read_lines(argv[3]);

	//test("our", find, our);
	test("upstream", find, upstream);

	return 0;
}

