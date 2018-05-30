#pragma once
#include <WinSock2.h>
#include <thread>
#include <iostream>


class tcp_listener
{
public:
	tcp_listener(std::function< int(char*) > callback);
	void start(unsigned short port);
private:
	SOCKET listenSocket;
	std::thread accept_thread;

	std::function< int(char*) > callback;
};