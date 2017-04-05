//#include <windows.h>
//#include <iostream>
//#include <fstream>
//#include <iomanip>
//#include <math.h>
//#include <string>
//#include <stdio.h>
//#include <conio.h>
//#include <time.h>
//#include <tchar.h>
//#include <dos.h>
//#include <stdlib.h>
//#include <cstdio>
//#include <tlhelp32.h>
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
		TEXT("client.exe")//,
		//TEXT("Plotting.exe")
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
	/*
	// DO as long as not PM.shutdown
	while (!PM->ShutDown.Flags.PM == 1)
	{
		if (RM->Terminate == 1) {
			PM->ShutDown.Status = 0xFFFF;
			break;
		}
		cout << PM->HeartBeat.Status << " " << HeartbeatCounter << " " << PM->HeartBeat.Flags.Arduino << " " << PM->HeartBeat.Flags.3D_Camera << endl;
		// Check heartbeats
		if ((PM->HeartBeat.Status & CRITICAL_MASK) != CRITICAL_MASK)
			HeartbeatCounter++;
		else
			HeartbeatCounter = 0;

		if (HeartbeatCounter >= MAX_HB_COUNT)
		{
			PM->ShutDown.Status = 0xFFFF;
			//cout << "HeartbeatCounter >= MAX_HB_COUNT" << endl;
			break;
		}

		if (PM->HeartBeat.Flags.GPS == 0) 
		{
			if (!IsProcessRunning(Units[1]))
			{
				cout << "Attempting Restart: " << Units[1] << endl;
				ZeroMemory(&s[1], sizeof(s[1]));
				s[1].cb = sizeof(s[1]);
				ZeroMemory(&p[1], sizeof(p[1]));
				// Start the child processes.

				if (!CreateProcess(NULL,  // No module name (use command line)
					Units[1],		// Command line
					NULL,			// Process handle not inheritable
					NULL,			// Thread handle not inheritable
					FALSE,			// Set handle inheritance to FALSE
					CREATE_NEW_CONSOLE,				// No creation flags
					NULL,			// Use parent's environment block
					NULL,			// Use parent's starting directory
					&s[1],				// Pointer to STARTUPINFO structure
					&p[1])				// Pointer to PROCESS_INFORMATION structure
					)
				{
					printf("%s failed (%d).\n", Units[1], GetLastError());
					//_getch();
				} else {
					cout << "Successfully Restarted: " << Units[1] << endl;
				}
			}
		}

		// Maintaining watch of heartbeats

		// Force heartbeat to ZERO
		PM->HeartBeat.Status = 0x00;
		// Manage shutdown/start up
		if (_kbhit())
			break;
		// Reset PM.Heartbeat
		PM->HeartBeat.Flags.PM = 1;
		WatchDog();
		Sleep(100);
	}*/
	
	/*PM->ShutDown.Status = 0xFFFF;
	//cout << PM->HeartBeat.Status << " " << PM->ShutDown.Status << " " << HeartbeatCounter << endl;
	*/
	_getch();

	return 0;
}
/*int WatchDog()
{
	return 0;
}*/
