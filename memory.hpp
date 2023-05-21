#pragma once
#include <Windows.h>

namespace memory {
	int get_proc_id(const char* process_name);
	HANDLE get_process_handle(int process_id);
	BOOL write_to_process(HANDLE process, LPVOID address, LPCVOID buffer);
	BOOL read_from_process(HANDLE process, LPVOID address, LPVOID* out_buffer);

}