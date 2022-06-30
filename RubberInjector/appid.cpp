#include "appid.h"
#include <string>


std::wstring PackageFullNameFromFamilyName(std::wstring familyName)
{
	std::wstring fullName;
	UINT32 count = 0;
	UINT32 length = 0;

	// First call gets the count and length; PACKAGE_FILTER_HEAD tells Windows to query Application Packages
	LONG status = FindPackagesByPackageFamily(familyName.c_str(), PACKAGE_FILTER_HEAD, &count, nullptr, &length, nullptr, nullptr);
	if (status == ERROR_SUCCESS || status != ERROR_INSUFFICIENT_BUFFER)
		return fullName;

	PWSTR* fullNames = (PWSTR*)malloc(count * sizeof(*fullNames));
	PWSTR buffer = (PWSTR)malloc(length * sizeof(WCHAR));
	UINT32* properties = (UINT32*)malloc(count * sizeof(*properties));
	if (fullNames == nullptr)
		goto Cleanup;

	if (buffer == nullptr)
		goto Cleanup;

	if (properties == nullptr)
		goto Cleanup;

	// Second call gets all fullNames
	// buffer and properties are needed even though they're never used
	status = FindPackagesByPackageFamily(familyName.c_str(), PACKAGE_FILTER_HEAD, &count, fullNames, &length, buffer, properties);
	if (status != ERROR_SUCCESS)
		goto Cleanup;
	else
		fullName = std::wstring(fullNames[0]); // Get the first activatable package found; usually there is only one anyway

Cleanup:
	if (properties != nullptr)
		free(properties);
	if (buffer != nullptr)
		free(buffer);
	if (fullNames != nullptr)
		free(fullNames);

	return fullName;
}


std::wstring AppIdFromFullName(std::wstring fullName)
{
	std::wstring appId;
	PACKAGE_INFO_REFERENCE pir = { 0 };

	// Get PackageInfo using the supplied PackageFullName
	LONG rc = OpenPackageInfoByFullName(fullName.c_str(), 0, &pir);
	if (rc != ERROR_SUCCESS)
		return appId;

	UINT32 count;
	UINT32 length = 0;

	// First call gets the length and count
	rc = GetPackageApplicationIds(pir, &length, NULL, &count);
	if (rc != ERROR_INSUFFICIENT_BUFFER)
	{
		ClosePackageInfo(pir);
		return appId;
	}

	// Allocate buffer space to store the ApplicationId's
	BYTE* buffer = (BYTE*)malloc(length);
	if (buffer == NULL)
	{
		ClosePackageInfo(pir);
		return appId;
	}

	// Second Call gets the ApplicationId's and stores them in the buffer
	rc = GetPackageApplicationIds(pir, &length, buffer, &count);
	if (rc != ERROR_SUCCESS)
	{
		ClosePackageInfo(pir);
		return appId;
	}

	PCWSTR* applicationUserModelIds = (PCWSTR*)buffer;

	// Set the appId as the first ID found; normally apps only have one AppId
	appId = applicationUserModelIds[0];

	free(buffer);

	// Close the package info opened earlier
	rc = ClosePackageInfo(pir);
	if (rc != ERROR_SUCCESS)
		return appId;

	return appId;
}


HRESULT LaunchApplication(LPCWSTR packageFullName, PDWORD pdwProcessId)
{
	CComPtr<IApplicationActivationManager> spAppActivationManager;
	HRESULT result = E_INVALIDARG;

	// Initialize IApplicationActivationManager
	result = CoCreateInstance(CLSID_ApplicationActivationManager,
		NULL,
		CLSCTX_LOCAL_SERVER,
		IID_IApplicationActivationManager,
		(LPVOID*)&spAppActivationManager);

	if (!SUCCEEDED(result)) return result;

	// This call ensures that the app is launched as the foreground window
	result = CoAllowSetForegroundWindow(spAppActivationManager, NULL);


	if (!SUCCEEDED(result)) return result;

	// Launch the app
	result = spAppActivationManager->ActivateApplication(packageFullName, NULL, AO_NONE, pdwProcessId);

	return result;
}

HRESULT LaunchApp(LPCWSTR AUMID, PDWORD pid)
{
	HRESULT hr = CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);
	if (FAILED(hr))
	{
		wprintf(L"LaunchApp %s: Failed to init COM. hr = 0x%08lx \n", AUMID, hr);
	}
	{
		CComPtr<IApplicationActivationManager> AppActivationMgr = nullptr;
		if (SUCCEEDED(hr))
		{
			hr = CoCreateInstance(CLSID_ApplicationActivationManager, nullptr,
				CLSCTX_LOCAL_SERVER, IID_PPV_ARGS(&AppActivationMgr));
			if (FAILED(hr))
			{
				wprintf(L"LaunchApp %s: Failed to create Application Activation  Manager.hr = 0x % 08lx \n", AUMID, hr);
			}
		}
		if (SUCCEEDED(hr))
		{
			hr = AppActivationMgr->ActivateApplication(AUMID, nullptr, AO_NONE, pid);
			if (FAILED(hr))
			{
				wprintf(L"LaunchApp %s: Failed to Activate App. hr = 0x%08lx  \n", AUMID, hr);
			}

			wprintf(L"Created process with pid: ");
		}
	}
	CoUninitialize();
	return hr;
}

DWORD SetPermissions(std::wstring wstrFilePath)
{
	PACL pOldDACL = NULL, pNewDACL = NULL;
	PSECURITY_DESCRIPTOR pSD = NULL;
	EXPLICIT_ACCESS eaAccess;
	SECURITY_INFORMATION siInfo = DACL_SECURITY_INFORMATION;
	DWORD dwResult = ERROR_SUCCESS;
	PSID pSID;

	// Get a pointer to the existing DACL
	dwResult = GetNamedSecurityInfo(wstrFilePath.c_str(), SE_FILE_OBJECT, DACL_SECURITY_INFORMATION, NULL, NULL, &pOldDACL, NULL, &pSD);
	if (dwResult != ERROR_SUCCESS)
		goto Cleanup;

	// Get the SID for ALL APPLICATION PACKAGES using its SID string
	ConvertStringSidToSid(L"S-1-15-2-1", &pSID);
	if (pSID == NULL)
		goto Cleanup;

	ZeroMemory(&eaAccess, sizeof(EXPLICIT_ACCESS));
	eaAccess.grfAccessPermissions = GENERIC_READ | GENERIC_EXECUTE;
	eaAccess.grfAccessMode = SET_ACCESS;
	eaAccess.grfInheritance = SUB_CONTAINERS_AND_OBJECTS_INHERIT;
	eaAccess.Trustee.TrusteeForm = TRUSTEE_IS_SID;
	eaAccess.Trustee.TrusteeType = TRUSTEE_IS_WELL_KNOWN_GROUP;
	eaAccess.Trustee.ptstrName = (LPWSTR)pSID;

	// Create a new ACL that merges the new ACE into the existing DACL
	dwResult = SetEntriesInAcl(1, &eaAccess, pOldDACL, &pNewDACL);
	if (ERROR_SUCCESS != dwResult)
		goto Cleanup;

	// Attach the new ACL as the object's DACL
	dwResult = SetNamedSecurityInfo((LPWSTR)wstrFilePath.c_str(), SE_FILE_OBJECT, siInfo, NULL, NULL, pNewDACL, NULL);
	if (ERROR_SUCCESS != dwResult)
		goto Cleanup;

Cleanup:
	if (pSD != NULL)
		LocalFree((HLOCAL)pSD);
	if (pNewDACL != NULL)
		LocalFree((HLOCAL)pNewDACL);

	return dwResult;
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