#define _POSIX_C_SOURCE

#include <csignal>
#include <iostream>
#include <memory>
#include <ucontext.h>

#include <sys/ucontext.h>

namespace {

void dump_regs(const ucontext_t &ucontext)
{
	unsigned short cs, ds, es, fs, gs;
	asm("mov %%cs, %0" : "=r"(cs));
	asm("mov %%ds, %0" : "=r"(ds));
	asm("mov %%es, %0" : "=r"(es));
	asm("mov %%fs, %0" : "=r"(fs));
	asm("mov %%gs, %0" : "=r"(gs));
	auto gregs = ucontext.uc_mcontext.gregs;
	std::cerr << "=== dump registers ===\n";
	std::cerr << "CSGSFS=0x" << std::hex << gregs[REG_CSGSFS] << '\n';
	std::cerr << "CS=0x" << std::hex << cs << '\n';
	std::cerr << "DS=0x" << std::hex << ds << '\n';
	std::cerr << "ES=0x" << std::hex << es << '\n';
	std::cerr << "FS=0x" << std::hex << fs << '\n';
	std::cerr << "GS=0x" << std::hex << gs << '\n';
	std::cerr << "RIP=0x" << std::hex << gregs[REG_RIP] << '\n';
	std::cerr << "RSP=0x" << std::hex << gregs[REG_RSP] << '\n';
	std::cerr << "ERR=0x" << std::hex << gregs[REG_ERR] << '\n';
	std::cerr << "TRAPNO=0x" << std::hex << gregs[REG_TRAPNO] << '\n';
	std::cerr << "CR2=0x" << std::hex << gregs[REG_CR2] << '\n';
	std::cerr << "=== end dump ===\n";
}

void segv(int, siginfo_t *info, void *_ucontext)
{
	std::cerr << "======================================\n";
	std::cerr << "Segmentation fault (SIGSEGV) detected!\n";
	int a{};
	std::cerr << "stack=" << &a << '\n';
	std::cerr << "code=" << info->si_code << '\n';
	std::cerr << "addr=" << info->si_addr << '\n';
	std::cerr << "errno=" << info->si_errno << '\n';

	dump_regs(*static_cast<ucontext_t *>(_ucontext));
	std::exit(1);
}

[[gnu::noinline]] void die(int *x)
{
	asm("mov %0, %%ds" : : "r"(0x1234));
	*x = 42;
}
}

int main(int argc, char **)
{
	bool altstack = argc > 1;
	struct sigaction sa{};
	sa.sa_sigaction = segv;
	sa.sa_flags = SA_SIGINFO | SA_ONSTACK;
	auto stack = std::make_unique<char[]>(SIGSTKSZ);
	stack_t newss{};
	newss.ss_sp = stack.get();
	newss.ss_size = SIGSTKSZ;
	if (altstack) {
		std::cerr << "Using alternate signal stack at " << static_cast<void *>(stack.get()) << '\n';
		::sigaltstack(&newss, nullptr);
	} else {
		std::cerr << "Not using alternate signal stack\n";
	}
	::sigaction(SIGSEGV, &sa, nullptr);
	int *x = nullptr;
	--x;
	std::cerr << "stack=" << &x << '\n';
	std::cerr << "x=" << x << '\n';
	std::cerr << "die=" << reinterpret_cast<void *>(die) << '\n';
	ucontext_t ucontext{};
	::getcontext(&ucontext);
	dump_regs(ucontext);

	die(x);

	return 0;
}

