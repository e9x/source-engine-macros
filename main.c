#define WIN32_LEAN_AND_MEAN
#include "./util.h"
#include <stdio.h>
#include <conio.h>
#include <stdlib.h>

const wchar_t* EXE = L"hl2.exe";
/*
VALORANT-Win64-Shipping.exe
portal2.exe
*/

typedef struct {
	const char* label;
	const char* key_label;
	char key;
	char detect;
	int speed;
} Macro;

Macro macros[] = {
	{ "BHop", "[SPACE]", VK_SPACE, VK_SPACE, 20 },
	{ "Use", "[F]", 'E', 'F', 20 }
};

#define MACRO_COUNT (sizeof(macros) / sizeof(Macro))

HANDLE threads[MACRO_COUNT * sizeof(HANDLE)];

HWND game;

int quit = 0;

DWORD WINAPI repeat_key(LPVOID param) {
	size_t index = (size_t)param;
	Macro data = macros[index];

	short vk_code = LOBYTE(VkKeyScan(data.key));

	ExKeyInfo lparam;
	memset(&lparam, 0, sizeof(ExKeyInfo));

	lparam.scan = MapVirtualKey(vk_code, MAPVK_VK_TO_VSC);

	while (!quit) {
		if (GetAsyncKeyState(data.detect) != 0) {
			lparam.repeat = 0;
			lparam.previous_state = 0;
			lparam.transition_state = 0;
			PostMessage(game, WM_KEYDOWN, vk_code, ExKeyInfo_uint(lparam));
			lparam.repeat = 1;
			lparam.previous_state = 1;
			lparam.transition_state = 1;
			PostMessage(game, WM_KEYUP, vk_code, ExKeyInfo_uint(lparam));
			Sleep(data.speed);
		}

		Sleep(1);
	}

	return EXIT_SUCCESS;
}

void wait_game() {
	printf("Waiting for %ls HWND...\n", EXE);
	while (!(game = find_process(EXE))) Sleep(1);
	printf("Found %ls HWND: 0x%p\n", EXE, game);
}

int main() {
	wait_game();
	
	for (size_t index = 0; index < MACRO_COUNT; index++) {
		threads[index] = 0;

		Macro data = macros[index];
		HANDLE thread = CreateThread(0, 0, repeat_key, (LPVOID)index, 0, 0);

		if(!thread)printf("Thread creation for %s failed, last error: %d", data.label, GetLastError());
		else printf("Press %s to repeatedly %s\n", data.key_label, data.label);
		
		threads[index] = thread;
	}

	printf("Press [ESC] in this terminal to quit\n");

	while (!(quit = _getch() == VK_ESCAPE)) if (!IsWindow(game)) {
		game = NULL;
		wait_game();
	}

	return EXIT_SUCCESS;
}