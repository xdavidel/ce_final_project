#pragma once
#include <Windows.h>
#include <iostream>
#include <unordered_map>


typedef const int(*IntFunc)();

int OnAccept(char* buffer);
const int Update();
const int Echo(char* buffer);

#ifndef INDLL_H
#define INDLL_H

#ifdef EXPORTING_DLL
extern "C" __declspec(dllexport) void StartConnection();

#else
extern "C" __declspec(dllexport) void StartConnection();

#endif


#endif