#include "memory.hpp"
#include <iostream>
#include <tlhelp32.h>

int memory::get_proc_id(const char* process_name) {
    int pid = 0;
    BOOL hResult;
    HANDLE hSnapshot;
    PROCESSENTRY32 pe;

    // snapshot of all processes in the system
    hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if(hSnapshot == INVALID_HANDLE_VALUE) {
        std::cout << "CreateToolhelp32Snapshot returned an invalid handle!" << std::endl;
        return -1;
    };

    // initializing size: needed for using Process32First
    pe.dwSize = sizeof(PROCESSENTRY32);

    // info about first process encountered in a system snapshot
    hResult = Process32First(hSnapshot, &pe);

    // retrieve information about the processes
    // and exit if unsuccessful
    while(hResult) {
        // if we find the process: return process ID
        if(strcmp(process_name, (const char*)pe.szExeFile) == 0) {
            pid = pe.th32ProcessID;
            break;
        }
        hResult = Process32Next(hSnapshot, &pe);
    }

    // closes an open handle (CreateToolhelp32Snapshot)
    CloseHandle(hSnapshot);
    return pid;
}

HANDLE memory::get_process_handle(int process_id) {
    return OpenProcess(PROCESS_ALL_ACCESS, FALSE, process_id);
}

BOOL memory::write_to_process(HANDLE process, LPVOID address, LPCVOID buffer) {
    BOOL result = FALSE;
    SIZE_T bytes_written = 0;

    result = WriteProcessMemory(process, address, buffer, sizeof(buffer), &bytes_written);
    if(bytes_written == 0)
        std::cout << "WriteProcessMemory wrote 0 bytes!" << std::endl;

    return result;
}

BOOL memory::read_from_process(HANDLE process, LPVOID address, LPVOID* out_buffer) {
    BOOL result = FALSE;
    SIZE_T bytes_read = 0;

    result = ReadProcessMemory(process, address, out_buffer, sizeof(out_buffer), &bytes_read);
    if(bytes_read == 0)
        std::cout << "ReadProcessMemory read 0 bytes!" << std::endl;

    return result;
}