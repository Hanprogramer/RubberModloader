#pragma once
#include "RubberModLoader.h"

// Hook type for CreateFileW
typedef HANDLE(WINAPI* FuncPtrCreateFileW)(LPCWSTR lpFilename, DWORD dwAccess, DWORD dwSharing, LPSECURITY_ATTRIBUTES saAttributes, DWORD dwCreation, DWORD dwAttributes, HANDLE hTemplate);

// Original Function Pointer CreateFileW
extern FuncPtrCreateFileW RML_CreateFileW;
namespace RML {
	// CreateFileW hook function
	HANDLE WINAPI NewCreateFileW(LPCWSTR lpFilename, DWORD dwAccess, DWORD dwSharing, LPSECURITY_ATTRIBUTES saAttributes, DWORD dwCreation, DWORD dwAttributes, HANDLE hTemplate);
}