#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include<windows.h>
#include<winsock2.h>
#include<ws2tcpip.h>
#include<iphlpapi.h>
#include<stdio.h>

#pragma comment(lib,"Ws2_32.lib")

#define DEFAULT_PORT "27015"
#define DEFAULT_BUFLEN 512
				

int main(int argc, char* argv[]) {
	WSADATA wsaData;
	int control;
	control = WSAStartup(MAKEWORD(2,2),&wsaData) ;
	if(control != 0){
		printf("wsastartup failed: %d",control) ;
		WSACleanup() ;
		return 1 ;
	}
	struct addrinfo *result = NULL,
					*ptr = NULL,
					hints ;

	ZeroMemory(&hints,sizeof(hints)) ;
	hints.ai_family = AF_INET ;
	hints.ai_socktype = SOCK_STREAM ;
	hints.ai_protocol = IPPROTO_TCP ;
	
	control = getaddrinfo(argv[1],DEFAULT_PORT,&hints,&result) ;
	if(control != 0){
		printf("getaddrinfo failed: %d",control) ;
		WSACleanup() ;
		return 1 ;
	}
	
	SOCKET connectSocket = INVALID_SOCKET ;
	
	ptr = result ;
	connectSocket = socket(ptr->ai_family,ptr->ai_socktype,ptr->ai_protocol) ;
	if(connectSocket == INVALID_SOCKET){
		printf("Error at socket():%ld",WSAGetLastError()) ;
		freeaddrinfo(result) ;
		WSACleanup() ;
		return 1 ;
	}
	
	control = connect(connectSocket, ptr->ai_addr ,(int)ptr->ai_addrlen) ;
	if(control == SOCKET_ERROR){
		closesocket(connectSocket) ;
		connectSocket = INVALID_SOCKET ; 
	}
	
	freeaddrinfo(result) ;
	if(connectSocket  == INVALID_SOCKET){
		printf("Unable to connect server.\n") ;
		WSACleanup() ;
		return 1 ;
	}
	
	int recvbuflen = DEFAULT_BUFLEN ;
	const char *sendbuf = "this is a test message. " ;
	char recvbuf[DEFAULT_BUFLEN] ;
	
	control = send(connectSocket,sendbuf,(int) strlen(sendbuf), 0) ;
	if(control = SOCKET_ERROR){
		printf("send failed: %d\n" , WSAGetLastError()) ;
		closesocket(connectSocket) ;
		WSACleanup ;
		return 1 ;
	}
	
	printf("Bytes sent: %ld\n",control) ;
	
	control = shutdown(connectSocket,SD_SEND) ;
	if(control = SOCKET_ERROR){
		printf("shutdown failed: %d\n",WSAGetLastError()) ;
		closesocket(connectSocket) ;
		WSACleanup ;
		return 1 ;
	}
	
	do{
		control = recv(connectSocket, recvbuf, recvbuflen, 0) ;
		if(control > 0)
			printf("Bytes received: %d\n",control) ;
		else if(control == 0)
			printf("Connection closed\n") ;
		else	
			printf("recv failed: %d\n", WSAGetLastError()) ;	
	}while(control > 0) ;
	
	control = shutdown(connectSocket,SD_SEND) ;
	if(control == SOCKET_ERROR){
		printf("Shutdown failed: %d\n", WSAGetLastError()) ;
		closesocket(connectSocket) ;
		WSACleanup() ;
		return 1 ;
	}
	
	closesocket(connectSocket) ;
	WSACleanup() ;
	
	return 0 ;	
}
