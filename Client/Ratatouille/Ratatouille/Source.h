#include <Windows.h> 
#include <iostream>

typedef void(*Func)();


typedef void(*Callback)(Func);

#define DLL_LIBRARY_NAME "TcpServer.dll"

HINSTANCE hInst;

int main(int argc, char **argv, char **envp);
void Update();
void ReplaceDll();