#include <cstdio>
#include <windows.h>
#include <tlhelp32.h>
#include <iostream>
#include <thread>        
#include <chrono> 
#include <tchar.h>

BOOL IsProcessRunning(wchar_t* processName) {
	PROCESSENTRY32 entry;
	entry.dwSize = sizeof(PROCESSENTRY32);

	HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);

	BOOL exists = false;

	if (Process32First(snapshot, &entry) == TRUE)
	{
		while (Process32Next(snapshot, &entry) == TRUE)
		{
			wchar_t* exeFile = _wcslwr(entry.szExeFile);
			if (_tcscmp(exeFile, _wcslwr(processName)) == 0)
			{
				exists = true;
			}
		}
	}

	CloseHandle(snapshot);
	return exists;
}

BOOL ChangeResolution(int deviceID, int width, int height) {
	DISPLAY_DEVICE dd;
	dd.cb = sizeof(DISPLAY_DEVICE);
	if (!EnumDisplayDevices(NULL, deviceID, &dd, 0))
		return false;

	DEVMODE dm;
	dm.dmSize = sizeof(DEVMODE);
	if (!EnumDisplaySettings(dd.DeviceName, ENUM_CURRENT_SETTINGS, &dm))
		return false;
	
	dm.dmPelsWidth = width;
	dm.dmPelsHeight = height;
	dm.dmFields = DM_PELSWIDTH | DM_PELSHEIGHT;

	LONG ret = ChangeDisplaySettings(&dm, CDS_TEST);
	if (ret == DISP_CHANGE_SUCCESSFUL)
		ret = ChangeDisplaySettings(&dm, CDS_UPDATEREGISTRY);

	return ret == DISP_CHANGE_SUCCESSFUL;
}

// int main()
// Windows SUBSYSTEM Entry
int APIENTRY _tWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nCmdShow)
{
	if (!ChangeResolution(0, 1280, 960))
		printf("Failed to change resolution");

	wchar_t CSGO[36];
	std::wcsncpy(CSGO, L"csgo.exe", 36);

	if (!IsProcessRunning(CSGO)) {
		system("START steam://rungameid/730");
		while (!IsProcessRunning(CSGO))
			std::this_thread::sleep_for(std::chrono::seconds(2));
	}
	
	while (IsProcessRunning(CSGO))
		std::this_thread::sleep_for(std::chrono::seconds(5));

	ChangeResolution(0, 1920, 1080);
	return 0;
}
