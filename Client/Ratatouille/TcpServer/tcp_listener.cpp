#pragma comment(lib, "Ws2_32.lib")

#include "tcp_listener.h"
#include <iostream>

#define DEFAULT_BUFLEN 512


tcp_listener::tcp_listener(std::function< int(char*) > callback)
{
	this->callback = callback;

	WSADATA wsa;

	if (WSAStartup(MAKEWORD(2, 2), &wsa) == SOCKET_ERROR)
	{
		std::cerr << "Cannot startup WSA: " << WSAGetLastError() << std::endl;
	}
	else
	{
		listenSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	}
}

void tcp_listener::start(unsigned short port)
{
	if (listenSocket != SOCKET_ERROR)
	{
		SOCKADDR_IN sock_addr;
		sock_addr.sin_port = htons(port);
		sock_addr.sin_family = AF_INET;
		sock_addr.sin_addr.s_addr = htonl(INADDR_ANY);

		if (bind(listenSocket, (SOCKADDR*)&sock_addr, sizeof(sock_addr)) == SOCKET_ERROR)
		{
			std::cerr << "Cannot bind socket on port " << port << std::endl;
			closesocket(listenSocket);

			return;
		}

		if (listen(listenSocket, 10) == SOCKET_ERROR)
		{
			std::cerr << "Cannot listen to socket." << std::endl;
			closesocket(listenSocket);
			return;
		}

		SOCKET clientSocket = accept(listenSocket, NULL, NULL);
		if (clientSocket == INVALID_SOCKET) {
			printf("accept failed with error: %d\n", WSAGetLastError());
			closesocket(clientSocket);
			WSACleanup();
			return;
		}

		// No longer need server socket
		closesocket(listenSocket);

		int recvbuflen = DEFAULT_BUFLEN;

		char sendbuf[] = "this is a test";
		char recvbuf[DEFAULT_BUFLEN] = { 0 };

		int iResult;


		do {
			iResult = recv(clientSocket, recvbuf, recvbuflen, 0);
			if (iResult > 0)
			{
				//printf("Bytes received: %d\n", iResult);
				iResult = callback(recvbuf);

			}
			else if (iResult == 0)
			{
				printf("Connection closed\n");

			}
			else
			{
				printf("recv failed: %d\n", WSAGetLastError());

			}
		} while (iResult > 0);

	}
	else
	{
		std::cerr << "Socket not set." << std::endl;
	}
}