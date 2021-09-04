#include <Psapi.h>
#include <string.h>

typedef struct {
	unsigned short repeat;
	unsigned char scan;
	int extended, previous_state, transition_state;
} ExKeyInfo;

unsigned int ExKeyInfo_uint(ExKeyInfo i) {
	return i.repeat | (i.scan << 16) | (i.extended << 24) | (i.previous_state << 30) | (i.transition_state << 31);
}

int strend_t(const wchar_t* s, const wchar_t* t) {
	size_t ls = lstrlenW(s);
	size_t lt = lstrlenW(t);
	return ls >= lt ? 0 == memcmp(t, s + (ls - lt), lt) : 0;
}

typedef struct {
	const wchar_t* match;
	HWND result;
} EnumData;

BOOL CALLBACK WINAPI enum_cb(HWND window, LPARAM param) {
	EnumData* data = (EnumData*)param;

	DWORD pid = 0;

	GetWindowThreadProcessId(window, &pid);

	wchar_t name[MAX_PATH] = { 0 };
	HANDLE process = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, pid);

	if (process) {
		if (GetModuleFileNameEx(process, NULL, name, MAX_PATH) && strend_t(name, data->match)) {
			data->result = window;
			return FALSE;
		}

		CloseHandle(process);
	}

	return TRUE;
}

HWND find_process(const wchar_t* name) {
	EnumData data;

	data.result = 0;
	data.match = name;

	EnumWindows(enum_cb, (LPARAM)&data);

	// if (!data.result) printf("data.result == 0, last error: %d\n", GetLastError());

	return data.result;
}