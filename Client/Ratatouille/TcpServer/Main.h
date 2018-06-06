#pragma once
#include <Windows.h>
#include <iostream>


int OnAccept(char* buffer);

const int Update(int length, char* value);
const int Echo(char* buffer);

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