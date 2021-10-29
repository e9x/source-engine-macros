#pragma once
#include <windows.h>

typedef struct {
	unsigned short repeat;
	unsigned char scan;
	int extended, previous_state, transition_state;
} ExKeyInfo;

unsigned int ExKeyInfo_uint(ExKeyInfo i);

HWND find_process(const wchar_t* name);