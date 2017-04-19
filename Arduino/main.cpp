#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <sstream>
#include <iostream>
#include <fstream>
#include <tchar.h>
#include "SerialClass.h"
#include <SMObject.h>
#include <SMStruct.h>
#include <conio.h>

using namespace std;

int main()
{
	int connection = false;
	int pin[3] = {};
	double instance = 0;

	SMObject ArduinoObj(TEXT("ArduinoData"), sizeof(Arduino));
	SMObject PMObj(TEXT("ProcessManagement"), sizeof(ProcessManagement));
	// Request access to GPS mem block
	ArduinoObj.SMAccess();
	PMObj.SMAccess();
	

	ProcessManagement* PM = (ProcessManagement*)PMObj.pData;
	Arduino* pArduino = (Arduino*)ArduinoObj.pData;
	Serial* SP = new Serial("\\\\.\\COM4");


	PM->ShutDown.Flags.Arduino = 0;
	// DO as long as not GPS.shutdown
	while (PM->ShutDown.Flags.Arduino != 1)
	{
		if (SP->IsConnected()) {
			connection = true;
		}
		
		char incomingData[256] = "";			
		int dataLength = 255;
		int readResult = 0;

		while (connection != 0) {
			readResult = SP->ReadData(incomingData, dataLength);
			incomingData[readResult] = 0;
			istringstream iss(incomingData);

			while (iss) {
				if (iss >> pin[0] >> pin[1] >> pin[2])
					;
			}
			instance++;

			if (SP->IsConnected() != true) {
				connection = false;
			}
			
			// Filling in Entire structure
			for (int i = 0; i < 3; i++) {
				pArduino->pin[i] = pin[i];
			}
			pArduino->instance = instance;

			cout << pArduino->pin[0] << " " << pArduino->pin[1] << " " << pArduino->pin[2] << " " << pArduino->instance << endl;

			Sleep(20);
		}
		
		PM->HeartBeat.Flags.Arduino = 1;
		if (_kbhit())
			break;
}

	return 0;
}