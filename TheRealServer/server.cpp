#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdio.h>
#include <stdlib.h>
#include <conio.h>
#include "SMObject.h"
#include "SMStruct.h"

// Need to link with Ws2_32.lib
#pragma comment (lib, "Ws2_32.lib")
// #pragma comment (lib, "Mswsock.lib")

#define DEFAULT_BUFLEN 512
#define DEFAULT_PORT "12345"

int main(void) 
{
    WSADATA wsaData;
    int iResult;

    SOCKET ListenSocket = INVALID_SOCKET;
    SOCKET ClientSocket = INVALID_SOCKET;

    struct addrinfo *result = NULL;
    struct addrinfo hints;

    int iSendResult;
    char recvbuf[DEFAULT_BUFLEN];
    int recvbuflen = DEFAULT_BUFLEN;
    
    // Initialize Winsock
	printf("Initialize Winsock\n");
    iResult = WSAStartup(MAKEWORD(2,2), &wsaData);
    if (iResult != 0) {
        printf("WSAStartup failed with error: %d\n", iResult);
        return 1;
    }

    ZeroMemory(&hints, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;
    hints.ai_flags = AI_PASSIVE;

    // Resolve the server address and port
	printf("Resolve the server address and port\n");
	iResult = getaddrinfo(NULL, DEFAULT_PORT, &hints, &result);
    if ( iResult != 0 ) {
        printf("getaddrinfo failed with error: %d\n", iResult);
        WSACleanup();
        return 1;
    }

    // Create a SOCKET for connecting to server
	printf("Create a SOCKET for connecting to server\n");
	ListenSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
    if (ListenSocket == INVALID_SOCKET) {
        printf("socket failed with error: %ld\n", WSAGetLastError());
        freeaddrinfo(result);
        WSACleanup();
        return 1;
    }

    // Setup the TCP listening socket
	printf("Setup the TCP listening socket\n");
	iResult = bind(ListenSocket, result->ai_addr, (int)result->ai_addrlen);
    if (iResult == SOCKET_ERROR) {
        printf("bind failed with error: %d\n", WSAGetLastError());
        freeaddrinfo(result);
        closesocket(ListenSocket);
        WSACleanup();
        return 1;
    }

    freeaddrinfo(result);

	printf("Listening\n");
	iResult = listen(ListenSocket, SOMAXCONN);
    if (iResult == SOCKET_ERROR) {
        printf("listen failed with error: %d\n", WSAGetLastError());
        closesocket(ListenSocket);
        WSACleanup();
        return 1;
    }

    // Accept a client socket
	printf("Accept a client socket\n");
	ClientSocket = accept(ListenSocket, NULL, NULL);
    if (ClientSocket == INVALID_SOCKET) {
        printf("accept failed with error: %d\n", WSAGetLastError());
        closesocket(ListenSocket);
        WSACleanup();
        return 1;
    }

    // No longer need server socket
	printf("No longer need server socket\n");
	closesocket(ListenSocket);

	int iteration = 0;

    // Receive until the peer shuts down the connection
	printf("Receive until the peer shuts down the connection\n");
	while (!_kbhit()) {
		printf("Iteration %d \n", iteration);
		SMObject CameraObj(TEXT("CameraData"), sizeof(Camera));
		SMObject PMObj(TEXT("ProcessManagement"), sizeof(ProcessManagement));
		// Request access to GPS mem block
		CameraObj.SMAccess();
		PMObj.SMAccess();

		Camera* pCamera = (Camera*)CameraObj.pData;
		ProcessManagement* PM = (ProcessManagement*)PMObj.pData;

		short result[230400];
		for (int i = 0; i < 76800; i++) {
			result[i] = pCamera->verticesX[i];
			result[i + 76800] = pCamera->verticesY[i];
			result[i + 76800 + 76800] = pCamera->verticesZ[i];
		}

		char *cameraBuffer = (char*)result;

		// Echo the buffer back to the sender
		iSendResult = send(ClientSocket, cameraBuffer, sizeof(result), 0);

		if (iSendResult == SOCKET_ERROR) {
			printf("send failed with error: %d\n", WSAGetLastError());
			closesocket(ClientSocket);
			WSACleanup();
			return 1;
		}
		printf("Bytes sent: %d\n", iSendResult);


		iteration++;
		Sleep(500);
    }

    // shutdown the connection since we're done
	printf("shutdown the connection since we're done\n");
	iResult = shutdown(ClientSocket, SD_SEND);
    if (iResult == SOCKET_ERROR) {
        printf("shutdown failed with error: %d\n", WSAGetLastError());
        closesocket(ClientSocket);
        WSACleanup();
        return 1;
    }

    // cleanup
	printf("cleanup\n");
	closesocket(ClientSocket);
    WSACleanup();
	_getch();
    return 0;
}