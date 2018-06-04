#include <Windows.h> 
#include <iostream>

typedef void(*Func)();


typedef void(*Callback)(Func);
typedef void(*CharArrayFunc)(char*);

#define DLL_LIBRARY_NAME "TcpServer.dll"

HINSTANCE hInst;
CharArrayFunc GetExecutablePath;
char executionPath[MAX_PATH];

int main(int argc, char **argv, char **envp);
void ReplaceDll();
int LoadingError(const char* functionName);