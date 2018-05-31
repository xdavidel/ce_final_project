#include "tcp_listener.h"
#include "Main.h"

std::unordered_map<std::string, IntFunc> fList;

int OnAccept(char* buffer)
{
	IntFunc toRun = fList[buffer];
	if (toRun) return toRun();
	else return Echo(buffer);
}

BOOL WINAPI DllMain(
	HINSTANCE hinstDLL, // handle to DLL module
	DWORD fdwReason, // reason for calling function
	LPVOID lpReserved) // reserved
{

	// Perform actions based on the reason for calling.
	switch (fdwReason)
	{

	case DLL_PROCESS_ATTACH:
		printf("Initialize once for each new process...\n");
		// Initialize once for each new process.
		// Return FALSE to fail DLL load.
		break;

	case DLL_THREAD_ATTACH:
		printf("Do thread-specific initialization...\n");
		// Do thread-specific initialization.
		break;

	case DLL_THREAD_DETACH:
		printf("Do thread-specific cleanup...\n");

		// Do thread-specific cleanup.

		break;


	case DLL_PROCESS_DETACH:
		printf("Perform any necessary cleanup...\n");

		// Perform any necessary cleanup.

		break;
	}

	// Successful DLL_PROCESS_ATTACH.
	return TRUE;
}


void StartConnection()
{
	const int port = 6363;
	fList.emplace(std::make_pair("update", Update));

	tcp_listener listener(OnAccept);
	printf("Starting socket on port %d...\n", port);
	listener.start(port);

	return;
}

const int Echo(char* buffer)
{
	std::cout << buffer << std::endl;
	return -1;
}

const int Update()
{
	std::cout << "Updating..." << std::endl;
	return 0;
}