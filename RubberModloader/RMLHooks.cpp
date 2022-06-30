#include "pch.h"
#include "RMLHooks.h"

// Original Function Pointer CreateFileW
FuncPtrCreateFileW RML_CreateFileW = NULL;

// CreateFileW hook function
HANDLE WINAPI RML::NewCreateFileW(LPCWSTR lpFilename, DWORD dwAccess, DWORD dwSharing, LPSECURITY_ATTRIBUTES saAttributes, DWORD dwCreation, DWORD dwAttributes, HANDLE hTemplate)
{
	//std::wstring filePath(lpFilename);
	/*DWORD pid = GetCurrentProcessId();
	std::string name = ProcessIdToName(pid);
	name.resize(name.size()-21);
	replaceAll(name, "\\", "/");
	std::string filePathStr(filePath.begin(), filePath.end());
	replace(filePathStr, name, "");
	if (a == 0) {
		std::cout << name << std::endl;
		a++;
	}
	if (a < 50 && hasEnding(filePathStr, "loading_spin.png")) {
		std::cout << filePathStr << std::endl;
		a++;
	}*/

	// Check if the app is accessing protected resources
	//if (filePath.find(APP_LOCATION) != filePath.npos)
	//{
	//	std::wstring newPath(MOD_FILES_PATH);

	//	// Windows provides the app the location of the WindowsApps directory, so the first half the file path will use back slashes
	//	// After that, some apps will use back slashes while others use forward slashes so be aware of what the app uses
	//	newPath += filePath.substr(filePath.find(L"\\", APP_LOCATION.size()) + 1, filePath.size());

	//	// Check if the file being accessed exists at the new path and reroute access to that file
	//	// Don't reroute directories as bad things can happen such as directories being ghost locked
	//	if (PathFileExists(newPath.c_str()) && !PathIsDirectory(newPath.c_str()))
	//		return _CreateFileW(newPath.c_str(), dwAccess, dwSharing, saAttributes, dwCreation, dwAttributes, hTemplate);
	//}
	// Let the app load other files normally
	return RML_CreateFileW(lpFilename, dwAccess, dwSharing, saAttributes, dwCreation, dwAttributes, hTemplate);
}