#include <iostream>
#include <cstring>
#include <sstream>
#include <vector>

#include <err.h>
#include <pthread.h>

class ThHolder {
public:
	ThHolder() = delete;

	ThHolder(pthread_t t) : t(t) {}

	ThHolder(ThHolder &&o) : t(o.t) {
		o.invalidate();
	}

	~ThHolder() {
		if (valid)
			pthread_join(t, NULL);
		invalidate();
	}

	void invalidate() {
		t = ~0UL;
		valid = false;
	}

	operator pthread_t () { return t; }
private:
	bool valid = true;
	pthread_t t;
};

static void *thr(void *data)
{
	unsigned th_num = (unsigned long)data;
	std::stringstream ss;

	ss << "thread " << th_num << " starts\n";
	std::cout << ss.str();

	for (unsigned long a = 0; a < 2000000000; a++)
		asm volatile("" ::: "memory");

	ss.str(std::string());
	ss << "thread " << th_num << " done\n";
	std::cout << ss.str();

	return nullptr;
}

#define THREADS (10)

int main()
{
	std::vector<ThHolder> th;
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

	for (unsigned a = 0; a < THREADS; a++) {
		pthread_t p;

		struct sched_param param = {
			.sched_priority = static_cast<int>((a + 1) * 1),
		};
		rv = pthread_attr_setschedparam(&attr, &param);
		if (rv)
			errx(1, "pthread_attr_setschedparam: %s", strerror(rv));

		rv = pthread_create(&p, &attr, thr, (void *)static_cast<unsigned long>(a));
		if (rv)
			errx(1, "pthread_create: %s", strerror(rv));

		th.push_back(ThHolder(p));
	}

	pthread_attr_destroy(&attr);

	return 0;
}

