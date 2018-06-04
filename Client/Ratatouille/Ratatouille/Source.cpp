#include "Source.h"

#define START_CCONNECTION "StartConnection"
#define EXECUTION_PATH "GetExecutablePath"

int main(int argc, char **argv, char **envp)
{
	Func startTcp;

	while (true)
	{
		// Trying to load DLL
		hInst = LoadLibrary(DLL_LIBRARY_NAME);
		if (!hInst) return LoadingError(DLL_LIBRARY_NAME);

		// Getting the Current Path function from DLL
		GetExecutablePath = (CharArrayFunc)GetProcAddress(hInst, EXECUTION_PATH);
		if (!GetExecutablePath) return LoadingError(EXECUTION_PATH);
		GetExecutablePath(executionPath);

		// Starting the tcp socket connection
		startTcp = (Func)GetProcAddress(hInst, START_CCONNECTION);
		if (!startTcp) return LoadingError(START_CCONNECTION);

		startTcp();

		FreeLibrary(hInst);
		ReplaceDll();
	}

	return 0;
}

int LoadingError(const char* functionName)
{
	std::cout << "Could not locate '" << functionName << "'" << std::endl;
	return EXIT_FAILURE;
}

void ReplaceDll()
{
	char dllFullPath[MAX_PATH];
	char dllBackupPath[MAX_PATH];
	char dllNewPath[MAX_PATH];

	{
		std::string path(executionPath);
		path.append(DLL_LIBRARY_NAME);
		strcpy_s(dllFullPath, path.c_str());
		path.append(".bak");
		strcpy_s(dllBackupPath, path.c_str());
		MoveFileEx(
			dllFullPath,					// lpExistingFileName
			dllBackupPath,					// lpNewFileName
			MOVEFILE_REPLACE_EXISTING		// dwFlags
		);
	}


	if (0 == GetLastError())
	{
		std::string path(executionPath);
		path.append(DLL_LIBRARY_NAME);
		path.append(".new");
		strcpy_s(dllNewPath, path.c_str());
		MoveFileEx(
			dllNewPath,						// lpExistingFileName
			dllFullPath,					// lpNewFileName
			MOVEFILE_REPLACE_EXISTING		// dwFlags
		);

		if (0 != GetLastError())
		{
			MoveFileEx(
				dllBackupPath,				// lpExistingFileName
				dllFullPath,				// lpNewFileName
				MOVEFILE_REPLACE_EXISTING	// dwFlags
			);
		}
	}
}