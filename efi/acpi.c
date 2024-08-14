#include <efi.h>
#include <efilib.h>

#include "common.h"

#define ACPI_OEM_ID_SIZE                6
#define ACPI_OEM_TABLE_ID_SIZE          8
#define ACPI_NAMESEG_SIZE               4

struct acpi_table_rsdp {
	CHAR8 signature[8];      /* ACPI signature, contains "RSD PTR " */
	UINT8 checksum;            /* ACPI 1.0 checksum */
	CHAR8 oem_id[ACPI_OEM_ID_SIZE];  /* OEM identification */
	UINT8 revision;            /* Must be (0) for ACPI 1.0 or (2) for ACPI 2.0+ */
	UINT32 rsdt_physical_address;      /* 32-bit physical address of the RSDT */
	UINT32 length;             /* Table length in bytes, including header (ACPI 2.0+) */
	UINT64 xsdt_physical_address;      /* 64-bit physical address of the XSDT (ACPI 2.0+) */
	UINT8 extended_checksum;   /* Checksum of entire table (ACPI 2.0+) */
	UINT8 reserved[3];         /* Reserved, must be zero */
} __packed;

struct acpi_table_header {
	CHAR8 signature[ACPI_NAMESEG_SIZE];      /* ASCII table signature */
	UINT32 length;             /* Length of table in bytes, including this header */
	UINT8 revision;            /* ACPI Specification minor version number */
	UINT8 checksum;            /* To make sum of entire table == 0 */
	CHAR8 oem_id[ACPI_OEM_ID_SIZE];  /* ASCII OEM identification */
	CHAR8 oem_table_id[ACPI_OEM_TABLE_ID_SIZE];      /* ASCII OEM table identification */
	UINT32 oem_revision;       /* OEM revision number */
	CHAR8 asl_compiler_id[ACPI_NAMESEG_SIZE];        /* ASCII ASL compiler vendor ID */
	UINT32 asl_compiler_revision;      /* ASL compiler version */
} __packed;

struct acpi_subtable_header {
        UINT8 type;
        UINT8 length;
} __packed;

struct acpi_table_rsdt {
	struct acpi_table_header header;        /* Common ACPI table header */
	UINT32 table_offset_entry[0];      /* Array of pointers to ACPI tables */
} __packed;

struct acpi_table_xsdt {
	struct acpi_table_header header;        /* Common ACPI table header */
	UINT64 table_offset_entry[0];      /* Array of pointers to ACPI tables */
} __packed;

struct acpi_table_madt {
        struct acpi_table_header header;        /* Common ACPI table header */
        UINT32 address;            /* Physical address of local APIC */
        UINT32 flags;
	struct acpi_subtable_header ctrl[0];
} __packed;

enum acpi_madt_type {
        ACPI_MADT_TYPE_LOCAL_APIC = 0,
        ACPI_MADT_TYPE_IO_APIC = 1,
        ACPI_MADT_TYPE_INTERRUPT_OVERRIDE = 2,
        ACPI_MADT_TYPE_LOCAL_APIC_NMI = 4,
};

struct acpi_madt_local_apic {
        struct acpi_subtable_header header;
        UINT8 processor_id;        /* ACPI processor id */
        UINT8 id;                  /* Processor's local APIC id */
        UINT32 lapic_flags;
} __packed;

struct acpi_madt_io_apic {
        struct acpi_subtable_header header;
        UINT8 id;                  /* I/O APIC ID */
        UINT8 reserved;            /* reserved - must be zero */
        UINT32 address;            /* APIC physical address */
        UINT32 global_irq_base;    /* Global system interrupt where INTI lines start */
} __packed;

struct acpi_madt_interrupt_override {
        struct acpi_subtable_header header;
        UINT8 bus;                 /* 0 - ISA */
        UINT8 source_irq;          /* Interrupt source (IRQ) */
        UINT32 global_irq;         /* Global system interrupt */
        UINT16 inti_flags;
} __packed;

struct acpi_madt_local_apic_nmi {
        struct acpi_subtable_header header;
        UINT8 processor_id;        /* ACPI processor id */
        UINT16 inti_flags;
        UINT8 lint;                /* LINTn to which NMI is connected */
} __packed;

static void dump_ACPI_header(CONST CHAR16 *prefix, struct acpi_table_header *hdr)
{
	Print(L"%s%lx len=%u (%.4a)\n", prefix, (UINT64)hdr, hdr->length, hdr->signature);
}

static void dump_MADT(struct acpi_table_madt *madt)
{
	struct acpi_subtable_header *ctrl = madt->ctrl;
	UINT32 entlen = madt->header.length - sizeof(*madt);
	UINT32 ioapic_addr = 0;

	Print(L"    addr=%X compat=%u entlen=%u\n", madt->address,
	      !!(madt->flags & 1), entlen);

	while (entlen) {
		Print(L"    type=%u len=%u ", ctrl->type, ctrl->length);
		switch (ctrl->type) {
		case ACPI_MADT_TYPE_LOCAL_APIC:
			struct acpi_madt_local_apic *lapic = (void *)ctrl;
			Print(L"LAPIC cpu=%u id=%u EN=%u online_cap=%u", lapic->processor_id,
			      lapic->id, !!(lapic->lapic_flags & 1), !!(lapic->lapic_flags & 2));
			break;
		case ACPI_MADT_TYPE_IO_APIC:
			struct acpi_madt_io_apic *ioapic = (void *)ctrl;
			ioapic_addr = ioapic->address;
			Print(L"IOAPIC id=%u addr=%X irq_base=%u", ioapic->id,
			      ioapic_addr, ioapic->global_irq_base);
			break;
		case ACPI_MADT_TYPE_INTERRUPT_OVERRIDE:
			struct acpi_madt_interrupt_override *intovr = (void *)ctrl;
			Print(L"INTOVR bus=%u src=%u glob=%u trig=%x pol=%x", intovr->bus,
			      intovr->source_irq, intovr->global_irq,
			      (intovr->inti_flags & 0xc) >> 2,
			      intovr->inti_flags & 3);
			break;
		case ACPI_MADT_TYPE_LOCAL_APIC_NMI:
			struct acpi_madt_local_apic_nmi *lapic_nmi = (void *)ctrl;
			Print(L"LAPIC_NMI cpu=%u trig=%x pol=%x lint=%u",
			      lapic_nmi->processor_id, (lapic_nmi->inti_flags & 0xc) >> 2,
			      lapic_nmi->inti_flags & 3, lapic_nmi->lint);
			break;
		default:
			Print(L"unknown");
			break;
		}
		Print(L"\n");

		entlen -= ctrl->length;
		ctrl = (void *)ctrl + ctrl->length;
	}

	dump_LAPIC(madt->address);
	if (ioapic_addr)
		dump_IOAPIC(ioapic_addr);
}

void dump_ACPI(EFI_SYSTEM_TABLE *systab)
{
	static EFI_GUID acpi_guid = ACPI_20_TABLE_GUID;
	struct acpi_table_rsdp *acpi_rsdp = NULL;
	struct acpi_table_rsdt *acpi_rsdt;
	struct acpi_table_xsdt *acpi_xsdt;

	for (unsigned a = 0; a < systab->NumberOfTableEntries; a++) {
		if (!CompareGuid(&systab->ConfigurationTable[a].VendorGuid, &acpi_guid)) {
			acpi_rsdp = systab->ConfigurationTable[a].VendorTable;
			if (strncmpa(acpi_rsdp->signature, (CHAR8 *)"RSD PTR ", 8)) {
				Print(L"Bad RSDP\n");
				acpi_rsdp = NULL;
			}
		}
	}

	if (!acpi_rsdp)
		return;

	acpi_rsdt = (struct acpi_table_rsdt *)(UINT64)acpi_rsdp->rsdt_physical_address;
	acpi_xsdt = (struct acpi_table_xsdt *)acpi_rsdp->xsdt_physical_address;

	Print(L"ACPI RSDP=%lx (len=%u sig=\"%.8a\", rev=%u) RSDT=%lx XSDT=%lx\n",
	      (UINT64)acpi_rsdp, acpi_rsdp->length,
	      acpi_rsdp->signature, acpi_rsdp->revision,
	      (UINT64)acpi_rsdt, (UINT64)acpi_xsdt);

	if (!acpi_rsdt || !acpi_xsdt)
		return;

	dump_ACPI_header(L"  ", &acpi_rsdt->header);
	dump_ACPI_header(L"  ", &acpi_xsdt->header);

	UINT32 entries = (acpi_rsdt->header.length - sizeof(*acpi_rsdt)) / sizeof(*acpi_rsdt->table_offset_entry);
	Print(L"ACPI RSDT entries=%u\n", entries);
	for (unsigned a = 0; a < entries; a++) {
		struct acpi_table_header *hdr = (void *)(UINT64)acpi_rsdt->table_offset_entry[a];
		dump_ACPI_header(L"  ", hdr);
	}

	entries = (acpi_xsdt->header.length - sizeof(*acpi_xsdt)) / sizeof(*acpi_xsdt->table_offset_entry);
	Print(L"ACPI XSDT len=%u entries=%u\n", entries);
	for (unsigned a = 0; a < entries; a++) {
		struct acpi_table_header *hdr = (void *)acpi_xsdt->table_offset_entry[a];
		dump_ACPI_header(L"  ", hdr);
		if (!strncmpa(hdr->signature, (CHAR8 *)"APIC", 4)) {
			dump_MADT((void *)hdr);
		}
	}
}
