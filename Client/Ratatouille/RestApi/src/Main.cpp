#include "Main.h"

BOOL WINAPI DllMain(
	HINSTANCE hinstDLL, // handle to DLL module
	DWORD fdwReason, // reason for calling function
	LPVOID lpReserved) // reserved
{

	// Perform actions based on the reason for calling.
	switch (fdwReason)
	{

	case DLL_PROCESS_ATTACH:
		printf("Initialize once for each new process...\n");
		// Initialize once for each new process.
		// Return FALSE to fail DLL load.
		break;

	case DLL_THREAD_ATTACH:
		printf("Do thread-specific initialization...\n");
		// Do thread-specific initialization.
		break;

	case DLL_THREAD_DETACH:
		printf("Do thread-specific cleanup...\n");

		// Do thread-specific cleanup.

		break;


	case DLL_PROCESS_DETACH:
		printf("Perform any necessary cleanup...\n");

		// Perform any necessary cleanup.

		break;
	}

	// Successful DLL_PROCESS_ATTACH.
	return TRUE;

}

const wchar_t* GetRequest()
{
	web::http::client::http_client client(U("http://localhost:6363"));
	web::http::http_request request(web::http::methods::GET);
	web::http::http_response response;

	try 
	{
		client.request(request).then([&](web::http::http_response res) {
			response = res;

		}).wait();

		if (web::http::status_codes::OK == response.status_code())
		{
			auto content = response.extract_string().get();
			return content.c_str();
			
		}
		else
		{
			return L"Fail";
		}
	}
	catch (...) 
	{
		return L"Could not reach server :(";
	}

}