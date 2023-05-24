#pragma once
#include <iostream>
#include <windows.h>

namespace memory {
    int get_proc_id(const std::wstring& process_name);
    HANDLE get_process_handle(int process_id);
    BOOL set_debug_privilege(BOOL b);

    template<typename T>
    BOOL write_to_process(HANDLE process, uintptr_t address, T* buffer) {
        BOOL result = FALSE;
        SIZE_T bytes_written = 0;

        if(sizeof(T) == 0) {
            std::cout << "Size of write buffer is zero. Useless write to memory." << std::endl;
            return FALSE;
        }

        result = WriteProcessMemory(process, (LPVOID)address, buffer, sizeof(T), &bytes_written);
        if(bytes_written == 0)
            std::cout << "WriteProcessMemory wrote 0 bytes! " << GetLastError() << std::endl;

        return result;
    }

    template<typename T>
    BOOL read_from_process(HANDLE process, uintptr_t address, T* out_buffer) {
        BOOL result = FALSE;
        SIZE_T bytes_read = 0;

        if(sizeof(T) == 0) {
            std::cout << "Size of read buffer is zero. Useless read from memory." << std::endl;
            return FALSE;
        }

        result = ReadProcessMemory(process, (LPVOID)address, out_buffer, sizeof(T), &bytes_read);
        if(bytes_read == 0)
            std::cout << "ReadProcessMemory read 0 bytes! " << GetLastError() << std::endl;

        return result;
    }
}