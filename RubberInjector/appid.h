#pragma once

#include <iostream>
#include <basetsd.h>
#include <Windows.h>
#include <appmodel.h>
#include <shobjidl_core.h>
#include <atlbase.h>
#include <AclAPI.h>
#include <sddl.h>

std::wstring PackageFullNameFromFamilyName(std::wstring familyName);
std::wstring AppIdFromFullName(std::wstring fullName);

HRESULT LaunchApplication(LPCWSTR packageFullName, PDWORD pdwProcessId);
HRESULT LaunchApp(LPCWSTR AUMID, PDWORD pid);

DWORD SetPermissions(std::wstring wstrFilePath);
std::string ProcessIdToName(DWORD processId);