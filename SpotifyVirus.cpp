#include <iostream>
#include <Windows.h>
#include <tlhelp32.h>

DWORD FindProcessId(const std::wstring& processName)
{
	PROCESSENTRY32 processInfo;
	processInfo.dwSize = sizeof(processInfo);

	HANDLE processesSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);
	if (processesSnapshot == INVALID_HANDLE_VALUE) {
		return 0;
	}

	Process32First(processesSnapshot, &processInfo);
	if (!processName.compare(processInfo.szExeFile))
	{
		CloseHandle(processesSnapshot);
		return processInfo.th32ProcessID;
	}

	while (Process32Next(processesSnapshot, &processInfo))
	{
		if (!processName.compare(processInfo.szExeFile))
		{
			CloseHandle(processesSnapshot);
			return processInfo.th32ProcessID;
		}
	}

	CloseHandle(processesSnapshot);
	return 0;
}

HWND GetWindowFromProcessID(DWORD targetProcID)
{
	HWND hwnd = NULL;
	do
	{
		hwnd = FindWindowEx(NULL, hwnd, NULL, NULL);

		DWORD dwProcID = 0;
		GetWindowThreadProcessId(hwnd, &dwProcID);

		if (dwProcID == targetProcID)
			return hwnd;

	} while (hwnd != NULL);

	return NULL;
}

HWND FindSpotify()
{
	DWORD spotifyPID = FindProcessId(L"Spotify.exe");

	if (!spotifyPID)
		return NULL;

	return GetWindowFromProcessID(spotifyPID);
}

void SendCommand(HWND spotify, int command)
{
	SendMessage(spotify, WM_APPCOMMAND, 0, MAKELPARAM(0, command));
}

void HandleInput(HWND spotify)
{
	static bool bPressed = 0; //Flag

	if (GetKeyState(VK_RCONTROL) & 0x8000)
	{
		// Strg UP -> Play/Pause
		if ((GetAsyncKeyState(VK_UP) & 0x8000) && !bPressed) {
			bPressed = true;
			SendCommand(spotify, APPCOMMAND_MEDIA_PLAY_PAUSE);
			//std::cout << "Skipping song" << std::endl;
		}

		// Strg Rechts -> Next
		if ((GetAsyncKeyState(VK_RIGHT) & 0x8000) && !bPressed) {
			bPressed = true;
			SendCommand(spotify, APPCOMMAND_MEDIA_NEXTTRACK);
			//std::cout << "Next song" << std::endl;
		}

		// Strg Links -> Prev
		if ((GetAsyncKeyState(VK_LEFT) & 0x8000) && !bPressed) {
			bPressed = true;
			SendCommand(spotify, APPCOMMAND_MEDIA_PREVIOUSTRACK);
			//std::cout << "Prev song" << std::endl;
		}

		// shady stuff
		if (GetKeyState(VK_UP) == 0 && !((GetAsyncKeyState(VK_LEFT) & 0x8000) || (GetAsyncKeyState(VK_RIGHT) & 0x8000))) {
			bPressed = false;
		}

		if (GetKeyState(VK_RIGHT) == 0 && !((GetAsyncKeyState(VK_UP) & 0x8000) || (GetAsyncKeyState(VK_LEFT) & 0x8000))) {
			bPressed = false;
		}

		if (GetKeyState(VK_LEFT) == 0 && !((GetAsyncKeyState(VK_RIGHT) & 0x8000) || (GetAsyncKeyState(VK_UP) & 0x8000))) {
			bPressed = false;
		}
	}

	if (GetKeyState(VK_RCONTROL) == 0)
	{
		bPressed = false;
	}
}

int main()
{
	ShowWindow(GetConsoleWindow(), SW_HIDE);

	while (true) {
		Sleep(20);

		HWND spotify = FindSpotify();

		if (!spotify)
			continue;

		HandleInput(spotify);
	}
}