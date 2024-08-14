#include <efi.h>
#include <efilib.h>

#include "common.h"

void dump_LAPIC(UINT32 address)
{
	UINT32 val;

	val = read32(address + 0x20);
	Print(L"LAPIC id=0x%1x ", (val >> 24) & 0xf);
	val = read32(address + 0x30);
	Print(L"ver=%u max_lvt=%u supp_eoi=%u\n", val & 0xf, (val >> 16) & 0xf,
	      !!(val & 0x1000));
}

static UINT32 read_IOAPIC(UINT32 address, UINT8 reg)
{
	volatile UINT32 *sel = (UINT32 *)(UINT64)address;

	*sel = reg;

	return read32(address + 0x10);
}

void dump_IOAPIC(UINT32 address)
{
	UINT32 val;

	val = read_IOAPIC(address, 0x00);
	Print(L"IOAPIC id=0x%1x ", (val >> 24) & 0xf);
	val = read_IOAPIC(address, 0x01);
	UINT32 max_red = (val >> 16) & 0xff;
	Print(L"ver=0x%x (== 0x11?) max_red=%u\n", val & 0xff, max_red);

	for (unsigned a = 0; a <= max_red; a++) {
		UINT64 val64 = read_IOAPIC(address, 0x11 + a*2);
		val64 <<= 32;
		val64 |= read_IOAPIC(address, 0x10 + a*2);
		Print(L"  %2u: dest=%02x msk=%u trig=%u rem=%u intpol=%u delivs=%u dstmod=%u delmod=0x%x intvec=%x\n",
		      a,
		      val64 >> 56,
		      !!(val64 & 0x10000),
		      !!(val64 & 0x08000),
		      !!(val64 & 0x04000),
		      !!(val64 & 0x02000),
		      !!(val64 & 0x01000),
		      !!(val64 & 0x00800),
		      (val64 >> 8) & 0x7,
		      val64 & 0xff);
	}
}
