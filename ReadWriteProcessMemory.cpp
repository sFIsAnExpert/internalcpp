#include <iostream> 
#include <windows.h>
#include <WinUser.h>
#include <TlHelp32.h>
#include <tchar.h>
#include <vector> 

DWORD GetProcId(const wchar_t* procName) 
{
	DWORD procId = 0;
	HANDLE hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if (hSnap != INVALID_HANDLE_VALUE)
	{
		PROCESSENTRY32W procEntry;
		procEntry.dwSize = sizeof(procEntry);

		if (Process32FirstW(hSnap, &procEntry))
		{
			do
			{
				if (!_wcsicmp(procEntry.szExeFile, procName))
				{
					procId = procEntry.th32ProcessID;
					break;
				}
			} while (Process32NextW(hSnap, &procEntry));

		}
	}
	CloseHandle(hSnap);
	return procId;
}

uintptr_t GetModuleBaseAddress(DWORD procId, const wchar_t* modName) 
{
	uintptr_t modBaseAddr = 0;
	HANDLE hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE | TH32CS_SNAPMODULE32, procId);
	if (hSnap != INVALID_HANDLE_VALUE)
	{
		MODULEENTRY32W modEntry;
		modEntry.dwSize = sizeof(modEntry);
		if (Module32FirstW(hSnap, &modEntry))
		{
			do
			{
				if (!_wcsicmp(modEntry.szModule, modName))
				{
					modBaseAddr = (uintptr_t)modEntry.modBaseAddr;
					break;
				}
			} while (Module32NextW(hSnap, &modEntry));
		}
	}
	CloseHandle(hSnap);
	return modBaseAddr;
}

uintptr_t FindDMAAddy(HANDLE hProc, uintptr_t ptr, std::vector<unsigned int> offsets) 
{
	uintptr_t addr = ptr;
	for (unsigned int i = 0; i < offsets.size(); ++i)
	{
		ReadProcessMemory(hProc, (BYTE*)addr, &addr, sizeof(addr), 0);
		addr += offsets[i];
	}
	return addr;
}

DWORD ProcID = GetProcId(L"Whatever it is.exe"); 
uintptr_t moduleBase = GetModuleBaseAddress(ProcID, L"Whatever.dll"); 
uintptr_t moduleBaseFinal = moduleBase + 0x4201024; 
std::vector<unsigned int> offset1 = { 0x10, 0x20, 0x30 }; 
int main() 
{
	DWORD pid; 
	HANDLE hGame = NULL; 
	HWND hWind = NULL; 
	int i = 0; 
	int memvalue = 0; 
	int newvalue = 1337;

	hWind = GetForegroundWindow(); 
	if (hWind)
	{
		GetWindowThreadProcessId(hWind, &pid); 
		if (pid != NULL)
		{
			hGame = GetCurrentProcess(); 
			if (hGame)
			{
				uintptr_t FinalMemAddress = FindDMAAddy(hGame, moduleBaseFinal, offset1); 
				ReadProcessMemory(hGame, (BYTE*)FinalMemAddress, &memvalue, sizeof(memvalue), nullptr); 
				std::cout << "Value of address is: " << memvalue << std::endl;
				while (1) 
				{
					i++;
					if (GetAsyncKeyState(VK_F12)) 
					{
						WriteProcessMemory(hGame, (BYTE*)FinalMemAddress, &newvalue, sizeof(newvalue), nullptr); 
						MessageBox(NULL, L"You have configured the process memory", L"Success", MB_OKCANCEL);
					}
					Sleep(100);
				}
				
			}
			else
				MessageBox(NULL, L"Could not get access to current process", L"Error", MB_OKCANCEL); 
		}
		else
			MessageBox(NULL, L"Could not get current process ID", L"Error", MB_OKCANCEL); 
	}
	else
		MessageBox(NULL, L"Could not open handle to current window", L"Error", MB_OKCANCEL);
}

