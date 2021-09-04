#include <windows.h>
#include <stdio.h>
#include <conio.h>
#include "./util.c"

const wchar_t* EXE = L"hl2.exe";

typedef struct {
	const char* label;
	const char* key_label;
	char key;
	char detect;
} RepeatData;

#define MACRO_COUNT 2

RepeatData macros[MACRO_COUNT] = {
	// { label, key label, send key, detect press }
	{ "BHop", "[SPACE]", VK_SPACE, VK_SPACE},
	{ "Use", "[E]", 'E', 'F'}
};

HWND game;

int quit = 0;

DWORD WINAPI repeat_key(LPVOID param) {
	RepeatData* data = (RepeatData*)param;

	short vk_code = LOBYTE(VkKeyScan(data->key));

	ExKeyInfo lparam;
	memset(&lparam, 0, sizeof(ExKeyInfo));

	lparam.scan = MapVirtualKey(vk_code, MAPVK_VK_TO_VSC);

	while (!quit) {
		if (GetAsyncKeyState(data->detect) != 0) {
			lparam.repeat = 0;
			lparam.previous_state = 0;
			lparam.transition_state = 0;
			PostMessage(game, WM_KEYDOWN, vk_code, ExKeyInfo_uint(lparam));
			lparam.repeat = 1;
			lparam.previous_state = 1;
			lparam.transition_state = 1;
			PostMessage(game, WM_KEYUP, vk_code, ExKeyInfo_uint(lparam));
			Sleep(10);
		}

		Sleep(1);
	}

	return EXIT_SUCCESS;
}

int main() {
	printf("Waiting for %ls HWND...\n", EXE);
	while (!(game = find_process(EXE)));
	printf("Found %ls HWND: 0x%p\n", EXE, game);
	
	HANDLE threads[MACRO_COUNT];
	
	for (int index = 0; index < MACRO_COUNT; index++) {
		RepeatData data = macros[index];
		HANDLE thread = CreateThread(0, 0, repeat_key, (LPVOID)&macros[index], 0, 0);

		if(!thread)printf("Thread creation for %s failed, last error: %d", data.label, GetLastError());
		else printf("Press %s to repeatedly %s\n", data.key_label, data.label);
		
		threads[index] = thread;
	}

	printf("Press [ESC] in this terminal to quit\n");

	while (!(quit = _getch() == VK_ESCAPE));

	for (int index = 0; index < MACRO_COUNT; index++) {
		if (threads[index]) WaitForSingleObject(threads[index], 100);
	}

	return EXIT_SUCCESS;
}