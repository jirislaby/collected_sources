#ifndef COMMON_H
#define COMMON_H

#include <efi.h>

#define __packed __attribute__((packed))

static inline UINT32 read32(UINT32 address)
{
	return *(volatile UINT32 *)(UINT64)address;
}

void dump_ACPI(EFI_SYSTEM_TABLE *systab);
void dump_APIC();
void dump_IOAPIC(UINT32 address);
void dump_LAPIC(UINT32 address);

#endif
