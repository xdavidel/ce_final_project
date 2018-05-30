#pragma once
#include <Windows.h>
#include <iostream>

#include <cpprest\http_listener.h>
#include <cpprest\json.h>
#include <cpprest\uri.h>
#include <cpprest\asyncrt_utils.h>
#include <cpprest\http_client.h>


#ifndef INDLL_H
#define INDLL_H

#ifdef EXPORTING_DLL
extern "C" __declspec(dllexport) const wchar_t* HttpGet(utility::string_t url);
extern "C" __declspec(dllexport) const wchar_t* HttpPost(utility::string_t url, web::json::value data);

#else
extern "C" __declspec(dllexport) const wchar_t* HttpGet(utility::string_t url);
extern "C" __declspec(dllexport) const wchar_t* HttpPost(utility::string_t url, web::json::value data);

#endif


#endif