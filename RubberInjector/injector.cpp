#include "injector.h"
//-----------------------------------------------------------
// Inject DLL to target process
//-----------------------------------------------------------
bool InjectDLL(const int& pid, const std::string& DLL_Path)
{
	long dll_size = DLL_Path.length() + 1;
	HANDLE hProc = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pid);

	if (hProc == NULL)
	{
		std::cerr << "[!]Fail to open target process!" << std::endl;
		return false;
	}
	std::cout << "[+]Opening Target Process..." << std::endl;

	LPVOID MyAlloc = VirtualAllocEx(hProc, NULL, dll_size, MEM_COMMIT, PAGE_EXECUTE_READWRITE);
	if (MyAlloc == NULL)
	{
		std::cerr << "[!]Fail to allocate memory in Target Process." << std::endl;
		return false;
	}

	std::cout << "[+]Allocating memory in Target Process." << std::endl;
	int IsWriteOK = WriteProcessMemory(hProc, MyAlloc, DLL_Path.c_str(), dll_size, 0);
	if (IsWriteOK == 0)
	{
		std::cerr << "[!]Fail to write in Target Process memory." << std::endl;
		return false;
	}
	std::cout << "[+]Creating Remote Thread in Target Process" << std::endl;

	DWORD dWord;
	LPTHREAD_START_ROUTINE addrLoadLibrary = (LPTHREAD_START_ROUTINE)GetProcAddress(LoadLibrary(L"kernel32"), "LoadLibraryA");
	HANDLE ThreadReturn = CreateRemoteThread(hProc, NULL, 0, addrLoadLibrary, MyAlloc, 0, &dWord);
	if (ThreadReturn == NULL)
	{
		std::cerr << "[!]Fail to create Remote Thread" << std::endl;
		return false;
	}

	if ((hProc != NULL) && (MyAlloc != NULL) && (IsWriteOK != ERROR_INVALID_HANDLE) && (ThreadReturn != NULL))
	{
		std::cout << "[+]DLL Successfully Injected :)" << std::endl;
		return true;
	}

	return false;
}