#pragma once
#include "pch.h"
#include "MinHook.h"
#include <string>


// Sets a hook on the function at origAddress function and provides a trampoline to the original function
BOOL setHook(LPVOID* origAddress, LPVOID* hookFunction, LPVOID* trampFunction);

// Attaches a hook on a function given the name of the owning module and the name of the function
BOOL attach(LPCWSTR wstrModule, LPCSTR strFunction, LPVOID* hook, LPVOID* original);

// Get Process name from a PID (in this case a full path)
std::string ProcessIdToName(DWORD processId);