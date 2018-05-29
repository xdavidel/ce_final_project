#include <Windows.h> 
#include <iostream>

typedef wchar_t*(*Func)();

int wmain(int argc, wchar_t *argv[], wchar_t *envp[])
{
	DWORD error;

	HINSTANCE hInst = LoadLibrary(L"RestApi.dll");
	error = GetLastError();

	if (error)
	{
		std::wcout << L"Could not locate the DLL" << std::endl;
		return EXIT_FAILURE;
	}

	if (nullptr == hInst) return 1;

	Func fn = (Func)GetProcAddress(hInst, "GetRequest");

	if (!fn) 
	{ 
		std::wcout << L"Could not locate the function" << std::endl;
		return EXIT_FAILURE; 
	}

	auto result = fn();

	return 0;
}