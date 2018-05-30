#include "Source.h"

int main(int argc, char **argv, char **envp)
{
	DWORD error;
	Callback fn;
	Func startTcp;

	while (true)
	{
		hInst = LoadLibrary(DLL_LIBRARY_NAME);
		error = GetLastError();

		if (error)
		{
			std::cout << "Could not locate the DLL" << std::endl;
			return EXIT_FAILURE;
		}

		if (nullptr == hInst) return 1;

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

void ReplaceDll()
{
	CreateFile("1.bak", 0, 0, 0, 0, 0, 0);
	MoveFileA("D:\\TcpServer.dll", "D:\\bak.bak");
	auto res = GetLastError();
	MoveFileA(".\\tmp.tmp", ".\\TcpServer.dll");
}