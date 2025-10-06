#include <array>
#include <string>

#include <err.h>
#include <fcntl.h>
#include <unistd.h>

#include <sys/ioctl.h>

#include <linux/fs.h>

namespace {

[[maybe_unused]] std::array<char, 81920> make_data() {
    std::array<char, 81920> a{};
    std::fill(a.begin(), a.end(), 'X');
    return a;
}

[[noreturn]] void run(bool create, unsigned i = 0)
{
	auto file = "tmp_" + std::to_string(i);

	if (create) {
		static auto data = make_data();
		auto fd = open(file.c_str(), O_WRONLY|O_CREAT|O_TRUNC|O_CLOEXEC, 0644);
		if (fd < 0)
			err(1, "open1");
		if (write(fd, data.data(), data.size()) != (ssize_t)data.size())
			err(1, "write");
		close(fd);
	}

	unsigned flags = FS_NODUMP_FL;
	auto fd2 = open(file.c_str(), O_RDONLY|O_NONBLOCK|O_NOFOLLOW);
	if (fd2 < 0)
		err(1, "open2");
	if (ioctl(fd2, FS_IOC_SETFLAGS, &flags) == -1)
		err(1, "ioctl");
	close(fd2);

	exit(0);
}

}

int main(int, char **argv)
{
	const bool create = std::atoi(argv[1]);
	run(create);
	return 0;
}
