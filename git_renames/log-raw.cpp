#include <iostream>

#include <sl/git/Buf.h>
#include <sl/git/Git.h>
#include <sl/git/Helpers.h>
#include <sl/git/Misc.h>
#include <sl/helpers/Color.h>
#include <sl/helpers/Misc.h>
#include <sl/helpers/Process.h>
#include <sl/helpers/PtrStore.h>
#include <sl/helpers/String.h>

using Clr = SlHelpers::Color;

namespace {

const std::string bigBang {"1da177e4c3f41524e886b7f1b8a0c1fc7321cac2"};

#if 0
void handleCommit(const SlGit::Repo &lrepo,
		  const SlGit::Commit &commit)
{
	const auto sha = commit.idStr();
	Clr(Clr::GREEN) << sha.substr(0, 12) << ' ' << commit.summary();
	git_diff_options opts GIT_DIFF_OPTIONS_INIT;
	opts.flags |= GIT_DIFF_DISABLE_PATHSPEC_MATCH;
	auto diff = lrepo.diff(*commit.parent(), commit, &opts);
	if (!diff)
		throw std::runtime_error("cannot get a diff for " + commit.idStr() + ": " +
					 lrepo.lastError());

	Clr(std::cerr, Clr::GREEN) << "\tfindSimilar";
	if (!diff->findSimilar())
		throw std::runtime_error("cannot find renames in a diff: " +
					 lrepo.lastError());

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
		throw std::runtime_error("diff failed: " + lrepo.lastError());
}

void between(const SlGit::Repo &lrepo, const std::string &begin,
	     const std::string &end)
{
	auto revsOpt = lrepo.revWalkCreate();
	if (!revsOpt)
		throw std::runtime_error("cannot create rev walk: " + lrepo.lastError());
	auto revs = std::move(*revsOpt);
	// TODO unneeded?
	if (!revs.sorting(GIT_SORT_TIME | GIT_SORT_TOPOLOGICAL))
		throw std::runtime_error("cannot set sorting: " + lrepo.lastError());

	if (!begin.empty() && !revs.hide(begin))
		throw std::runtime_error("cannot find begin commit " + begin + ": " +
					 lrepo.lastError());

	if (!revs.push(end))
		throw std::runtime_error("cannot find end commit " + end + ": " +
					 lrepo.lastError());

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

#elif 0

SlGit::Commit getCommit(const SlGit::Repo &lrepo, const std::string &id)
{
	auto obj = lrepo.revparseSingle(id.empty() ? bigBang : id);
	if (std::holds_alternative<SlGit::Commit>(obj))
		return std::move(std::get<SlGit::Commit>(obj));
	else if (auto tag = std::get_if<SlGit::Tag>(&obj)) {
		auto dest = tag->peel();
		if (std::holds_alternative<SlGit::Commit>(dest))
			return std::move(std::get<SlGit::Commit>(dest));;
	}

	throw std::runtime_error("cannot obtain commit for " + id + ": " +
				 lrepo.lastError());


}

void between(const SlGit::Repo &lrepo, const std::string &begin,
	     const std::string &end)
{
	auto diff = lrepo.diff(getCommit(lrepo, begin), getCommit(lrepo, end));
	if (!diff)
		throw std::runtime_error("cannot diff " + lrepo.lastError());

	git_diff_find_options opts GIT_DIFF_FIND_OPTIONS_INIT;
	opts.flags = GIT_DIFF_FIND_RENAMES;
	opts.rename_limit = ~0U;
	opts.rename_threshold = 36;

	SlHelpers::Measure mAll;

	Clr(std::cerr, Clr::GREEN) << "findSimilar";
	SlHelpers::Measure m;
	if (!diff->findSimilar(&opts))
		throw std::runtime_error("cannot find similar in diff " + lrepo.lastError());
	Clr(std::cerr, Clr::GREEN) << "findSimilar took " << m.lap();

	unsigned count = 0;
	SlGit::Diff::ForEachCB cb {
		.file = [&count](const git_diff_delta &delta, float) {
			if (delta.status != GIT_DELTA_RENAMED)
				return 0;
			Clr(Clr::GREEN) << ':' << std::oct << delta.old_file.mode << ' ' <<
					   delta.new_file.mode << std::dec << ' ' <<
					   SlGit::Helpers::oidToStr(delta.old_file.id).substr(0, 12) << ' ' <<
					   SlGit::Helpers::oidToStr(delta.new_file.id).substr(0, 12) <<
					   " R" << std::setw(3) << std::setfill('0') << delta.similarity << '\t' <<
					   delta.old_file.path << '\t' << delta.new_file.path;
			count++;
			return 0;
		},
	};

	Clr(std::cerr, Clr::GREEN) << "foreach";
	m.reset();
	if (diff->forEach(cb))
		throw std::runtime_error("cannot walk the diff " + lrepo.lastError());

	Clr(std::cerr, Clr::GREEN) << "foreach took " << m.lap() << " count=" << count;
	Clr(std::cerr, Clr::GREEN) << "all took " << mAll.lap();
}
#else
void between(const SlGit::Repo &lrepo, std::string begin, const std::string &end)
{
	SlHelpers::Measure mAll;
	std::vector<std::string> args { "-C", lrepo.workDir(), "log", "-M30", "-l0", "--oneline",
				"--no-merges", "--raw", "--diff-filter=R",
				"--format=",
				std::move(begin.append("..").append(end)) };
	SlHelpers::Process p;
	p.spawn("/usr/bin/git", args, true);

	/*std::ostringstream oss;
	SlHelpers::String::join(oss, args, " ");
	Clr(Clr::GREEN) << "args: " << oss.str();*/

	SlHelpers::PtrStore<FILE, decltype([](FILE *f) { if (f) fclose(f); })> stream;
	stream.reset(fdopen(p.readPipe(), "r"));
	if (!stream)
		throw std::runtime_error("cannot open stdout of git");

	SlHelpers::PtrStore<char, decltype([](char *ptr) { free(ptr); })> lineRaw;
	size_t len = 0;

	struct RenameInfo {
	    std::string path;
	    unsigned similarity;
	};
	using Map = std::unordered_map<std::string, RenameInfo, SlHelpers::String::Hash,
		SlHelpers::String::Eq>;
	Map renames;

	while (getline(lineRaw.ptr(), &len, stream.get()) != -1) {
		auto line = lineRaw.str();
		if (line.empty() || line.front() != ':')
			throw std::runtime_error("bad line: " + std::string(line));

		//Clr(Clr::GREEN) << line;

		auto vec = SlHelpers::String::splitSV(line, " \t\n");
		if (vec.size() < 7)
			throw std::runtime_error("bad formatted line: " + std::string(line));

		unsigned int similarity{};
		std::from_chars(vec[4].data() + 1, vec[4].data() + vec[4].size(), similarity);
		if (!similarity)
			throw std::runtime_error("bad rename: " + std::string(vec[4]));
		auto oldFile = vec[5];
		auto newFile = vec[6];

		//Clr(Clr::GREEN) << "\tR=" << similarity << ' ' << oldFile << " -> " << newFile;

		auto it = renames.find(newFile);
		if (it != renames.end()) {
			auto final = std::move(it->second);
			renames.erase(it);

			// do not store reverted and back and forth renames
			if (oldFile != final.path) {
				final.similarity *= similarity;
				final.similarity /= 100U;
				renames.emplace(oldFile, std::move(final));
			}
		} else {
			renames.emplace(oldFile, RenameInfo{std::string(newFile), similarity});
		}
	}

	for (const auto &e: renames)
		Clr(Clr::GREEN) << "R" << e.second.similarity << '\t' << e.first << '\t' <<
				   e.second.path;

	if (!feof(stream.get()) || ferror(stream.get()))
		throw std::runtime_error(std::string("not completely read: ") + strerror(errno));

	if (!p.waitForFinished())
		throw std::runtime_error("cannot wait for git");

	if (p.signalled())
		throw std::runtime_error("git crashed");
	if (auto e = p.exitStatus())
		throw std::runtime_error("git exited with " + std::to_string(e));

	Clr(Clr::GREEN) << "all took " << mAll.lap();
}

#endif

} // namespace

int main()
{
	try {
		std::filesystem::path linux{"/home/xslaby/linux"};
		const auto repo = SlGit::Repo::open(linux);
		if (!repo)
			throw std::runtime_error("cannot open " + linux.string());
		between(*repo, "v6.11", "v6.12");
	} catch (const std::runtime_error &e) {
		Clr(std::cerr, Clr::RED) << e.what();
		return EXIT_FAILURE;
	}

	return 0;
}
