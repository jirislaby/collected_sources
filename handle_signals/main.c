#define _GNU_SOURCE
#include <fenv.h>
#include <math.h>
#include <signal.h>
#include <stdio.h>
#include <string.h>
#include <ucontext.h>
#include <unistd.h>

#include <asm/insn.h>

char *hell_ptr = (void *)0x10;
double zero;

static unsigned char op_to_reg[] = {
	REG_RAX,
	REG_RCX,
	REG_RDX,
	REG_RBX,
	REG_RSP,
	REG_RBP,
	REG_RSI,
	REG_RDI,
};

static void siga(int sig, siginfo_t *sigi, void *con)
{
	static int counter;
	struct insn insn;
	ucontext_t *ucon = con;
	greg_t *regs = ucon->uc_mcontext.gregs;
	struct _libc_fpstate *fpregs = ucon->uc_mcontext.fpregs;

	if (counter++ >= 2000) {
		puts("recursive signal, cannot recover, it seems!");
		exit(0);
	}

	printf("  > %s: cnt=%d signr=%d addr=%p code=%d RSP=%llx\n", __func__,
			counter, sig, sigi->si_addr, sigi->si_code,
			regs[REG_RSP]);

	printf("%lf\n", zero + 2);
	printf("%d\n", *hell_ptr);
	return;

	insn_init(&insn, (void *)regs[REG_RIP], 20, 1);
	insn_get_length(&insn);
	printf("  > RIP=0x%llx (insn size=%u)\n", regs[REG_RIP], insn.length);

	if (insn.modrm.nbytes) {
		int modrm_reg = X86_MODRM_REG(insn.modrm.bytes[0]);
		printf("  > writing ");

		regs[REG_RIP] += insn.length;
		if (sig == 8) {
			const double con = 1234567890.123456;
			printf("%lf instead of %lf", con,
				*(double *)(fpregs->_xmm[modrm_reg].element));
			*(double *)(fpregs->_xmm[modrm_reg].element) = con;
		} else {
			const long con = 123;
			printf("%ld instead of %lld", con,
					regs[op_to_reg[modrm_reg]]);
			regs[op_to_reg[modrm_reg]] = con;
		}
		printf(" to reg %d\n", modrm_reg);
	}
}

int main()
{
	const struct sigaction sigact = {
		.sa_sigaction = siga,
		.sa_flags = SA_SIGINFO | SA_NODEFER,
	};

	sigaction(SIGSEGV, &sigact, NULL);
	sigaction(SIGFPE, &sigact, NULL);
	feenableexcept(-1);

	printf("FP division by zero:\n");
	printf("res=%lf\n", 10 / zero);
	printf("Invalid ptr dereference 1:\n");
	printf("res=%d\n", *hell_ptr);
	printf("Invalid ptr dereference 2 (check $?):\n");

	return *hell_ptr;
}
