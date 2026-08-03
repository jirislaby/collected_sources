#include <cerrno>
#include <cstring>
#include <filesystem>
#include <memory>
#include <thread>
#include <unistd.h>

#include <linux/module.h>
#include <sys/syscall.h>

#include <sl/helpers/Color.h>
#include <sl/helpers/Exception.h>

using Clr = SlHelpers::Color;
using RunEx = SlHelpers::RuntimeException;
using SlHelpers::raise;

#ifndef NUM_MODULES
#define NUM_MODULES 1
#endif

namespace {

struct FileCloser {
	void operator()(FILE* f) const {
		if (f)
			std::fclose(f);
	}
};

constexpr const unsigned NUM_THREADS = NUM_MODULES;
constexpr const unsigned NUM_LOOPS = 1000;

void doOne(std::filesystem::path argv0, unsigned number)
{
	auto numberStr = std::to_string(number);
	std::string modName{"mod"};
	modName += numberStr;

	auto modPath = argv0.parent_path();
	modPath /= modName;
	modPath += ".ko";

	std::unique_ptr<FILE, FileCloser> mod(std::fopen(modPath.c_str(), "r"));
	if (!mod)
		RunEx("fopen failed: ") << modPath << ": " << strerror(errno) << raise;

	auto fileNo = fileno(mod.get());
	numberStr += ' ';

	for (auto i = 0U; i < NUM_LOOPS; ++i) {
		if (syscall(SYS_delete_module, modName.c_str(), 0) && errno != ENOENT)
			RunEx("delete_module failed: ") << strerror(errno) << raise;

		if (syscall(SYS_finit_module, fileNo, "", 0))
			RunEx("finit_module failed: ") << strerror(errno) << raise;
		if (!(i % 20)) {
			std::cout << numberStr << std::flush;
			usleep(1000 + 100 * number);
		}
	}
}

void handledOne(std::filesystem::path argv0, unsigned number)
{
	try {
		doOne(std::move(argv0), number);
	} catch (const std::exception &e) {
		Clr(std::cerr, Clr::RED) << e.what();
	}
}

} // namespace

int main(int, char **argv)
{
	std::jthread t[NUM_THREADS];
	unsigned i = 0;
	for (auto &t1 : t)
		t1 = std::jthread(handledOne, argv[0], i++);

	for (auto &t1 : t)
		t1.join();

	std::cout << '\n';
}
