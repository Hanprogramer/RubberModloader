#include "pch.h"
#include "RubberModLoader.h"


namespace RML {
	DWORD WINAPI RubberModLoaderThread(HMODULE hModule)
	{
		AllocConsole();
		FILE* f;
		freopen_s(&f, "CONOUT$", "w", stdout);
		ShowWindow(GetConsoleWindow(), SW_SHOW);
		std::cout << "[RML] Rubber Modloader thread starting..\n";

		uintptr_t moduleBase = (uintptr_t)GetModuleHandle(L"Minecraft.Windows.exe");
		bool bPos = true;
		while (true) {
			if (GetAsyncKeyState(VK_DELETE) & 1) {
				std::cout << "Delete has pressed\n";
				break;
			}

			if (GetAsyncKeyState(VK_RETURN) & 1) {
				std::cout << "Return has pressed\n";
				bPos = !bPos;
			}
			if (bPos) {
				uintptr_t localPlayerAddr = FindDMAAddy(moduleBase + 0x048663C8, { 0x0, 0x20, 0xC8, 0x4B8 });
				std::cout << "Player: " << "0x" << std::hex << localPlayerAddr << std::endl;
				float xpos = *(float*)localPlayerAddr;
				float ypos = *(float*)(localPlayerAddr + 0x0004);
				std::cout << "x: " << std::to_string(xpos) << "\t"
					<< "y: " << std::to_string(ypos) << std::endl;
				bPos = false;
			}

			Sleep(5);
		}
		fclose(f);
		FreeConsole();
		FreeLibraryAndExitThread(hModule, 0);
		return 0;
	}

	void StartRMLThreadWin32(HMODULE hModule)
	{
		CloseHandle(
			CreateThread(nullptr, 0, (LPTHREAD_START_ROUTINE)RubberModLoaderThread, hModule, 0, nullptr)
		);
	}

	bool Initialize(HMODULE hModule)
	{
		std::cout << "[RML] Initializing MinHook...\n";
		// Initialize MinHook
		if (MH_Initialize() != MH_OK)
			return FALSE;

		std::cout << "[RML] Initializing file hooks...\n";


		//// Attach a hook on CreateProcessW and return the status of the hook
		BOOL hook = TRUE;
		hook &= attach(L"KernelBase.dll", "CreateFileW", (LPVOID*)&NewCreateFileW, (LPVOID*)&RML_CreateFileW);
		if (hook == FALSE) {
			std::cout << "[RML Error] Failed to create file hook\n";
			return false;
		}

		std::cout << "[RML] Done\n";
		std::cout << "[RML] Starting mod thread...\n";
		StartRMLThreadWin32(hModule);
		return true;
	}

	bool Dispose()
	{
		// Uninitialize MinHook
		if (MH_Uninitialize() != MH_OK)
			return FALSE; // This status will end up being ignored

		return TRUE;
	}

}