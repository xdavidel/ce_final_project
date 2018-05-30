#include "Source.h"

int main(int argc, char **argv, char **envp)
{
	DWORD error;
	Callback fn;
	Func startTcp;

	while (true)
	{
		hInst = LoadLibrary(DLL_LIBRARY_NAME);

		if (!hInst)
		{
			std::cout << "Could not locate the DLL" << std::endl;
			return EXIT_FAILURE;
		}

		// Starting the tcp socket connection
		startTcp = (Func)GetProcAddress(hInst, "StartConnection");
		if (!startTcp)
		{
			std::cout << "Could not locate the function" << std::endl;
			return EXIT_FAILURE;
		}

		startTcp();

		FreeLibrary(hInst);
		ReplaceDll();
	}

	return 0;
}

std::string ExePath() {
	char buffer[MAX_PATH];
	GetModuleFileName(NULL, buffer, MAX_PATH);
	std::string::size_type pos = std::string(buffer).find_last_of("\\/");
	return std::string(buffer).substr(0, pos + 1);
}

void ReplaceDll()
{
	char dllFullPath[MAX_PATH];
	char dllBackupPath[MAX_PATH];
	char dllNewPath[MAX_PATH];

	std::string path = ExePath();
	path.append(DLL_LIBRARY_NAME);

	
	strcpy_s(dllFullPath, path.c_str());

	path.append(".bak");
	strcpy_s(dllBackupPath, path.c_str());
	MoveFileA(dllFullPath, dllBackupPath);
	if (0 == GetLastError())
	{
		path = ExePath();
		path.append(DLL_LIBRARY_NAME);
		path.append(".new");
		strcpy_s(dllNewPath, path.c_str());

		if (0 != MoveFileA(dllNewPath, dllFullPath))
		{
			MoveFileA(dllBackupPath, dllFullPath);
		}
	}
}