#include <efi.h>
#include <efilib.h>

#define min(x, y)	((x) < (y) ? (x) : (y))

static EFI_STATUS get_keystroke(EFI_INPUT_KEY *key)
{
	SIMPLE_INPUT_INTERFACE *ci = ST->ConIn;
	UINTN EventIndex;
	EFI_STATUS efi_status;

	do {
		gBS->WaitForEvent(1, &ci->WaitForKey, &EventIndex);
		efi_status = ci->ReadKeyStroke(ci, key);
	} while (efi_status == EFI_NOT_READY);

	return efi_status;
}

static EFI_GUID memattr_table_guid =
	{ 0xdcfa911d, 0x26eb, 0x469f, {0xa2, 0x20, 0x38, 0xb7, 0xdc, 0x46, 0x12, 0x20} };
static EFI_GUID linux_eventlog_guid =
	{ 0xb7799cb0, 0xeca2, 0x4943, {0x96, 0x67, 0x1f, 0xae, 0x07, 0xb7, 0x47, 0xfa} };

struct efi_memory_attributes_table {
	UINT32 version;
	UINT32 num_entries;
	UINT32 desc_size;
	UINT32 flags;
        EFI_MEMORY_DESCRIPTOR entry[];
};

struct linux_efi_tpm_eventlog {
	UINT32 size;
	UINT32 final_events_preboot_size;
	UINT8 version;
	UINT8 log[];
};

static void dump_tables()
{
	for (unsigned i = 0; i < ST->NumberOfTableEntries; i++) {
		Print(L"tab guid=%g ptr=0x%lX\n", &ST->ConfigurationTable[i].VendorGuid,
		      ST->ConfigurationTable[i].VendorTable);
	}
}

static void dump_memattr()
{
	struct efi_memory_attributes_table *table;

	if (LibGetSystemConfigurationTable(&memattr_table_guid, (void **)&table) != EFI_SUCCESS)
		return;

	Print(L"MEMATTR tab addr=0x%lx vers=%u count=%u size=%u (%u) flags=0x%08x\n",
	      table, table->version, table->num_entries, table->desc_size, sizeof(EFI_MEMORY_DESCRIPTOR), table->flags);
	void *desc_ptr = table->entry;

	for (unsigned i = 0; i < min(4, table->num_entries); i++, desc_ptr += table->desc_size) {
		EFI_MEMORY_DESCRIPTOR *desc = desc_ptr;
		Print(L"    type=0x%x phys=0x%lx virt=0x%lx pages=%4u attr=0x%016lx\n",
		      desc->Type, desc->PhysicalStart, desc->VirtualStart,
		      desc->NumberOfPages, desc->Attribute);
	}
}

static void install_tpm()
{
	EFI_MEMORY_TYPE pat = PoolAllocationType;
	PoolAllocationType = EfiACPIReclaimMemory;

	/* dummy, RNG in Linux kernel */
	AllocatePool(4 + 32);

	unsigned log_size = 64 << 10;
	struct linux_efi_tpm_eventlog *log_tbl = AllocateZeroPool(sizeof(*log_tbl) + log_size);
	log_tbl->size = log_size;
        log_tbl->final_events_preboot_size = 10;
        log_tbl->version = 1;

	PoolAllocationType = pat;

	Print(L"Installing TPM table 0x%lX\n", log_tbl);
	if (gBS->InstallConfigurationTable(&linux_eventlog_guid, log_tbl) != EFI_SUCCESS)
		Print(L"Could not install\n");
}

EFI_STATUS efi_main(EFI_HANDLE image, EFI_SYSTEM_TABLE *systab)
{
	EFI_INPUT_KEY key;

	InitializeLib(image, systab);

	gBS->SetWatchdogTimer(0, 0x0, 0, NULL);

	systab->ConOut->Reset(systab->ConOut, FALSE);
	systab->ConOut->EnableCursor(systab->ConOut, TRUE);

	dump_tables();

	Print(L"Press a key to continue\n");
	get_keystroke(&key);

	dump_memattr();

	install_tpm();
	dump_memattr();

	Print(L"Press a key to continue\n");
	get_keystroke(&key);

	dump_tables();

	Print(L"Press:\n    s to shutdown\n    c to cold reboot\n    n to boot next\nother keys to warm reboot...\n");
	get_keystroke(&key);

	if (key.UnicodeChar == 'n')
		return EFI_SUCCESS;

	EFI_RESET_TYPE reset = EfiResetWarm;
	if (key.UnicodeChar == 's')
		reset = EfiResetShutdown;
	else if (key.UnicodeChar == 'c')
		reset = EfiResetCold;

	return gRT->ResetSystem(reset, EFI_SUCCESS, 0, NULL);
}
