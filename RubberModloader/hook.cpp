#include "pch.h"
#include "hook.h"

/*
Some important notes:

1. WHY IS MY ALLOCCONSOLE WINDOW HIDDEN?????
Answer by Fligger: https://www.unknowncheats.me/forum/general-programming-and-reversing/461078-allocconsole-winapi-windows-uwp-apps.html
Make sure you have read @VoxelBlock's Basic and Intermediate Techniques of UWP App Modding post before trying this.

A while ago I found out how to freely modify UWP apps gamefiles and get "AllocConsole()" and winapi windows to show up using dll injection.

What you need to do is relocate your gamefiles to a place of choice and the reregister it in "DevelopementMode" using a powershell command!

To do that open your WindowsApps folder and copy out your game / app to a place of choice.
If you can't access the folder make sure claim ownership of WindowsApps.

Next open that copied folder and delete the "AppxSignature.p7x" file. (this would stop you from reinstalling it)

Now uninstall the original game, open powershell and type "Add-AppxPackage -Register -Path " then drag and drop the "AppxManifest.xml" file into powershell and press enter.
Make sure you have enabled Deverloper mode in windows settings to allow unsigned UWP apps to get installed.

Now you allocConsole Console should work just fine and you will be enable to modify files freely.

more info about Add-AppxPackage


*/
BOOL setHook(LPVOID* origAddress, LPVOID* hookFunction, LPVOID* trampFunction)
{
	if (MH_CreateHook(origAddress, hookFunction, reinterpret_cast<LPVOID*>(trampFunction)) != MH_OK)
		return FALSE;

	if (MH_EnableHook(origAddress) != MH_OK)
		return FALSE;

	return TRUE;
}

BOOL attach(LPCWSTR wstrModule, LPCSTR strFunction, LPVOID* hook, LPVOID* original)
{
	HMODULE hModule = GetModuleHandle(wstrModule);
	if (hModule == NULL) {
		std::wcout << L"Error attach:" << wstrModule << L":" << strFunction << L" Cause: Error getting module handle" << std::endl;
		return FALSE;
	}

	FARPROC hFunction = GetProcAddress(hModule, strFunction);
	if (hFunction == NULL) {
		std::wcout << L"Error attach:" << wstrModule << L":" << strFunction << L" Cause: Error getting process address" << std::endl;
		return FALSE;
	}

	return setHook((LPVOID*)hFunction, hook, original);
}

std::string ProcessIdToName(DWORD processId)
{
	std::string ret;
	HANDLE handle = OpenProcess(
		PROCESS_QUERY_LIMITED_INFORMATION,
		FALSE,
		processId);
	if (handle)
	{
		DWORD buffSize = 1024;
		CHAR buffer[1024];
		if (QueryFullProcessImageNameA(handle, 0, buffer, &buffSize))
		{
			ret = buffer;
		}
		else
		{
			printf("Error GetModuleBaseNameA : %lu", GetLastError());
		}
		CloseHandle(handle);
	}
	else
	{
		printf("Error OpenProcess : %lu", GetLastError());
	}
	return ret;
}