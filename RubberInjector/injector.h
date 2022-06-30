#pragma once

#include <string>
#include <iostream>
#include <windows.h>

bool InjectDLL(const int& pid, const std::string& DLL_Path);