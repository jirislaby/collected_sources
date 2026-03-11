#include <array>
#include <cstdlib>
#include <err.h>
#include <unistd.h>

#include <sys/wait.h>

#include "Holder.h"

using ProcHolder = Holder<pid_t, decltype([](pid_t pid) { waitpid(pid, NULL, 0); } )>;

#if 1
const constinit auto FORKERS = 15U;
const constinit auto REP = 100000U;
const constinit auto SUBFORKERS = 100U;
#else
const constinit auto FORKERS = 1U;
const constinit auto REP = 20U;
const constinit auto SUBFORKERS = 1U;
#endif

namespace {

#pragma GCC diagnostic ignored "-Wclobbered"

void child()
{
	for (auto i = 0U; i < REP; ++i) {
		std::array<ProcHolder, SUBFORKERS> forkers;

		for (auto a = 0U; a < forkers.size(); a++) {
			switch (auto pid = vfork()) {
			case 0:
				_exit(0);
			case -1:
				err(1, "DIE vfork of %d'th forker", a);
				break;
			default:
				forkers[a].set(pid);
				break;
			}
		}
	}
}

}

int main()
{
	std::array<ProcHolder, FORKERS> forkers;

	for (auto a = 0U; a < forkers.size(); a++) {
		switch (auto pid = fork()) {
		case 0:
			child();
			return 0;
		case -1:
			err(1, "DIE fork of %d'th forker", a);
			break;
		default:
			forkers[a].set(pid);
			break;
		}
	}

	return 0;
}
