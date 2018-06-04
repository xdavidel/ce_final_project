#pragma once
#include <Windows.h>
#include <iostream>
#include <unordered_map>

typedef const int(*IntFunc)();
std::unordered_map<std::string, IntFunc> fList;

bool expectUpdate;
size_t downloadSize;

int OnAccept(char* buffer);

const int RequestUpdate(char* buffer);
const int Update(char* buffer);
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