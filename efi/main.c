#include <efi.h>
#include <efilib.h>

#include "common.h"

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

static void handle_keys()
{
	EFI_INPUT_KEY key;

	do {
		Print(L"Press a key (ESC to exit)...\n");
		get_keystroke(&key);
		Print(L"\nYou pressed=%d/%d\n", key.ScanCode, key.UnicodeChar);
	} while (key.ScanCode != 23);
}

static void usleep(UINTN us)
{
	gBS->Stall(us);
}

static void msleep(UINTN ms)
{
	usleep(ms * 1000);
}

static void dump_vars()
{
	CHAR16 var[64] = L"";
	EFI_GUID guid;
	unsigned counter = 0;

	while (1) {
		UINTN size = sizeof(var);
		EFI_STATUS ret = gRT->GetNextVariableName(&size, var, &guid);
		if (ret) {
			if (ret != EFI_NOT_FOUND)
				Print(L"Error: %r\n", ret);
			break;
		}
		Print(L"VAR: %.25s GUID: %g\n", var, &guid);
		if (!(++counter % 20)) {
			EFI_INPUT_KEY k;
			get_keystroke(&k);
		}

	}
}

EFI_STATUS efi_main(EFI_HANDLE image, EFI_SYSTEM_TABLE *systab)
{
	CHAR16 buf[64];

	InitializeLib(image, systab);

	gBS->SetWatchdogTimer(0, 0x0, 0, NULL);

	systab->ConOut->Reset(systab->ConOut, FALSE);
	systab->ConOut->EnableCursor(systab->ConOut, TRUE);

	Print(L"Hello, world!\n");
	dump_x2APIC();
	while (0) {
		Input(L"Input command: ", buf, sizeof(buf));
		Print(L"\nYou wrote: %s\n", buf);
		if (!StrnCmp(buf, L"q", 1))
			break;

		if (!StrCmp(buf, L"acpi"))
			dump_ACPI(systab);
		else if (!StrCmp(buf, L"apic"))
			dump_x2APIC();
		else if (!StrCmp(buf, L"keys"))
			handle_keys();
		else if (!StrCmp(buf, L"vars"))
			dump_vars();
	}

	msleep(1000);

	return gRT->ResetSystem(EfiResetShutdown, EFI_SUCCESS, 0, NULL);
}
