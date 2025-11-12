#include <cxxopts.hpp>
#include <iostream>

#include <sl/helpers/Color.h>
#include <sl/git/Buf.h>
#include <sl/git/Git.h>

using Clr = SlHelpers::Color;

namespace {

void handleSHA(const SlGit::Repo &repo, const std::string &sha, bool renames)
{
	const auto commit = repo.commitRevparseSingle(sha);
	if (!commit)
		throw std::runtime_error("cannot find commit " + sha);

	if (commit->parentCount() > 1)
		throw std::runtime_error(commit->idStr() + " is a merge commit");

	const auto diff = repo.diff(*commit, *commit->parent());

	if (renames && diff->findSimilar(nullptr))
		throw std::runtime_error("diff findSimilar failed");

	struct SlGit::Diff::ForEachCB cb {
		.file = [](const git_diff_delta &delta,
				float) -> int {
			Clr(Clr::BLUE) << "FILE ======= " << delta.old_file.path << " -> " <<
					  delta.new_file.path << ' ' << delta.similarity << ' ' <<
					  delta.status;
			return 0;
		},
		.hunk = [](const git_diff_delta &,
				const git_diff_hunk &hunk) -> int {
			Clr(Clr::BLUE) << hunk.header << Clr::NoNL;
			return 0;
		},
		.line = [](const git_diff_delta &, const git_diff_hunk &,
				const git_diff_line &line) -> int {
			const auto org = line.origin;
			const auto color = org == '-' ? Clr::RED : org == '+' ? Clr::GREEN : Clr::DEFAULT;
			Clr(color) << line.origin <<
				      std::string_view(line.content, line.content_len) << Clr::NoNL;
			return 0;
		},
	};

	if (diff->forEach(cb))
		throw std::runtime_error("diff failed");

	const auto buf = diff->toBuf(GIT_DIFF_FORMAT_PATCH);
	if (!buf)
		throw std::runtime_error("diff to buf failed");

	std::cout << buf->sv();
}

void exc(const std::vector<std::string> &shas, bool renames)
{
	std::filesystem::path linux{"/home/xslaby/linux"};
	const auto repo = SlGit::Repo::open(linux);
	if (!repo)
		throw std::runtime_error("cannot open " + linux.string());

	for (const auto &sha: shas)
		handleSHA(*repo, sha, renames);

}

} // namespace

int main(int argc, char **argv)
{
	cxxopts::Options options { "git_renames", "git diff renames example" };
	bool renames;
	std::vector<std::string> shas;
	options.add_options()
		("h,help", "Show this help message")
		("r,renames", "Detect renames",
			cxxopts::value(renames)->default_value("false"))
		("shas", "List of shas",
			cxxopts::value(shas))
	;
	options.parse_positional("shas");
	options.positional_help("shas...");
	try {
		const auto opts = options.parse(argc, argv);
		if (opts.contains("help")) {
			std::cout << options.help();
			return 0;
		}
		if (shas.empty())
			throw std::runtime_error("too few args");
		exc(shas, renames);
	} catch (const cxxopts::exceptions::exception &e) {
		Clr(std::cerr, Clr::RED) << e.what();
		std::cerr << options.help();
		return EXIT_FAILURE;
	} catch (const std::runtime_error &e) {
		Clr(std::cerr, Clr::RED) << e.what();
		return EXIT_FAILURE;
	}

	return 0;
}

