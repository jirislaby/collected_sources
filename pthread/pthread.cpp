#include <iostream>
#include <cstring>
#include <sstream>
#include <vector>

#include <err.h>
#include <pthread.h>

#include <sys/wait.h>

#include "../Holder.h"

namespace {

#if 1
auto constinit FORKERS = 10U;
auto constinit THREADS = 20U;
auto constinit LOOPS = 200000000U;
auto constinit NOPS = 1000U;
#else
auto constinit FORKERS = 2U;
auto constinit THREADS = 10U;
auto constinit LOOPS = 10U;
auto constinit NOPS = 1000U;
#endif

using ThHolder = Holder<pthread_t, decltype([](pthread_t t) { pthread_join(t, NULL); } )>;
using ProcHolder = Holder<pid_t, decltype([](pid_t pid) { waitpid(pid, NULL, 0); } )>;

void *thr(void *data)
{
	unsigned th_num = reinterpret_cast<unsigned long>(data);
	auto proc_num = th_num >> 16;
	th_num &= 0xffff;

	std::stringstream ss;

	ss << "thread " << proc_num << '/' << th_num << " starts\n";
	std::cout << ss.str();

	ss.str(std::string());
	ss << static_cast<char>('A' + proc_num) << static_cast<char>('a' + th_num);

	for (auto a = 0UL; a < LOOPS; a++) {
		for (auto a = 0U; a < NOPS; ++a)
			asm volatile("nop" ::: "memory");
		sched_yield();
		if (!(a % 100000))
			std::cout << ss.view() << std::flush;

	}

	ss.str(std::string());
	ss << "thread " << proc_num << '/' << th_num << " done\n";
	std::cout << ss.str();

	return nullptr;
}

void make_threads(unsigned process)
{
	std::vector<ThHolder> th;

#if 0
	cpu_set_t cpus;
	CPU_ZERO(&cpus);
	CPU_SET(0, &cpus);
	pthread_attr_t attr;

	int rv = pthread_attr_init(&attr);
	if (rv)
		errx(1, "pthread_attr_init: %s", strerror(rv));


	rv = pthread_attr_setaffinity_np(&attr, sizeof(cpus), &cpus);
	if (rv)
		errx(1, "pthread_attr_setschedpolicy: %s", strerror(rv));

	rv = pthread_attr_setschedpolicy(&attr, SCHED_FIFO);
	if (rv)
		errx(1, "pthread_attr_setschedpolicy: %s", strerror(rv));

	rv = pthread_attr_setinheritsched(&attr, PTHREAD_EXPLICIT_SCHED);
	if (rv)
		errx(1, "pthread_attr_setinheritsched: %s", strerror(rv));
#endif

	for (auto a = 0U; a < THREADS; a++) {
		pthread_t p;
#if 0
		struct sched_param param = {
			.sched_priority = static_cast<int>((a + 1) * 1),
		};
		rv = pthread_attr_setschedparam(&attr, &param);
		if (rv)
			errx(1, "pthread_attr_setschedparam: %s", strerror(rv));
#endif
		auto rv = pthread_create(&p, nullptr/*&attr*/, thr, reinterpret_cast<void *>((process << 16) | a));
		if (rv)
			errx(1, "pthread_create: %s", strerror(rv));

		th.emplace_back(p);
	}

	//pthread_attr_destroy(&attr);
}

} // namespace


int main()
{
	std::vector<ProcHolder> forkers;

	for (auto a = 0U; a < FORKERS; a++) {
		switch (auto pid = fork()) {
		case 0:
			make_threads(a);
			return 0;
		case -1:
			err(1, "DIE fork of %d'th forker", a);
			break;
		default:
			forkers.emplace_back(pid);
			break;
		}
	}

	return 0;
}

