#include "memory.hpp"
#include <psapi.h>
#include <tlhelp32.h>
#include "utils.hpp"

int memory::get_proc_id(const std::wstring& process_name) {
    int pid = 0;
    BOOL result;
    HANDLE snapshot_handle;
    PROCESSENTRY32 pe;

    // snapshot of all processes in the system
    snapshot_handle = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if(snapshot_handle == INVALID_HANDLE_VALUE) {
        std::cout << "CreateToolhelp32Snapshot returned an invalid handle!" << std::endl;
        return -1;
    };

    // initializing size: needed for using Process32First
    pe.dwSize = sizeof(PROCESSENTRY32);

    // info about first process encountered in a system snapshot
    result = Process32First(snapshot_handle, &pe);

    // retrieve information about the processes
    // and exit if unsuccessful
    while(result) {
        // use wstring to compare
        std::wstring temp(pe.szExeFile);
        // if we find the process: return process ID
        if(process_name.compare(temp) == 0) {
            pid = pe.th32ProcessID;
            break;
        }
        result = Process32Next(snapshot_handle, &pe);
    }

    if(pid == 0)
        std::cout << "Could not find process \"" << utils::ws2s(process_name) << "\"." << GetLastError() << std::endl;

    // closes an open handle (CreateToolhelp32Snapshot)
    CloseHandle(snapshot_handle);
    return pid;
}

HANDLE memory::get_process_handle(int process_id) {
    HANDLE h = OpenProcess(PROCESS_ALL_ACCESS, FALSE, process_id);
    if(!h)
        std::cout << "Could not get handle for pid: " << process_id << ". " << GetLastError() << std::endl;

    return h;
}

BOOL memory::set_debug_privilege(BOOL b) {
    LUID privilege_id;
    HANDLE token_handle;

    if(!OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES, &token_handle)) {
        std::cout << "Could not open process token for current process." << std::endl;
        return FALSE;
    }

    if(!LookupPrivilegeValue(NULL, SE_DEBUG_NAME, &privilege_id)) {
        std::cout << "Could not lookup SE_DEBUG privilege" << std::endl;
        return FALSE;
    }

    TOKEN_PRIVILEGES token_privilege;
    token_privilege.PrivilegeCount = 1;
    token_privilege.Privileges[0].Luid = privilege_id;
    token_privilege.Privileges[0].Attributes = b ? SE_PRIVILEGE_ENABLED : 0;

    if(!AdjustTokenPrivileges(token_handle, FALSE, &token_privilege, sizeof(TOKEN_PRIVILEGES), NULL, NULL)) {
        std::cout << "Could not adjust privilege for token." << std::endl;
        return FALSE;
    }

    return TRUE;
}

//HMODULE memory::get_process_base_address(HANDLE process, const std::string& process_name) {
//    CHAR image_name[MAX_PATH];
//    int len = GetProcessImageFileNameA(process, image_name, MAX_PATH);
//
//    if(len == 0) {
//        std::cout << "Could not find image name for process handle. " << GetLastError() << std::endl;
//        return nullptr;
//    }
//
//    //https://stackoverflow.com/questions/4298331/exe-or-dll-image-base-address
//    // enumerate all modules
//    DWORD bytes_needed;
//    HMODULE modules[1024];
//    BOOL result = EnumProcessModules(process, modules, sizeof(modules), &bytes_needed);
//    if(!result) {
//        std::cout << "Could not enumerate process modules for process handle. " << GetLastError() << std::endl;
//        return nullptr;
//    }
//
//    for(int i = 0; i < (bytes_needed / sizeof(HMODULE)); i++) {
//        TCHAR module_name[MAX_PATH];
//        GetModuleFileNameEx(process, modules[i], module_name, sizeof(module_name) / sizeof(TCHAR));
//
//        std::string temp = utils::ws2s(module_name);
//
//        int starting_pos = temp.size() - process_name.size();
//        if(temp.compare(starting_pos, std::string::npos, process_name))
//            return modules[i];
//    }
//
//    std::cout << "Could not find base address for process: " << process_name << ". " << GetLastError() << std::endl;
//    return nullptr;
//}