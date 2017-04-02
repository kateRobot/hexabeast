
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
	int		b, l, on	= 1;
	char	recvbuf[10] = { }; 
	SOCKET	s, sa;
	struct	sockaddr_in channel;  // holds IP address 
	WORD	wVersionRequested;
	WSADATA wsaData;
	int		err;
	int		bytesRecv;
	
	
	//--- INITIALIZATION -----------------------------------
	wVersionRequested = MAKEWORD( 1, 1 );
	err = WSAStartup( wVersionRequested, &wsaData );

	if ( err != 0 ) {
		printf("WSAStartup error %ld", WSAGetLastError() );
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


	// ---- create SOCKET--------------------------------------
	s = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);    
	if (s < 0) {
		printf("socket error %ld",WSAGetLastError() );
		WSACleanup();
		return false;
	}

	setsockopt(s, SOL_SOCKET, SO_REUSEADDR, (char *) &on, sizeof(on)); 
	printf("Socket created\n");
	//---------------------------------------------------------

	//---- BIND socket ----------------------------------------
	b = bind(s, (struct sockaddr *) &channel, sizeof(channel)); 
	if (b < 0) {
		printf("bind error %ld", WSAGetLastError() ); 
		WSACleanup();
		return false;
	}
	//----------------------------------------------------------

	//---- LISTEN socket ----------------------------------------
	l = listen(s, QUEUE_SIZE);                 // specify queue size 
	if (l < 0) {
		printf("listen error %ld",WSAGetLastError() );
		WSACleanup();
		return false;
	}
	printf("Listening...\n");
	//-----------------------------------------------------------

	while (1) {
	//---- ACCEPT connection ------------------------------------
	sa = accept(s, 0, 0);                  // block for connection request  
	if (sa < 0) {
		printf("accept error %ld ", WSAGetLastError() ); 
		WSACleanup();
		return false;
	}
	else {
		printf("Connected to client\n");
		printf("Waiting for data...\n");
	}
	//------------------------------------------------------------
	// Socket is now set up and bound. Wait for connection and process it. 
	
	//---- RECV bytes --------------------------------------------
	bytesRecv = recv( sa, recvbuf, 12, 0 );
	err = WSAGetLastError( );// 10057 = A request to send or receive data was disallowed because the socket is not connected and (when sending on a datagram socket using a sendto call) 
	if ( bytesRecv == 0 || bytesRecv == WSAECONNRESET ) {
      printf( "Connection Closed.\n");
	  WSACleanup();
    }
    printf( " Bytes Recv: %s \n ", recvbuf );
	closesocket( sa );
	//-------------------------------------------------------------
	}
	
	closesocket( s );
	WSACleanup();
	_getche();
	return 0;
}

