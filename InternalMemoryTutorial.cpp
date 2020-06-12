#include <iostream> 
#include <windows.h>
#include <WinUser.h>


int main() 
{
	DWORD pid; 
	HANDLE hGame = NULL; 
	HWND hWind = NULL; 
	int i = 0; 

	hWind = GetForegroundWindow(); 
	if (hWind)
	{
		pid = GetCurrentProcessId(); 
		if (pid != NULL)
		{
			hGame = GetCurrentProcess(); 
			if (hGame)
			{
				//start your code here
				while (1) 
				{
					i++;
					if (GetAsyncKeyState(VK_F12)) 
					{
						MessageBox(NULL, L"You have made it in the process", L"Success", MB_OKCANCEL);
						std::cout << GetLastError() << std::endl;
					}
					Sleep(100);
				}
				//end your code here
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

