
#include "stdafx.h"
#include "winsock2.h"
#include "conio.h"
#include <stdlib.h>
#include <stdio.h>

#define SERVER_PORT 12345
#define BUF_SIZE 4096  // block transfer size  
#define QUEUE_SIZE 10 


int _tmain(int argc, _TCHAR* argv[])
{
	int		b1, b2, on = 1;
	int		listenToCamera, listenToArduino = 1;
	char	recvbuf[16] = {};
	SOCKET	s1, s2, sa1, sa2;
	struct	sockaddr_in channel;  // holds IP address 
	WORD	wVersionRequested;
	WSADATA wsaData;
	int		err;
	int		bytesRecv;


	//--- INITIALIZATION -----------------------------------
	wVersionRequested = MAKEWORD(1, 1);
	err = WSAStartup(wVersionRequested, &wsaData);

	if (err != 0) {
		printf("WSAStartup error %ld", WSAGetLastError());
		WSACleanup();
		return false;
	}
	printf("Connection initialised\n");
	//------------------------------------------------------



	//---- Build address structure to bind to socket.--------  
	memset(&channel, 0, sizeof(channel));// zerochannel 
	channel.sin_family = AF_INET;
	channel.sin_addr.s_addr = htonl(INADDR_ANY);
	channel.sin_port = htons(SERVER_PORT);
	//--------------------------------------------------------


	// ---- create SOCKET--for Arduino--------------------
	s1 = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (s1 < 0) {
		printf("socket error %ld", WSAGetLastError());
		WSACleanup();
		return false;
	}

	setsockopt(s1, SOL_SOCKET, SO_REUSEADDR, (char *)&on, sizeof(on));
	printf("Socket no.1 created\n");


	// ---- create SOCKET--for Camera--------------------
	s2 = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (s2 < 0) {
		printf("socket error %ld", WSAGetLastError());
		WSACleanup();
		return false;
	}

	setsockopt(s2, SOL_SOCKET, SO_REUSEADDR, (char *)&on, sizeof(on));
	printf("Socket no.2 created\n");
	//---------------------------------------------------------

	//---- BIND socket ----------------------------------------
	b1 = bind(s1, (struct sockaddr *) &channel, sizeof(channel));
	b2 = bind(s2, (struct sockaddr *) &channel, sizeof(channel));
	if ((b1 < 0) || (b2 < 0)) {
		printf("bind error %ld", WSAGetLastError());
		WSACleanup();
		return false;
	}
	//----------------------------------------------------------

	//---- LISTEN socket ----------------------------------------
	listenToArduino = listen(s1, QUEUE_SIZE);                 // specify queue size 
	listenToCamera = listen(s2, QUEUE_SIZE);
	if (listenToArduino < 0 || listenToCamera < 0) {
		printf("listen error %ld", WSAGetLastError());
		WSACleanup();
		return false;
	}
	printf("Both sockets listening...\n");
	//-----------------------------------------------------------



	while (1) {
		//---- ACCEPT connection ---Arduino---------------------
		sa1 = accept(s1, 0, 0);                  // block for connection request  
		if (sa1 < 0) {
			printf("accept error %ld ", WSAGetLastError());
			WSACleanup();
			return false;
		}
		else {
			printf("Connected to Arduino\n");
			printf("Waiting for data...\n");
		}

		//---- ACCEPT connection ----Camera------------------------
		sa2 = accept(s2, 0, 0);                  // block for connection request  
		if (sa2 < 0) {
			printf("accept error %ld ", WSAGetLastError());
			WSACleanup();
			return false;
		}
		else {
			printf("Connected to camera\n");
			printf("Waiting for data...\n");
		}


		//------------------------------------------------------------
		// Sockets now set up and bound. Wait for connections and process them. 

		//---- RECV bytes -----Arduino-------------------------
		bytesRecv = recv(sa1, recvbuf, 38400, 0);
		err = WSAGetLastError();// 10057 = A request to send or receive data was disallowed because the socket is not connected and (when sending on a datagram socket using a sendto call) 
		if (bytesRecv == 0 || bytesRecv == WSAECONNRESET) {
			printf("Connection Closed.\n");
			WSACleanup();
		}
		printf(" Bytes Recv: %s \n ", recvbuf);
		closesocket(sa1);
		//-------------------------------------------------------------


		//---- RECV bytes ------Camera----------------------------
		bytesRecv = recv(sa2, recvbuf, 38400, 0);
		err = WSAGetLastError();// 10057 = A request to send or receive data was disallowed because the socket is not connected and (when sending on a datagram socket using a sendto call) 
		if (bytesRecv == 0 || bytesRecv == WSAECONNRESET) {
			printf("Connection Closed.\n");
			WSACleanup();
		}
		printf(" Bytes Recv: %s \n ", recvbuf);
		closesocket(sa2);
		//-------------------------------------------------------------
	}
	closesocket( s1 );
	closesocket(s2);
	WSACleanup();
	_getche();
	return 0;


}

