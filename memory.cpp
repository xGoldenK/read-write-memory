#include "memory.hpp"
#include <tlhelp32.h>

int memory::get_proc_id(const std::wstring& process_name) {
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
        // use wstring to compare
        std::wstring temp(pe.szExeFile);
        // if we find the process: return process ID
        if(process_name.compare(temp) == 0) {
            pid = pe.th32ProcessID;
            break;
        }
        hResult = Process32Next(hSnapshot, &pe);
    }

    if(pid == 0) {
        std::cout << "Could not find process \"" << process_name.c_str() << "\"." << std::endl;
        std::cout << "Last error: " << GetLastError() << std::endl;
    }

    // closes an open handle (CreateToolhelp32Snapshot)
    CloseHandle(hSnapshot);
    return pid;
}

HANDLE memory::get_process_handle(int process_id) {
    HANDLE h = OpenProcess(PROCESS_CREATE_PROCESS, FALSE, process_id);
    if(!h) {
        std::cout << "Could not get handle for pid: " << process_id << "." << std::endl;
        std::cout << "Last error: " << GetLastError() << std::endl;
    }

    return h;
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

BOOL memory::set_debug_privilege(BOOL b) {
    LUID privilegeId;
    HANDLE hToken;

    if(!OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES, &hToken)) {
        std::cout << "Could not open process token for current process." << std::endl;
        return FALSE;
    }

    if(!LookupPrivilegeValue(NULL, SE_DEBUG_NAME, &privilegeId)) {
        std::cout << "Could not lookup SE_DEBUG privilege" << std::endl;
        return FALSE;
    }

    TOKEN_PRIVILEGES tokenPriv;
    tokenPriv.PrivilegeCount = 1;
    tokenPriv.Privileges[0].Luid = privilegeId;
    tokenPriv.Privileges[0].Attributes = b ? SE_PRIVILEGE_ENABLED : 0;

    if(!AdjustTokenPrivileges(hToken, FALSE, &tokenPriv, sizeof(TOKEN_PRIVILEGES), NULL, NULL)) {
        std::cout << "Could not adjust privilege for token." << std::endl;
        return FALSE;
    }

    return TRUE;
}