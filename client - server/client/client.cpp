// client.cpp : Defines the entry point for the console application.
//


#include "stdafx.h"
#include "winsock2.h"
#include "conio.h"
#include <stdlib.h>
#include <stdio.h>

#define SERVER_PORT 12345
#define BUF_SIZE 4096 // block transfer size  
#define QUEUE_SIZE 10
#define IPAddress "127.0.0.1" // Local to the system - Loop back address

int _tmain(int argc, _TCHAR* argv[])
{
	WORD		wVersionRequested;
	WSADATA		wsaData;
	SOCKADDR_IN target; //Socket address information
	SOCKET		s;
	int			err;
	int			bytesSent;
	char		buf[12] = "hi hello";
	
	
	while(1) {
	//--- INITIALIZATION -----------------------------------
	wVersionRequested = MAKEWORD( 1, 1 );
	err = WSAStartup( wVersionRequested, &wsaData );

	if ( err != 0 ) {
		printf("WSAStartup error %ld", WSAGetLastError() );
		WSACleanup();
		return false;
	}
	//------------------------------------------------------
	
	//---- Build address structure to bind to socket.--------  
	target.sin_family = AF_INET; // address family Internet
	target.sin_port = htons (SERVER_PORT); //Port to connect on
	target.sin_addr.s_addr = inet_addr (IPAddress); //Target IP
	//--------------------------------------------------------

	
	// ---- create SOCKET--------------------------------------
	s = socket (AF_INET, SOCK_STREAM, IPPROTO_TCP); //Create socket
	if (s == INVALID_SOCKET)
	{
		printf("socket error %ld" , WSAGetLastError() );
		WSACleanup();
		return false; //Couldn't create the socket
	}  
	//---------------------------------------------------------

	
	//---- try CONNECT -----------------------------------------
	if (connect(s, (SOCKADDR *)&target, sizeof(target)) == SOCKET_ERROR)
	{
		printf("connect error %ld", WSAGetLastError() );
		WSACleanup();
		return false; //Couldn't connect
	}
	//-----------------------------------------------------------
	
	//---- SEND bytes -------------------------------------------

	printf("Type your message here:\n");
	gets_s(buf);
	bytesSent = send( s, buf, 12, 0 ); 
	printf( "Bytes Sent: %ld \n", bytesSent );

	//------------------------------------------------------------
	closesocket( s );
	WSACleanup();
	}

	_getche();
	return 0;
}

