// client.cpp : Defines the entry point for the console application.
//


#include "stdafx.h"
#include "winsock2.h"
#include <windows.h>
#include <stdlib.h>
#include <stdio.h>
#include <fstream>
#include "conio.h"
#include <SMObject.h>
#include <SMStruct.h>
#include <vector>
#include <exception>


#define NEW_HEIGHT		120
#define NEW_WIDTH		160
#define SERVER_PORT 12345
#define BUF_SIZE 4096 // block transfer size  
#define QUEUE_SIZE 10
#define IPAddress "127.0.0.1" // Local to the system - Loop back address

using namespace std;

int _tmain(int argc, _TCHAR* argv[])
{
	WORD		wVersionRequested;
	WSADATA		wsaData;
	SOCKADDR_IN target; //Socket address information
	SOCKET		s;
	int			err;
	int			bytesSent1;
	int			bytesSent2;
	char		buf[16] = "hi hello";
	
	printf("FLAG A\n");
	SMObject CameraObj(TEXT("CameraData"), sizeof(Camera));
	SMObject ArduinoObj(TEXT("ArduinoData"), sizeof(Arduino));
	SMObject PMObj(TEXT("ProcessManagement"), sizeof(ProcessManagement));
	// Request access to GPS mem block
	CameraObj.SMAccess();
	ArduinoObj.SMAccess();
	PMObj.SMAccess();

	printf("FLAG B\n");
	//HWND hWnd = GetConsoleWindow();
	//ShowWindow(hWnd, SW_HIDE);// SW_SHOWMINIMIZED);//SW_SHOW, SW_HIDE

	ProcessManagement* PM = (ProcessManagement*)PMObj.pData;
	Arduino* pArduino = (Arduino*)ArduinoObj.pData;
	Camera* pCamera = (Camera*)CameraObj.pData;

	//PM->ShutDown.Flags.Camera = 0;
	while (1) {
		printf("FLAG C\n");

		//--- INITIALIZATION -----------------------------------
		wVersionRequested = MAKEWORD(1, 1);
		err = WSAStartup(wVersionRequested, &wsaData);

		if (err != 0) {
			printf("WSAStartup error %ld", WSAGetLastError());
			WSACleanup();
			return false;
		}
		//------------------------------------------------------

		//---- Build address structure to bind to socket.--------  
		target.sin_family = AF_INET; // address family Internet
		target.sin_port = htons(SERVER_PORT); //Port to connect on
		target.sin_addr.s_addr = inet_addr(IPAddress); //Target IP
		//--------------------------------------------------------


		// ---- create SOCKET--------------------------------------
		s = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP); //Create socket
		if (s == INVALID_SOCKET)
		{
			printf("socket error %ld", WSAGetLastError());
			WSACleanup();
			return false; //Couldn't create the socket
		}
		//---------------------------------------------------------


		//---- try CONNECT -----------------------------------------
		if (connect(s, (SOCKADDR *)&target, sizeof(target)) == SOCKET_ERROR)
		{
			printf("connect error %ld", WSAGetLastError());
			WSACleanup();
			return false; //Couldn't connect
		}
		//-----------------------------------------------------------
		printf("FLAG D\n");

		//---- SEND bytes -------------------------------------------
		//while (s != INVALID_SOCKET)
		//{
			short depth[120][160];
			for (int i = 0; i < 120; i++) {
				for (int j = 0; j < 160; j++) {
					depth[i][j] = (short)pCamera->depth[i][j];
				}
			}
			printf("FLAG D2\n");

			/*char *DEPTHMAP = NULL;
			char *PINS = NULL;
			memcpy((char *)(DEPTHMAP), (char *)&depth, sizeof(depth));
			printf("FLAG E\n");
			*/
			int pin[3];
			for (int i = 0; i < 3; i++) {
				pin[i] = pArduino->pin[i];
			}
			printf("FLAG F\n");

			// send bytes in a data dump
			bytesSent1 = send(s, (char *)depth, sizeof(depth), 0);
			bytesSent2 = send(s, (char *)pin, sizeof(pin), 0);

			// verify bytes were sent
			// returns 1 (message sent), 0 (no message), -1 (?)
			printf("Camera Bytes Sent: %ld \n", bytesSent1);
			printf("Arduino Bytes Sent: %ld \n", bytesSent2);

			// delay
			Sleep(1000);

		//}

		//------------------------------------------------------------
		closesocket(s);
		WSACleanup();
	}

	_getche();
	return 0;
}

