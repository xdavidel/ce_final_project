#pragma comment(lib, "Ws2_32.lib")

#include "tcp_listener.h"
#include <iostream>

#define MAX_BUFFER_LENGTH 200000

tcp_listener::tcp_listener(std::function< int(char*) > callback, char* outBuffer)
{
	this->callback = callback;
	this->outBuffer = outBuffer;

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

		int recvbuflen = MAX_BUFFER_LENGTH;

		int iResult;


		do 
		{
			char recvbuf[MAX_BUFFER_LENGTH] = { 0 };
			iResult = recv(clientSocket, recvbuf, recvbuflen, 0);
			// Data received
			if (iResult > 0)
			{
				int n_Length = *(int*)(recvbuf + 1);
				if (n_Length < MAX_BUFFER_LENGTH)
				{

					iResult = callback(recvbuf);
					if (-2 == iResult)
					{
						sendto(clientSocket, outBuffer, strlen(outBuffer), 0, (SOCKADDR*)&sock_addr, sizeof(sock_addr));
					}
				}
				//printf("Bytes received: %d\n", iResult);

			}
			// No data => close connection
			else if (iResult == 0)
			{
				printf("Connection closed\n");

			}
			else
			{
				printf("recv failed: %d\n", WSAGetLastError());

			}
		} while (iResult != 0);

		closesocket(clientSocket);

	}
	else
	{
		std::cerr << "Socket not set." << std::endl;
	}
}