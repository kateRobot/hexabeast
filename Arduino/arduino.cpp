#define _USE_MATH_DEFINES
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include <string>
#include <iostream>
#include <sstream>
#include <math.h>
#include <stdio.h>
#include <tchar.h>
#include "SerialClass.h"
#include "arduino.h"

using namespace std;

Microprocessor::Microprocessor() {
	
}

Microprocessor::~Microprocessor() {

}

void Microprocessor::Get_Data() {

	Serial* SP = new Serial("\\\\.\\COM4");    // adjust as needed

											   //if (SP->IsConnected())
											   //printf("We're connected\n");
	
	if (SP->IsConnected()) {
		connection = true;
	}
	cout << "3" << endl;
	//printf("Welcome to the serial test app!\n\n");

	char incomingData[256] = "";			// don't forget to pre-allocate memory
											//printf("%s\n",incomingData);
	int dataLength = 255;
	int readResult = 0;

	ArduinoData ArduinoRecord;

	//while (connection != 0) {
		readResult = SP->ReadData(incomingData, dataLength);
		cout << "hia" << endl;
		// printf("Bytes read: (0 means no data available) %i\n",readResult);
		incomingData[readResult] = 0;
		istringstream iss(incomingData);

		cout << "hib" << endl;
		while (iss) {
			if (iss >> ArduinoRecord.pin[0] >> ArduinoRecord.pin[1] >> ArduinoRecord.pin[2])
				;
		}
		//printf("%d %d %d\n", ArduinoRecord.pin[0], ArduinoRecord.pin[1], ArduinoRecord.pin[2]);
		ArduinoRecord.instance++;
		//printf("%s",incomingData);

		//Sleep(1000);
		if (SP->IsConnected() != true) {
			connection = false;
		}
	//}
	cout << "hic" << endl;

	// Filling in Entire structure
	for (int i = 0; i < 3; i++)
		pin[i] = ArduinoRecord.pin[i];

	instance = ArduinoRecord.instance;

	cout << pin[0] << " " << pin[1] << " " << pin[2] << " " << instance << endl;
	cout << "hid" << endl;

}