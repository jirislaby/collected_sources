#include <fstream>
#include <string>
#include <unistd.h>
#include <vector>
#include <sys/wait.h>

static void loop(std::ofstream &ofs, size_t loops, const std::string &val)
{
	for (unsigned i = 0; i < loops; ++i) {
		ofs << val;
		ofs.flush();
	}
}

int main(int, char **argv)
{
	std::ofstream ofs;
	ofs.open("/sys/kernel/debug/irqsim/gen_irq");
	if (!ofs.good())
		return EXIT_FAILURE;
	std::vector<pid_t> children;
	for (unsigned i = 0; i < 20; ++i) {
		auto child = fork();
		if (!child) {
			unsigned irq = (i % 2) ? i/2 : 127 - i/2;
			loop(ofs, std::stol(argv[1]), std::to_string(irq));
			ofs.close();
			return 0;
		}
		children.push_back(child);
	}
	ofs.close();
	for (const auto &ch : children)
		waitpid(ch, NULL, 0);
	return 0;
}
