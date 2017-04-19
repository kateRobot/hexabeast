#include <windows.h>
#include <iostream>
#include <fstream>
#include <iomanip>
#include <math.h>
#include <string>
#include <stdio.h>
#include <conio.h>
#include <time.h>
#include <tchar.h>
#include <dos.h>
#include <stdlib.h>
#include <cstdio>
#include <tlhelp32.h>
#include <SMObject.h>
#include <SMStruct.h>
#include <conio.h>

#define NUM_UNITS 3
#define CRITICAL_MASK 62
#define MAX_HB_COUNT 100

using namespace std;

bool IsProcessRunning(const char *processName)
{
	bool exists = false;
	PROCESSENTRY32 entry;
	entry.dwSize = sizeof(PROCESSENTRY32);

	HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);

	if (Process32First(snapshot, &entry))
		while (Process32Next(snapshot, &entry))
			if (!_stricmp(entry.szExeFile, processName))
				exists = true;

	CloseHandle(snapshot);
	return exists;
}

int WatchDog();

int main()
{
	int HeartbeatCounter = 0;
	STARTUPINFO s[10];
	PROCESS_INFORMATION p[10];
	// Request access to PM mem block
	SMObject ArduinoObj(TEXT("ArduinoData"), sizeof(Arduino));
	SMObject CameraObj(TEXT("CameraData"), sizeof(Camera));
	SMObject PMObj(TEXT("ProcessManagement"), sizeof(ProcessManagement));

	// Create SM objects
	ArduinoObj.SMCreate();
	CameraObj.SMCreate();
	PMObj.SMCreate();

	// Request access to mem block
	ArduinoObj.SMAccess();
	CameraObj.SMAccess();
	PMObj.SMAccess();

	Arduino* pArduino = (Arduino*)ArduinoObj.pData;
	Camera* pCamera = (Camera*)CameraObj.pData;
	ProcessManagement* PM = (ProcessManagement*)PMObj.pData;

	// Start up all modules
	// Start up sequence
	TCHAR* Units[10] = //Max processes is 10
	{
		TEXT("Arduino.exe"),
		TEXT("3D_Camera.exe"),
		TEXT("TheRealServer.exe")
	};
	//Run all other processes ======
	for (int i = 0; i < NUM_UNITS; i++)
	{
		// Check if each process is running
		if (!IsProcessRunning(Units[i]))
		{
			ZeroMemory(&s[i], sizeof(s[i]));
			s[i].cb = sizeof(s[i]);
			ZeroMemory(&p[i], sizeof(p[i]));
			// Start the child processes.

			if (!CreateProcess(NULL,  // No module name (use command line)
				Units[i],		// Command line
				NULL,			// Process handle not inheritable
				NULL,			// Thread handle not inheritable
				FALSE,			// Set handle inheritance to FALSE
				CREATE_NEW_CONSOLE,				// No creation flags
				NULL,			// Use parent's environment block
				NULL,			// Use parent's starting directory
				&s[i],				// Pointer to STARTUPINFO structure
				&p[i])				// Pointer to PROCESS_INFORMATION structure
				)
			{
				printf("%s failed (%d).\n", Units[i], GetLastError());
				//_getch();
				return -1;
			}
		}
		cout << "Started: " << Units[i] << endl;
		Sleep(100);	// Any other initialization
	}
	cout << "All processes done" << endl;
	PM->ShutDown.Flags.PM = 0;
	
	_getch();

	return 0;
}
