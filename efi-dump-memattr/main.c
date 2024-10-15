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

struct efi_memory_attributes_table {
	UINT32 version;
	UINT32 num_entries;
	UINT32 desc_size;
	UINT32 flags;
        EFI_MEMORY_DESCRIPTOR entry[];
};

EFI_STATUS efi_main(EFI_HANDLE image, EFI_SYSTEM_TABLE *systab)
{
	InitializeLib(image, systab);

	gBS->SetWatchdogTimer(0, 0x0, 0, NULL);

	systab->ConOut->Reset(systab->ConOut, FALSE);
	systab->ConOut->EnableCursor(systab->ConOut, TRUE);

	struct efi_memory_attributes_table *table;
	if (LibGetSystemConfigurationTable(&memattr_table_guid, (void **)&table) == EFI_SUCCESS) {
		Print(L"tab addr=0x%lx vers=%u count=%u size=%u (%u) flags=0x%08x\n",
		      table, table->version, table->num_entries, table->desc_size, sizeof(EFI_MEMORY_DESCRIPTOR), table->flags);
		void *desc_ptr = table->entry;

		for (unsigned i = 0; i < min(64, table->num_entries); i++, desc_ptr += table->desc_size) {
			EFI_MEMORY_DESCRIPTOR *desc = desc_ptr;
			Print(L"\ttype=0x%x phys=0x%lx virt=0x%lx pages=%4u attr=0x%016lx\n",
			      desc->Type, desc->PhysicalStart, desc->VirtualStart,
			      desc->NumberOfPages, desc->Attribute);
		}
	}

	Print(L"Press:\n\ts to shutdown\n\tc to cold reboot\n\tother keys to warm reboot...\n");
	EFI_INPUT_KEY key;
	get_keystroke(&key);

	EFI_RESET_TYPE reset = EfiResetWarm;
	if (key.UnicodeChar == 's')
		reset = EfiResetShutdown;
	else if (key.UnicodeChar == 'c')
		reset = EfiResetCold;

	return gRT->ResetSystem(reset, EFI_SUCCESS, 0, NULL);
}
