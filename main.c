#include <windows.h>
#include <stdio.h>
#include "./util.c"

HWND hl2;

int quit = 0;

DWORD WINAPI repeat_key(LPVOID param) {
	int key = (int)param;

	short vkCode = LOBYTE(VkKeyScan(key));

	ExKeyInfo lParam;
	memset(&lParam, 0, sizeof(ExKeyInfo));

	lParam.scan = MapVirtualKey(vkCode, MAPVK_VK_TO_VSC);

	while (!quit) {
		if (GetAsyncKeyState(key) != 0) {
			lParam.repeat = 0;
			lParam.previous_state = 0;
			lParam.transition_state = 0;
			PostMessage(hl2, WM_KEYDOWN, vkCode, ExKeyInfo_uint(lParam));
			lParam.repeat = 1;
			lParam.previous_state = 1;
			lParam.transition_state = 1;
			PostMessage(hl2, WM_KEYUP, vkCode, ExKeyInfo_uint(lParam));
			Sleep(10);
		}

		Sleep(1);
	}

	return EXIT_SUCCESS;
}

int main() {
	printf("Waiting for hl2.exe ...\n");
	while (!(hl2 = find_process(L"hl2.exe")));
	printf("Found hl2.exe: %p\n", hl2);
	
	HANDLE jump_thread = CreateThread(0, 0, repeat_key, (LPVOID)VK_SPACE, NULL, NULL);
	HANDLE use_thread = CreateThread(0, 0, repeat_key, (LPVOID)'E', NULL, NULL);

	if (jump_thread == 0 || use_thread == 0) {
		printf("Jump thread: %p\nUse thread: %p", jump_thread, use_thread);
		return EXIT_FAILURE;
	}

	printf("Press [E] to repeatedly press use\nPress [SPACE] to repeatedly jump\nPress [ESC] in this terminal to quit\n");

	while (!(quit = getch() == VK_ESCAPE));
	
	WaitForSingleObject(jump_thread, 100);
	WaitForSingleObject(use_thread, 100);

	return EXIT_SUCCESS;
}