#include <cxxopts.hpp>
#include <iostream>
#include <set>

#include <sqlite3.h>

#include <sl/git/Buf.h>
#include <sl/git/Git.h>
#include <sl/git/Misc.h>
#include <sl/kerncvs/Branches.h>
#include <sl/helpers/Color.h>
#include <sl/helpers/Misc.h>
#include <sl/helpers/String.h>
#include <sl/sqlite/SQLConn.h>

using Clr = SlHelpers::Color;

namespace {

const std::filesystem::path dbName {"changes.sqlite"};
const std::string bigBang {"1da177e4c3f41524e886b7f1b8a0c1fc7321cac2"};

class SQLConn : public SlSqlite::SQLConn {
protected:
	bool createDB() override {
		static const Tables tables {
			{ "tag", {
				"id INTEGER PRIMARY KEY",
				"name TEXT NOT NULL UNIQUE",
			}},
			{ "branch", {
				"id INTEGER PRIMARY KEY",
				"name TEXT NOT NULL UNIQUE",
				"ancestors INTEGER NOT NULL",
				"tag INTEGER NOT NULL REFERENCES tag(id) ON DELETE CASCADE",
			}},
			{ "sha", {
				"id INTEGER PRIMARY KEY",
				"name TEXT NOT NULL UNIQUE",
			}},
			{ "file", {
				"id INTEGER PRIMARY KEY",
				"name TEXT NOT NULL UNIQUE"
			}},
			{ "backport", {
				"id INTEGER PRIMARY KEY",
				"sha INTEGER NOT NULL REFERENCES sha(id) ON DELETE CASCADE",
				"branch INTEGER NOT NULL REFERENCES branch(id) ON DELETE CASCADE",
			}},
			{ "change", {
				"id INTEGER PRIMARY KEY",
				"score INTEGER",
				"sha INTEGER NOT NULL REFERENCES sha(id) ON DELETE CASCADE",
				"_from INTEGER REFERENCES file(id) ON DELETE CASCADE",
				"_to INTEGER REFERENCES file(id) ON DELETE CASCADE",
				"tag INTEGER REFERENCES tag(id) ON DELETE CASCADE",
			}},
		};

		static const Views views {
			{ "branch_sort_view",
				"SELECT branch.name AS branch, tag.name AS tag, branch.ancestors, "
					"branch.id AS branch_id, tag.id AS tag_id "
				"FROM branch "
				"LEFT JOIN tag ON tag.id = branch.tag "
				"ORDER BY branch.tag DESC, branch.ancestors ASC;"
			},
			{ "base_added_view",
				"SELECT branch.name AS branch, file.name AS file, sha.name AS sha "
				"FROM file "
				"JOIN change ON change._to = file.id AND change._from IS NULL "
				"JOIN sha ON sha.id = change.sha "
				"JOIN tag ON tag.id = change.tag "
				"JOIN branch ON branch.tag = tag.id;"
			},
			{ "base_renamed_view",
				"SELECT branch.name AS branch, file.name AS file, "
					"new_file.name AS new_file, sha.name AS sha "
				"FROM file "
				"JOIN change ON change._to IS NOT NULL AND change._from = file.id "
				"JOIN file AS new_file ON change._to = new_file.id "
				"JOIN sha ON sha.id = change.sha "
				"JOIN tag ON tag.id = change.tag "
				"JOIN branch ON branch.tag = tag.id;"
			},
			{ "base_removed_view",
				"SELECT branch.name AS branch, file.name AS file, sha.name AS sha "
				"FROM file "
				"JOIN change ON change._to IS NULL AND change._from = file.id "
				"JOIN sha ON sha.id = change.sha "
				"JOIN tag ON tag.id = change.tag "
				"JOIN branch branch ON branch.tag = tag.id;"
			},
			{ "backports_added_view",
				"SELECT branch.name AS branch, file.name AS file, sha.name AS sha "
				"FROM backport "
				"JOIN branch ON branch.id = backport.branch "
				"JOIN change change ON change.sha = backport.sha "
				"JOIN sha sha ON sha.id = change.sha "
				"JOIN file ON file.id = change._from AND change._to IS NULL;"
			},
			{ "backports_renamed_view",
				"SELECT branch.name AS branch, file.name AS file, "
					"new_file.name AS new_file, sha.name AS sha "
				"FROM backport "
				"JOIN branch ON branch.id = backport.branch "
				"JOIN change ON change.sha = backport.sha "
				"JOIN sha ON sha.id = change.sha "
				"JOIN file ON file.id = change._from AND change._to IS NOT NULL "
				"JOIN file new_file ON change._to = new_file.id;"
			},
			{ "backports_removed_view",
				"SELECT branch.name AS branch, file.name AS file, sha.name AS sha "
				"FROM backport "
				"JOIN branch ON branch.id = backport.branch "
				"JOIN change ON change.sha = backport.sha "
				"JOIN sha ON sha.id = change.sha "
				"JOIN file ON file.id = change._from AND change._to IS NULL;"
			},
			{ "added_view",
				"SELECT branch, file, sha, 'base' AS source "
					"FROM base_added_view "
				"UNION ALL "
				"SELECT branch, file, sha, 'backport' AS source "
					"FROM backports_added_view;"
			},
			{ "renamed_view",
				"SELECT branch, file, new_file, sha, 'base' AS source "
					"FROM base_renamed_view "
				"UNION ALL "
				"SELECT branch, file, new_file, sha, 'backport' AS source "
					"FROM backports_renamed_view;"
			},
			{ "removed_view",
				"SELECT branch, file, sha, 'base' AS source "
					"FROM base_removed_view "
				"UNION ALL "
				"SELECT branch, file, sha, 'backport' AS source "
					"FROM backports_removed_view;"
			},
		};

		return createTables(tables) && createViews(views);
	}

	virtual bool prepDB() override {
		return	prepareStatement("INSERT INTO tag(name) VALUES (:tag)", insTag) &&
			prepareStatement("INSERT INTO branch(name, ancestors, tag) "
					 "SELECT :name, :ancestors, id FROM tag WHERE name = :tag",
					 insBranch) &&
			prepareStatement("INSERT OR IGNORE INTO file(name) VALUES (:file)",
					 insFile) &&
			prepareStatement("INSERT INTO sha(name) VALUES (:sha)", insSHA) &&
			prepareStatement("INSERT INTO change(sha, score, _from, _to, tag) "
					 "VALUES ("
					 "(SELECT id FROM sha WHERE name = :sha),"
					 ":score,"
					 "(SELECT id FROM file WHERE name = :from_file),"
					 "(SELECT id FROM file WHERE name = :to_file),"
					 "(SELECT id FROM tag WHERE name = :tag))",
					 insChange) &&
			prepareStatement("INSERT INTO backport(sha, branch) VALUES ("
					 "(SELECT id FROM sha WHERE name = :sha), "
					 "(SELECT id FROM branch WHERE name = :branch))",
					 insBackport);
	}
public:
	bool insertTag(const std::string &tag) const {
		return true;
		return insert(insTag, { { ":tag", tag } });
	}
	bool insertBranch(const std::string &name, int ancestors, const std::string &tag) const {
		return true;
		return insert(insBranch, {
			      { ":name", name },
			      { ":ancestors", ancestors },
			      { ":tag", tag },
		});
	}
	bool insertFile(const std::string &file) const {
		return true;
		return insert(insFile, { { ":file", file } });
	}
	bool insertSHA(const std::string &sha) const {
		return true;
		return insert(insSHA, { { ":sha", sha } });
	}
	bool insertChange(const std::string &sha, const BindVal &score, const BindVal &fromFile,
			  const BindVal &toFile, const std::string &tag) const {
		return true;
		return insert(insChange, {
			      { ":sha", sha },
			      { ":score", score },
			      { ":from_file", fromFile },
			      { ":to_file", toFile },
			      { ":tag", tag },
		});
	}
	bool insertBackport(const std::string &sha, const std::string &branch) const {
		return true;
		return insert(insBackport, {
			      { ":sha", sha },
			      { ":branch", branch },
		});
	}


private:
	SlSqlite::SQLStmtHolder insTag;
	SlSqlite::SQLStmtHolder insBranch;
	SlSqlite::SQLStmtHolder insFile;
	SlSqlite::SQLStmtHolder insSHA;
	SlSqlite::SQLStmtHolder insChange;
	SlSqlite::SQLStmtHolder insBackport;
};

void handleSHA(const SlGit::Repo &repo, const std::string &sha, bool renames)
{
	const auto commit = repo.commitRevparseSingle(sha);
	if (!commit)
		throw std::runtime_error("cannot find commit " + sha + ": " +
					 repo.lastError().first);

	if (commit->parentCount() > 1)
		throw std::runtime_error(commit->idStr() + " is a merge commit");

	const auto diff = repo.diff(*commit, *commit->parent());

	if (renames && diff->findSimilar())
		throw std::runtime_error("diff findSimilar failed: " + repo.lastError().first);

	struct SlGit::Diff::ForEachCB cb {
		.file = [](const git_diff_delta &delta, float) -> int {
			Clr(Clr::BLUE) << "FILE ======= " << delta.old_file.path << " -> " <<
					  delta.new_file.path << ' ' << delta.similarity << ' ' <<
					  delta.status;
			return 0;
		},
		.hunk = [](const git_diff_delta &, const git_diff_hunk &hunk) -> int {
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
		throw std::runtime_error("diff failed: " + repo.lastError().first);

	const auto buf = diff->toBuf(GIT_DIFF_FORMAT_PATCH);
	if (!buf)
		throw std::runtime_error("diff to buf failed: " + repo.lastError().first);

	//std::cout << buf->sv();
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

std::string extract_srcversion(const std::string &content)
{
	std::istringstream stream(content);
	for (std::string l; std::getline(stream, l);)
		if (l.starts_with("SRCVERSION="))
			return l.substr(l.find('=') + 1);
	return {};
}

auto get_tags_from_ksource_tree(const SlKernCVS::Branches::BranchesList &branches,
				const SlGit::Repo &repo)
{
	std::unordered_map<std::string, std::string> ret;

	for (const auto &b: branches) {
		const auto config = repo.catFile("origin/" + b, "rpm/config.sh");
		if (!config) {
			Clr(std::cerr, Clr::RED) << "cannot obtain config for " << b;
			continue;
		}
		ret[b] = extract_srcversion(*config);
	}

	return ret;
}

void fetch_root_tree_files(const SQLConn &sql, const SlGit::Repo &lrepo, const std::string &tag)
{
	auto tree_index = lrepo.index();
	if (!tree_index)
		throw std::runtime_error("no index: " + lrepo.lastError().first);

	const auto commit = lrepo.commitRevparseSingle(bigBang);
	if (!commit)
		throw std::runtime_error("no big bang commit: " + lrepo.lastError().first);

	tree_index->readTree(*commit->tree());

	if (!sql.insertSHA(bigBang))
		throw std::runtime_error("cannot insert SHA '" + bigBang + "': " +
					 sql.lastError());

	sql.begin();
	for (const auto &e: *tree_index) {
		const std::string path(e.path);
		//std::cout << "\t" << path << '\n';
		if (!sql.insertFile(path))
			throw std::runtime_error("cannot insert file " + path + ": " +
						 sql.lastError());
		if (!sql.insertChange(bigBang, std::monostate(), std::monostate(), path, tag))
			throw std::runtime_error("cannot insert change: " + sql.lastError());
	}
	sql.end();
}

void handleCommit(const SQLConn &sql, const SlGit::Repo &lrepo, const std::string &end,
		  const SlGit::Commit &commit)
{
	const auto sha = commit.idStr();
	if (!sql.insertSHA(sha))
		throw std::runtime_error("cannot insert sha " + sha + ": " + sql.lastError());
	return;
	Clr(std::cerr, Clr::GREEN) << commit.idStr().substr(0, 12);
	auto diff = lrepo.diff(*commit.parent(), commit);
	if (!diff)
		throw std::runtime_error("cannot get a diff for " + commit.idStr() + ": " +
					 lrepo.lastError().first);
	Clr(std::cerr, Clr::GREEN) << "\tfindSimilar";
	if (diff->findSimilar())
		throw std::runtime_error("cannot find renames in a diff: " +
					 lrepo.lastError().first);

	struct SlGit::Diff::ForEachCB cb {
		.file = [&sql, &sha, &end](const git_diff_delta &delta, float) -> int {
			switch (delta.status) {
			case GIT_DELTA_ADDED: {
				std::string newFile(delta.new_file.path);
				if (!sql.insertFile(newFile))
					throw std::runtime_error("cannot insert file: " +
								 sql.lastError());
				if (!sql.insertChange(sha, std::monostate(), std::monostate(),
						      newFile, end))
					throw std::runtime_error("cannot insert change A " + sha +
								 ": " + sql.lastError());
				break;
			}
			case GIT_DELTA_DELETED: {
				std::string oldFile(delta.old_file.path);
				if (!sql.insertFile(oldFile))
					throw std::runtime_error("cannot insert file: " +
								 sql.lastError());
				if (!sql.insertChange(sha, std::monostate(), oldFile,
						      std::monostate(), end))
					throw std::runtime_error("cannot insert change D " + sha +
								 ": " + sql.lastError());
				break;
			}
			case GIT_DELTA_RENAMED: {
				std::string oldFile(delta.old_file.path);
				if (!sql.insertFile(oldFile))
					throw std::runtime_error("cannot insert file: " +
								 sql.lastError());
				std::string newFile(delta.new_file.path);
				if (!sql.insertFile(newFile))
					throw std::runtime_error("cannot insert file: " +
								 sql.lastError());
				if (!sql.insertChange(sha, delta.similarity, oldFile, newFile, end))
					throw std::runtime_error("cannot insert change R " + sha +
								 ": " + sql.lastError());
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

void between(const SQLConn &sql, const SlGit::Repo &lrepo, const std::string &begin,
	     const std::string &end)
{
	auto revsOpt = lrepo.revWalkCreate();
	if (!revsOpt)
		throw std::runtime_error("cannot create rev walk: " + lrepo.lastError().first);
	auto revs = std::move(*revsOpt);

	if (!begin.empty() && !revs.hide(begin))
		throw std::runtime_error("cannot find begin commit " + begin + ": " +
					 lrepo.lastError().first);
	if (!revs.push(end))
		throw std::runtime_error("cannot find end commit " + end + ": " +
					 lrepo.lastError().first);

	Clr(Clr::CYAN) << begin << ".." << end;
	static unsigned cnt = 0;
	while (auto commit = revs.next()) {
		if (cnt > 1000)
			break;
		if (commit->parentCount() > 1)
			continue;
		if (commit->idStr() == bigBang)
			break;
		handleCommit(sql, lrepo, end, *commit);
		//std::cerr << '\t' << commit->idStr() << '\n';
		cnt++;
	}
	Clr(Clr::CYAN) << cnt;
}

std::optional<std::string> get_hash(const std::string &fc, const SlGit::Repo &lrepo)
{
	static constexpr const std::string_view gCommit{"Git-commit:"};
	static constexpr const std::string_view hexNumber{"0123456789abcdefABCDEF"};
	std::istringstream iss{fc};
	for (std::string line; std::getline(iss, line); ) {
		auto start = line.find(gCommit);
		if (start == line.npos)
			continue;

		auto startHex = line.find_first_of(hexNumber, start + gCommit.size());
		if (startHex == line.npos) {
			Clr(std::cerr, Clr::YELLOW) << "bad line: " << line;
			continue;
		}
		auto endHex = line.find_first_not_of(hexNumber, startHex);

		auto sha = line.substr(startHex, endHex - startHex);
		if (sha.size() < 9) {
			Clr(std::cerr, Clr::YELLOW) << "bad sha in line: " << line;
			continue;
		}
		auto commit = lrepo.commitRevparseSingle(sha);
		if (commit)
			return commit->idStr();
	}
	return std::nullopt;
}

void get_commits_per_branch(const SQLConn &sql,
			    const SlKernCVS::Branches::BranchesList &branches,
			    const SlGit::Repo &krepo,
			    const SlGit::Repo &lrepo)
{
	for (const auto &b: branches) {
		auto commit = krepo.commitRevparseSingle("origin/" + b);
		if (!commit) {
			Clr(Clr::YELLOW) << "cannot parse " << b << ": " <<
					    krepo.lastError().first;
			continue;
		}
		auto tree = commit->tree();
		auto treeEntry = tree->treeEntryByPath("patches.suse/");
		if (!treeEntry) {
			Clr(Clr::YELLOW) << "cannot find patches.suse/ in " << b << ": " <<
					    krepo.lastError().first;
			continue;
		}

		auto patchesSUSE = krepo.treeLookup(*treeEntry);
		for (auto i = 0U; i < patchesSUSE->entryCount(); ++i) {
			const auto e = patchesSUSE->treeEntryByIndex(i);
			const auto content = e.catFile(krepo);
			if (!content)
				throw std::runtime_error("cannot read patches.suse/" + e.name() +
							 ": " + krepo.lastError().first);
			const auto hs = get_hash(*content, lrepo);
			if (!hs) {
				Clr(std::cerr, Clr::BLUE) << "=== no git-commit in origin/" << b << ":patches.suse/" << e.name();
				//Clr(std::cerr, Clr::BLUE) << content->substr(0, 1000);
				continue;
			}

			if (!sql.insertBackport(*hs, b) &&
					sql.lastErrorCodeExt() != SQLITE_CONSTRAINT_NOTNULL)
				throw std::runtime_error("cannot insert backport '" + *hs +
							 "' into branch '" + b + "': " +
							 sql.lastError());
		}
	}
}

void buildDB()
{
	SQLConn sql;
	if (!sql.open(dbName, SlSqlite::CREATE))
		throw std::runtime_error("cannot create db " + dbName.string() + ": " +
					 sql.lastError());

	auto branchesOpt = SlKernCVS::Branches::getBuildBranches();
	if (!branchesOpt)
		throw std::runtime_error("cannot obtain branches");

	const auto kpath = SlHelpers::Env::get<std::filesystem::path>("KSOURCE_GIT");
	if (!kpath || kpath->empty())
		throw std::runtime_error("cannot get KSOURCE_GIT");

	const auto krepo = SlGit::Repo::open(*kpath);
	if (!krepo)
		throw std::runtime_error("cannot open KSOURCE_GIT (" + kpath->string() + "): " +
					 SlGit::Repo::lastError().first);

	auto branches = std::move(*branchesOpt);
	branches.push_back("master");
	auto tags = get_tags_from_ksource_tree(branches/*.keys()*/, *krepo);
	for (const auto &k: tags)
		Clr(Clr::GREEN) << k.first << " -> " << k.second;
	std::set<std::string, SlHelpers::CmpVersions> uniq_tags_no_v;
	std::transform(tags.cbegin(), tags.cend(),
		       std::inserter(uniq_tags_no_v, uniq_tags_no_v.end()),
		       [](const auto &p) { return p.second; });
	std::vector<std::string> uniq_tags;
	std::transform(uniq_tags_no_v.cbegin(), uniq_tags_no_v.cend(),
		       std::back_inserter(uniq_tags),
		       [](const auto &p) { return 'v' + p; });
	for (const auto &t: uniq_tags) {
		Clr(Clr::GREEN) << t;
		if (!sql.insertTag(t))
			throw std::runtime_error("cannot insert tag '" + t + "': " +
						 sql.lastError());
	}
	uniq_tags.push_back("linus/master");
	//const auto number_of_ancestors = transitive_closure(branches);
	for (const auto &t: tags)
		if (!sql.insertBranch(t.first, 0/*number_of_ancestors.at(t.first)*/, "v" + t.second))
			throw std::runtime_error("cannot insert branch '" + t.first + "': " +
						 sql.lastError());

	const auto lpath = SlHelpers::Env::get<std::filesystem::path>("LINUX_GIT");
	if (!lpath || lpath->empty())
		throw std::runtime_error("cannot get LINUX_GIT");
	const auto lrepo = SlGit::Repo::open(*lpath);
	if (!lrepo)
		throw std::runtime_error("cannot open LINUX_GIT (" + lpath->string() + "): " +
					 SlGit::Repo::lastError().first);

	fetch_root_tree_files(sql, *lrepo, uniq_tags[0]);

	sql.begin();
	between(sql, *lrepo, "", uniq_tags[0]);
	/*for (auto i = 0U; i < uniq_tags.size() - 1; ++i)
		between(sql, *lrepo, uniq_tags[i], uniq_tags[i + 1]);*/

	get_commits_per_branch(sql, branches, *krepo, *lrepo);
	sql.end();
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
		exc(shas, renames);

		buildDB();
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

#if 0
CVE_PATTERN = re.compile(r"CVE-[0-9]{4}-[0-9]{4,}")
MERGES_PATTERN = re.compile(r'[ \t]merge:-?([^ ]+)')
BRANCH_BLACKLIST = ['vanilla', 'linux-next']
BLACKLIST = 'Dell Inc.,XPS 13 9300' # weird file with spaces nobody gives a fig about

def do_query(query):
    with sqlite3.connect(DB_NAME) as conn:
        conn.execute('PRAGMA foreign_keys = ON')
        cursor = conn.cursor()
        return [ x for x in cursor.execute(query) ]

def get_commits():
    return { x for (x,) in do_query('SELECT name FROM commits;') }

class Db:
    def __init__(self):
        self.tags_by_id =  { id: name for id, name in do_query('SELECT * FROM tags;') }
        self.commits_by_id = { id: name for id, name in do_query('SELECT * FROM commits;') }
        self.changes_by_id = { id: (from_id, to_id, commit_id, tag_id, score)
                               for id, from_id, to_id, commit_id, tag_id, score
                               in do_query('SELECT id, from_id, to_id, commit_id, tag_id, score FROM changes;') }

    def file_history(self, f):
        pivot = None
        for k, v in self.changes_by_id.items():
            if v[1] == f:
                pivot = k
                break
        print(self.commits_by_id[v[2]], " ", self.tags_by_id[v[3]])

def fetch_cves(cves, branch):
    path_to_repo = os.getenv('VULNS_GIT', None)
    if not path_to_repo:
        print("Cannnot get VULNS_GIT", file=sys.stderr)
        sys.exit(1)
    repo = git.Repository(path_to_repo)
    tree_index = git.Index()
    try:
        tree_index.read_tree(repo.revparse_single(branch).tree)
    except Exception as e:
        print(f'branch {branch} probably does not exist: {e}', file=sys.stderr)
        sys.exit(1)
    def path_contains_one_of(path, elements):
        for e in elements:
            if e in path:
                return True
        return False
    def get_cve(s):
        m = re.search(CVE_PATTERN, s)
        return m.group(0) if m else ''
    path_filtered_tree_data = [ t for t in tree_index if t.path.startswith('cve/') and t.path.endswith('.sha1') and path_contains_one_of(t.path, cves) ]
    published_data = [ (t.path, t.id) for t in path_filtered_tree_data if t.path.startswith('cve/published/') ]
    rejected_data = [ (t.path, t.id) for t in path_filtered_tree_data if t.path.startswith('cve/rejected/') ]
    return ( { get_cve(t[0]): repo[t[1]].data.decode('ascii').rstrip().split() for t in published_data },
             { get_cve(t[0]): repo[t[1]].data.decode('ascii').rstrip().split() for t in rejected_data })


#endif
