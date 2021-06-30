#include <windows.h>
#include "hidapi/hidapi.h"

#define HK_MODS    (MOD_WIN | MOD_SHIFT | MOD_CONTROL | MOD_ALT | MOD_NOREPEAT)
#define HK_EXIT    VK_F23
#define HK_COPY    VK_F24

#define BUFFER_SIZE 32

#pragma comment(lib, "hidapi/hidapi")

int APIENTRY wWinMain(_In_ HINSTANCE instance, _In_opt_ HINSTANCE prevInstance, _In_ LPWSTR cmdLine, _In_ int cmdShow)
{
	if (hid_init())
		return -1;

	RegisterHotKey(NULL, 1, HK_MODS, HK_EXIT);
	RegisterHotKey(NULL, 2, HK_MODS, HK_COPY);

	MSG msg = { 0 };

	while (GetMessage(&msg, NULL, 0, 0) != 0)
	{
		if (msg.message == WM_HOTKEY)
		{
			int hotkey = HIWORD(msg.lParam);
			if (hotkey == HK_EXIT)
			{
				break;
			}
			else if (hotkey == HK_COPY)
			{
				if (OpenClipboard(NULL))
				{
					HANDLE clipboardHandle = GetClipboardData(CF_TEXT);
					if (clipboardHandle != NULL)
					{
						char* clipboardText = static_cast<char*>(GlobalLock(clipboardHandle));
						if (clipboardText != NULL)
						{
							struct hid_device_info* hidDevices = hid_enumerate(0, 0);

							for (struct hid_device_info* hidDevice = hidDevices; hidDevice != NULL; hidDevice = hidDevice->next)
							{
								if (hidDevice->usage == 0x61 && hidDevice->usage_page == 0xff60)
								{
									hid_device* hidHandle = hid_open_path(hidDevice->path);
									if (hidHandle != NULL)
									{
										unsigned char buffer[BUFFER_SIZE] = { 0 };

										for (const char* hidText = clipboardText; *hidText; )
										{
											size_t length = strlen(clipboardText);
											if (length > BUFFER_SIZE - 2)
												length = BUFFER_SIZE - 2;

											memset(buffer, 0x00, sizeof(buffer));
											buffer[0] = 0x00;
											buffer[1] = 0xfd;
											memcpy(buffer + 2, clipboardText, length);

											int result = hid_write(hidHandle, buffer, length + 2);
											if (result == -1)
												break;

											hidText += length;
										}

										hid_close(hidHandle);
									}
								}
							}

							hid_free_enumeration(hidDevices);

							GlobalUnlock(clipboardText);
						}
					}

					EmptyClipboard();
					CloseClipboard();
				}
			}
		}
	}

	hid_exit();
	return 0;
}