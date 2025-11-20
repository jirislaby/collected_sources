#include <iostream>

#include <sl/git/Buf.h>
#include <sl/git/Git.h>
#include <sl/git/Misc.h>
#include <sl/helpers/Color.h>
#include <sl/helpers/String.h>

using Clr = SlHelpers::Color;

namespace {

const std::string bigBang {"1da177e4c3f41524e886b7f1b8a0c1fc7321cac2"};

void handleCommit(const SlGit::Repo &lrepo,
		  const SlGit::Commit &commit)
{
	const auto sha = commit.idStr();
	Clr(Clr::GREEN) << sha.substr(0, 12) << ' ' << commit.summary();
	git_diff_options opts = GIT_DIFF_OPTIONS_INIT;
	opts.flags |= GIT_DIFF_DISABLE_PATHSPEC_MATCH;
	auto diff = lrepo.diff(*commit.parent(), commit, &opts);
	if (!diff)
		throw std::runtime_error("cannot get a diff for " + commit.idStr() + ": " +
					 lrepo.lastError().first);
	return;
	Clr(std::cerr, Clr::GREEN) << "\tfindSimilar";
	if (diff->findSimilar())
		throw std::runtime_error("cannot find renames in a diff: " +
					 lrepo.lastError().first);

	struct SlGit::Diff::ForEachCB cb {
		.file = [](const git_diff_delta &delta, float) -> int {
			switch (delta.status) {
			case GIT_DELTA_ADDED: {
				std::string newFile(delta.new_file.path);
				break;
			}
			case GIT_DELTA_DELETED: {
				std::string oldFile(delta.old_file.path);
				break;
			}
			case GIT_DELTA_RENAMED: {
				std::string oldFile(delta.old_file.path);
				break;
			}
			default:
				break;
			}
			/*Clr(Clr::BLUE) << "FILE ======= " << delta.old_file.path << " -> " <<
					  delta.new_file.path << ' ' << delta.similarity << ' ' <<
					  delta.status;*/
			return 0;
		},
	};

	Clr(std::cerr, Clr::GREEN) << "\tfor each";
	if (diff->forEach(cb))
		throw std::runtime_error("diff failed: " + lrepo.lastError().first);
}

void between(const SlGit::Repo &lrepo, const std::string &begin,
	     const std::string &end)
{
	auto revsOpt = lrepo.revWalkCreate();
	if (!revsOpt)
		throw std::runtime_error("cannot create rev walk: " + lrepo.lastError().first);
	auto revs = std::move(*revsOpt);
	// TODO unneeded?
	if (revs.sorting(GIT_SORT_TIME | GIT_SORT_TOPOLOGICAL))
		throw std::runtime_error("cannot set sorting: " + lrepo.lastError().first);

	if (!begin.empty() && !revs.hide(begin))
		throw std::runtime_error("cannot find begin commit " + begin + ": " +
					 lrepo.lastError().first);
	if (!revs.push(end))
		throw std::runtime_error("cannot find end commit " + end + ": " +
					 lrepo.lastError().first);

	Clr(Clr::CYAN) << begin << ".." << end;
	while (auto commit = revs.next()) {
		if (commit->parentCount() > 1)
			continue;
		if (commit->idStr() == bigBang)
			break;
		handleCommit(lrepo, *commit);
		//std::cerr << '\t' << commit->idStr() << '\n';
	}
}

} // namespace

int main()
{
	try {
		std::filesystem::path linux{"/home/xslaby/linux"};
		const auto repo = SlGit::Repo::open(linux);
		if (!repo)
			throw std::runtime_error("cannot open " + linux.string());
		between(*repo, "", "v3.0");
	} catch (const std::runtime_error &e) {
		Clr(std::cerr, Clr::RED) << e.what();
		return EXIT_FAILURE;
	}

	return 0;
}
