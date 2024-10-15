#include <efi.h>
#include <efilib.h>

#include "common.h"

#define IA32_APIC_BASE	0x01b
#define  X2APIC_ENABLE	BIT(10)

#define APIC_BASE_MSR		0x800
#define  APIC_ID		0x020
#define  APIC_VERSION		0x030
#define  APIC_LVT_TIMER		0x320
#define  APIC_TIMER_INIT_CNT	0x380
#define  APIC_TIMER_CUR_CNT	0x390
#define  APIC_TIMER_DIVIDE	0x3e0

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

static UINT32 read_x2apic(UINT32 reg)
{
	return rdmsr(APIC_BASE_MSR + (reg >> 4));
}

#if 0
static void write_x2apic(UINT32 reg, UINT32 val)
{
	wrmsr(APIC_BASE_MSR + (reg >> 4), val);
}
#endif

struct idt_desc {
        UINT32 a;
        UINT32 b;
        UINT32 c;
        UINT32 rsvd;
} __packed;

struct idt {
	UINT16 len;
	void *addr;
} __packed;

static void timer_irq()
{
	while (1) {
	}
	//Print(L"%s\n");
}

static void dump_idtr(const struct idt_desc *idtd)
{
	UINT16 seg = idtd->a >> 16;
	UINT64 off = ((UINT64)idtd->c << 32) | (idtd->b & 0xffff0000) | (idtd->a & 0xffff);
	Print(L"%X %X %X -> %.4x:%lX flags=%.4x\n", idtd->a, idtd->b, idtd->c, seg, off, idtd->b & 0xffff);
}

static void dump_idt()
{
	struct idt idt;
	struct idt_desc *idtr;

	asm volatile("sidt %0" : "=m" (idt));

	Print(L"IDT=%lX size=%x\n", idt.addr, idt.len);
	idtr = idt.addr;
	for (unsigned a = 0; a < idt.len; a += sizeof(*idtr), idtr++)
		dump_idtr(idtr);

	idtr = idt.addr;
	UINT64 handler = (UINT64)timer_irq;
	idtr[0x20].a &= 0xffff0000;
	idtr[0x20].a |= handler & 0x0000ffff;
	idtr[0x20].b &= 0x0000ffff;
	idtr[0x20].b |= handler & 0xffff0000;
	dump_idtr(&idtr[0x20]);
}

void dump_x2APIC()
{
	UINT32 ecx;

	dump_idt();

	cpuid(1, 0, NULL, NULL, &ecx, NULL);
	if (!(ecx & BIT(21)))
		return;

	UINT64 base = rdmsr(IA32_APIC_BASE);
	Print(L"ia32_base_msr=%lX\n", base);
	wrmsr(IA32_APIC_BASE, base | X2APIC_ENABLE);

	Print(L"ID=%X\n", read_x2apic(APIC_ID));
	Print(L"ver=%X\n", read_x2apic(APIC_VERSION));
	UINT32 timer = read_x2apic(APIC_LVT_TIMER);
	Print(L"timer LVT=%X init=%X cur=%X div=%X\n", timer,
	      read_x2apic(APIC_TIMER_INIT_CNT),
	      read_x2apic(APIC_TIMER_CUR_CNT),
	      read_x2apic(APIC_TIMER_DIVIDE));
	//timer |= 0x10;
	//write_x2apic(APIC_LVT_TIMER, timer);
}
