#include "proc.h"
#include "icl.h"

DWORD GetProcId(const wchar_t* procName){
	DWORD procId = 0;
	HANDLE hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if (hSnap != INVALID_HANDLE_VALUE) {
		PROCESSENTRY32 procEntry;

		procEntry.dwSize = sizeof(procEntry);

		if (Process32First(hSnap, &procEntry)) {


			do
			{
				if (!_wcsicmp(procEntry.szExeFile, procName)) {
					procId = procEntry.th32ProcessID;
					break;
				}

			} while (Process32Next(hSnap, &procEntry));
		}
	}
	CloseHandle(hSnap);
	return procId;
}

uintptr_t GetModuleBaseAddress(DWORD procId, const wchar_t* modName){
	uintptr_t modBaseAddr = 0;
	HANDLE hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE | TH32CS_SNAPMODULE32, procId);
	if (hSnap != INVALID_HANDLE_VALUE) {
		MODULEENTRY32 modEntry;
		modEntry.dwSize = sizeof(modEntry);
		if (Module32First(hSnap, &modEntry)) {
			do
			{
				if (!_wcsicmp(modEntry.szModule, modName)) {
					modBaseAddr = (uintptr_t)modEntry.modBaseAddr;
					break;
				}

			} while (Module32Next(hSnap, &modEntry));
		}
	}
	CloseHandle(hSnap);
	return modBaseAddr;
}

uintptr_t FindDMAAddy(HANDLE hProc, uintptr_t ptr, std::vector<unsigned int> offsets){
	uintptr_t addr = ptr;
	for (unsigned int i2 = 0; i2 < offsets.size(); ++i2) {
		ReadProcessMemory(hProc, (BYTE*)addr, &addr, sizeof(addr), 0);
		addr += offsets[i2];
	}
	return addr;
}

uintptr_t tdm;
uintptr_t tdm1 = 0;
int Adpe = 0;

uintptr_t FindDMAAddy32(HANDLE hProc, uintptr_t ptr, std::vector<unsigned int> offsets) {
	tdm = ptr;
	tdm1 = 0;
    Adpe = 0;
	ReadProcessMemory(hProc, (BYTE*)tdm, &tdm1, sizeof(tdm1), 0);
	Adpe = tdm1;
	ReadProcessMemory(hProc, (BYTE*)Adpe + offsets[0], &tdm1, sizeof(tdm1), 0);
	Adpe = tdm1;
	for (unsigned int i2 = 1; i2 < offsets.size(); ++i2) {
	    ReadProcessMemory(hProc, (BYTE*)Adpe + offsets[i2], &tdm1, sizeof(tdm1), 0);
		Adpe = tdm1;
	}
	return Adpe;
}



DWORD GetNthChildProcessId(DWORD parentProcessId, int n) {
	HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if (hSnapshot == INVALID_HANDLE_VALUE) {
		std::cerr << "Failed to create snapshot of running processes" << std::endl;
		return 0;
	}

	PROCESSENTRY32 pe32;
	pe32.dwSize = sizeof(pe32);
	DWORD childProcessId = 0;
	int childCount = 0;
	if (Process32First(hSnapshot, &pe32)) {
		do {
			if (pe32.th32ParentProcessID == parentProcessId) {
				childCount++;
				if (childCount == n) {
					childProcessId = pe32.th32ProcessID;
					break;
				}
			}
		} while (Process32Next(hSnapshot, &pe32));
	}

	CloseHandle(hSnapshot);
	return childProcessId;
}
