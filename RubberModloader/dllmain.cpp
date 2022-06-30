//// dllmain.cpp : Defines the entry point for the DLL application.
#include "pch.h"
#include "dllmain.h"
// Path to modified game files store in AppData
std::wstring MOD_FILES_PATH(L"");

// Path to the apps protected resources in WindowsApps
// Don't use the full path name, just keep the Publisher.AppName part
std::wstring APP_LOCATION(L"C:\\Program Files\\WindowsApps\\Publisher.AppName");


BOOL Initialize(HMODULE hModule)
{
	AllocConsole();
	FILE* f;
	freopen_s(&f, "CONOUT$", "w", stdout);
	ShowWindow(GetConsoleWindow(), SW_SHOW);
	std::cout << "[RML] Getting appData folder...\n";

	TCHAR szPath[MAX_PATH];
	if (SUCCEEDED(SHGetFolderPath(NULL, CSIDL_LOCAL_APPDATA, NULL, 0, szPath)))
	{
		// Get the path to the mod files folder
		std::wstring appData(szPath);
		appData = appData.substr(0, appData.rfind(L"AC")); // Get the base directory
		appData += L"LocalState\\ModFiles\\"; // Get the location of any new files you want the app to use

		MOD_FILES_PATH = appData;
	}
	else
		return FALSE;
	

	std::cout << "[RML] Initializing RML Engine...\n";
	// Initialize RML engine
	if (!RML::Initialize(hModule)) {
		return FALSE;
	}
	std::cout << "[RML] Initialization done\n";
	return TRUE;
}
BOOL APIENTRY DllMain(HMODULE hModule, DWORD  ul_reason_for_call, LPVOID lpReserved)
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
		return Initialize(hModule); // If initialization failed, the DLL will detach
		break;
	case DLL_THREAD_ATTACH:
		break;
	case DLL_THREAD_DETACH:
		break;
	case DLL_PROCESS_DETACH:
		RML::Dispose(); // Return value doesn't matter when detaching
		break;
	}
	return TRUE;
}
