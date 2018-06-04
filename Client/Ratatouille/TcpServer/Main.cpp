#include "tcp_listener.h"
#include "Main.h"

BOOL WINAPI DllMain(
	HINSTANCE hinstDLL,		// handle to DLL module
	DWORD fdwReason,		// reason for calling function
	LPVOID lpReserved)		// reserved
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

/**
After a command and execute the proper function in response
Returns the result of the proper requested function.

@param buffer: The command requested
@return Tthe result of the proper requested function.
*/
int OnAccept(char* buffer)
{
	if (buffer[0] == 'u')
	{
		return RequestUpdate(buffer);
	}
	else if (expectUpdate)
	{
		return Update(buffer);
	}
	else
	{
		IntFunc toRun = fList[buffer];
		if (toRun) return toRun();
		else return Echo(buffer);
	}
}

/**
Starting a new tcp connection and setting a socket om port 6363
*/
void StartConnection()
{
	const int port = 6363;
	expectUpdate = false;

	//fList.emplace(std::make_pair("echo", Echo));

	tcp_listener listener(OnAccept);
	printf("Starting socket on port %d...\n", port);
	listener.start(port);

	return;
}


/**
@param buffer: An output buffer that will return the full path of the executable
Returns the full path of the executable.
*/
void GetExecutablePath(char* buffer)
{	
	GetModuleFileName(NULL, buffer, MAX_PATH);
	std::string::size_type pos = std::string(buffer).find_last_of("\\/");
	//std::string(buffer).substr(0, pos + 1);

	buffer[pos + 1] = '\0';
}

/**
Prints out the data received in the buffer

@param buffer: The data to echo out
@return -1
*/
const int Echo(char* buffer)
{
	std::cout << buffer << std::endl;
	return -1;
}

/**
Set a flag to prepare to receive a binary data

@return -1.
*/
const int RequestUpdate(char* buffer)
{
	std::cout << "Expecting Update..." << std::endl;
	expectUpdate = true;

	// set the dll size in bytes
	downloadSize = atoi(buffer + 1);

	return -1;
}

/**
After receiving an update command - digest a binary data and create a DLL file
Returns 0 if operation failed along the way.

@param buffer: The data to convert into DLL
@return 0 for a failure.
*/
const int Update(char* buffer)
{
	char exePath[MAX_PATH];

	GetExecutablePath(exePath);
	std::string filePath(exePath);
	filePath.append("TcpServer.dll.new");

	// Saving binary data as TcpServer.dll.new
	auto hFile = CreateFile(
		filePath.c_str(),				// lpFileName,
		GENERIC_READ | GENERIC_WRITE,	// dwDesiredAccess,
		0,								// dwShareMode,
		NULL,							// lpSecurityAttributes,
		CREATE_ALWAYS,					// dwCreationDisposition,
		FILE_ATTRIBUTE_NORMAL,			// dwFlagsAndAttributes,
		NULL							// hTemplateFile
	);

	// if file could not be created
	if (!hFile) { return -1; }

	if (WriteFile(
		hFile,							// file handler
		buffer,							// data
		downloadSize,					// nNumberOfBytesToWrite 
		NULL,							// lpNumberOfBytesWritten 
		NULL							// lpOverlapped 
	)) 
	{
		// write succeded

		auto e = GetLastError();

		expectUpdate = false;
		CloseHandle(hFile);
		return 0;
	}
	else 
	{
		// writing failed
		CloseHandle(hFile);
		return -1;
	}

}