#ifndef COMMON_H
#define COMMON_H

#include <efi.h>

#define BIT(n) (1UL << (n))

#define __packed __attribute__((packed))

static inline UINT32 read32(UINT32 address)
{
	return *(volatile UINT32 *)(UINT64)address;
}

void dump_ACPI(EFI_SYSTEM_TABLE *systab);
void dump_x2APIC();
void dump_IOAPIC(UINT32 address);
void dump_LAPIC(UINT32 address);

static inline void cpuid(UINT32 in, UINT32 cnt, UINT32 *eax, UINT32 *ebx, UINT32 *ecx, UINT32 *edx)
{
	UINT32 _eax, _ebx, _ecx, _edx;
	asm volatile("cpuid" : "=a" (_eax), "=b" (_ebx), "=c" (_ecx), "=d" (_edx) : "a" (in), "c" (cnt));
	if (eax)
		*eax = _eax;
	if (ebx)
		*ebx = _ebx;
	if (ecx)
		*ecx = _ecx;
	if (edx)
		*edx = _edx;
}

static inline UINT64 rdmsr(UINT32 msr)
{
	UINT32 eax, edx;

	asm volatile("rdmsr" : "=a" (eax), "=d" (edx) : "c" (msr));

	return (UINT64)edx << 32 | eax;
}

static inline void wrmsr(UINT32 msr, UINT64 val)
{
	UINT32 eax = val, edx = val >> 32;

	asm volatile("wrmsr" : : "a" (eax), "c" (msr), "d" (edx));
}

#endif
