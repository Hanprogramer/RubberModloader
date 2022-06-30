// RubberInjector.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include "appid.h"
#include <string>
#include "injector.h"
const std::wstring MINECRAFT_PREVIEW = L"Microsoft.MinecraftWindowsBeta_8wekyb3d8bbwe";
const std::string RML_DLL = "RubberModloader.dll";

// TODO: Auto use current executable location
const std::string RML_DLL_PATH = "D:\\programing\\han\\C++\\RubberModloader\\x64\\Debug\\RubberModloader.dll"; 
const std::wstring RML_DLL_PATH_W = L"D:\\programing\\han\\C++\\RubberModloader\\x64\\Debug\\RubberModloader.dll"; 

int main()
{
	// Get Minecraft UWP's information
	std::wstring packageId = PackageFullNameFromFamilyName(MINECRAFT_PREVIEW);
	std::wstring appId = AppIdFromFullName(packageId);
	// TODO::Support non beta
	std::wcout << "Package ID:" << packageId << std::endl << "App ID: " << appId << std::endl;

	// Set permission of the dll
	SetPermissions(RML_DLL_PATH_W);

	// Start the activity
	DWORD dwPid = 0;
	LaunchApp(appId.c_str(), &dwPid);
	std::cout << "PID:" << std::hex << dwPid << std::endl;
	std::cout << "P name:" << ProcessIdToName(dwPid) << std::endl;


	// Inject the DLL
	std::cout << "Injecting..." << std::endl;
	if (InjectDLL(dwPid, RML_DLL_PATH)) {
		std::cout << "Injecting DONE" << std::endl;
	}
	else {
		std::cout << "Injecting ERROR" << std::endl;
		return 1;
	}
	return 0;
}