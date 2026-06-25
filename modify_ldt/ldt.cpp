#include <cerrno>
#include <cstring>
#include <sl/helpers/Color.h>
#include <sl/helpers/Exception.h>

#include <syscall.h>
#include <asm/ldt.h>

using Clr = SlHelpers::Color;
using RunEx = SlHelpers::RuntimeException;
using SlHelpers::raise;

namespace {

enum struct LDTFunc : int {
	GET = 0,
	SET = 1,
};

void switch32()
{
	asm volatile(
		"pushw  0x002b\n"	// ss
		"pushq  %rsp\n"	// rsp
		"pushf\n"
		"pushw  0x7f38\n"	// cs
		"pushq  0\n"	// rip
		"iret\n"
		//"1:\n"
	);
}

void modify_ldt(LDTFunc func, struct user_desc &desc)
{
	auto ret = syscall(SYS_modify_ldt, func, &desc, sizeof(desc));
	//if ((func == LDTFunc::GET && ret != 0) || (func == LDTFunc::SET && ret != sizeof(desc)))
	if (ret < 0)
		RunEx("modify_ldt-") << static_cast<unsigned>(func) << " (" << desc.entry_number <<
			") failed with ret=" << ret << ": " << strerror(errno) << raise;
}

void dumpLDT(unsigned LDT)
{
	struct user_desc desc;
	desc.entry_number = LDT;
	modify_ldt(LDTFunc::GET, desc);
	Clr(std::cout) << "LDT entry " << std::dec << LDT << ": " <<
		desc.entry_number << ", b=0x" <<
		std::hex << desc.base_addr << ", l=0x" <<
		desc.limit << ", 32=" <<
		desc.seg_32bit << ", c=" <<
		desc.contents << ", RX=" <<
		desc.read_exec_only << ", LIP=" <<
		desc.limit_in_pages << ", NP=" <<
		desc.seg_not_present << ", USE=" <<
		desc.useable;
}

void setLDT(unsigned LDT)
{
	struct user_desc desc;
	desc.entry_number = LDT;
	desc.base_addr = 0x12345000;
	desc.limit = 0x1000;
	desc.seg_32bit = 1;
	desc.contents = 0;
	desc.read_exec_only = 0;
	desc.limit_in_pages = 0;
	desc.seg_not_present = 0;
	desc.useable = 1;
	modify_ldt(LDTFunc::SET, desc);
}

void handledMain(int /*argc*/, char **/*argv*/)
{
	for (int i = 0; i < 10; ++i)
		dumpLDT(i);

	setLDT(7);
	dumpLDT(7);
	switch32();
}

}

int main(int argc, char **argv)
{
	try {
		handledMain(argc, argv);
	} catch (const std::exception &e) {
		Clr(std::cerr, Clr::RED) << e.what();
		return EXIT_FAILURE;
	}

	return 0;
}

