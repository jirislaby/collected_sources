#include <efi.h>
#include <efilib.h>

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

static void usleep(UINTN us)
{
	gBS->Stall(us);
}

static void msleep(UINTN ms)
{
	usleep(ms * 1000);
}

EFI_STATUS efi_main(EFI_HANDLE image, EFI_SYSTEM_TABLE *systab)
{
	CHAR16 buf[64];
	EFI_INPUT_KEY key;

	InitializeLib(image, systab);

	gBS->SetWatchdogTimer(0, 0x0, 0, NULL);

	systab->ConOut->Reset(systab->ConOut, FALSE);
	systab->ConOut->EnableCursor(systab->ConOut, TRUE);

	Print(L"Hello, world!\n");
	Input(L"Input something: ", buf, sizeof(buf));
	Print(L"\nYou wrote: %s\n", buf);

	do {
		Print(L"Press a key (ESC to exit)...\n");
		get_keystroke(&key);
		Print(L"\nYou pressed=%d/%d\n", key.ScanCode, key.UnicodeChar);
	} while (key.ScanCode != 23);

	msleep(3000);

	return gRT->ResetSystem(EfiResetShutdown, EFI_SUCCESS, 0, NULL);
}
