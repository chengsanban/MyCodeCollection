#include"IComTest.h"


#include <iostream>  
#include <windows.h>  

#include "factory.h"  
#include "ComTestClass.h"  

using namespace std;

HMODULE g_hModule;    
ULONG g_num;  

/***************************reg key print********************************/
typedef LONG NTSTATUS;

#ifndef STATUS_SUCCESS
#define STATUS_SUCCESS ((NTSTATUS)0x00000000L)
#endif

#ifndef STATUS_BUFFER_TOO_SMALL
#define STATUS_BUFFER_TOO_SMALL ((NTSTATUS)0xC0000023L)
#endif

std::wstring GetKeyPathFromKKEY(HKEY key)
{
	std::wstring keyPath;
	if (key != NULL)
	{
		HMODULE dll = LoadLibrary(L"ntdll.dll");
		if (dll != NULL) {
			typedef DWORD(__stdcall *NtQueryKeyType)(
				HANDLE  KeyHandle,
				int KeyInformationClass,
				PVOID  KeyInformation,
				ULONG  Length,
				PULONG  ResultLength);

			NtQueryKeyType func = reinterpret_cast<NtQueryKeyType>(::GetProcAddress(dll, "NtQueryKey"));

			if (func != NULL) {
				DWORD size = 0;
				DWORD result = 0;
				result = func(key, 3, 0, 0, &size);
				if (result == STATUS_BUFFER_TOO_SMALL)
				{
					size = size + 2;
					wchar_t* buffer = new (std::nothrow) wchar_t[size / sizeof(wchar_t)]; // size is in bytes
					if (buffer != NULL)
					{
						result = func(key, 3, buffer, size, &size);
						if (result == STATUS_SUCCESS)
						{
							buffer[size / sizeof(wchar_t)] = L'\0';
							keyPath = std::wstring(buffer + 2);
						}

						delete[] buffer;
					}
				}
			}

			FreeLibrary(dll);
		}
	}
	return keyPath;
}

/***************************reg key print********************************/


int myReg(LPCWSTR lpPath)    
{
	HKEY clsidKey, subClsidKey;

	if (ERROR_SUCCESS == RegOpenKey(HKEY_CLASSES_ROOT, L"CLSID", &clsidKey))
	{
		//wchar_t string[256];
		//wsprintf(string, GetKeyPathFromKKEY(clsidKey).c_str());
		//OutputDebugStringW(string);

		if (ERROR_SUCCESS == RegCreateKey(clsidKey, L"{9CA9DBE8-C0B1-42c9-B6C7-856BE5756855}", &subClsidKey))
		{
			HKEY tinps32k;
			if (ERROR_SUCCESS == RegCreateKey(subClsidKey, L"InprocServer32", &tinps32k))
			{
				if (ERROR_SUCCESS == RegSetValue(tinps32k, NULL, REG_SZ, lpPath, wcslen(lpPath) * 2))
				{
				}
				RegCloseKey(tinps32k);
			}
			RegCloseKey(subClsidKey);
		}
		RegCloseKey(clsidKey);
	}

	if (ERROR_SUCCESS == RegCreateKey(HKEY_CLASSES_ROOT, L"COMCTL.ComTest", &clsidKey))
	{
		if (ERROR_SUCCESS == RegCreateKey(clsidKey, L"CLSID", &subClsidKey))
		{
			if (ERROR_SUCCESS == RegSetValue(subClsidKey,
				NULL,
				REG_SZ,
				L"{9CA9DBE8-C0B1-42c9-B6C7-856BE5756855}",
				wcslen(L"{9CA9DBE8-C0B1-42c9-B6C7-856BE5756855}") * 2))
			{
			}
			RegCloseKey(subClsidKey);
		}
		RegCloseKey(clsidKey);
	}
 
	return 0;
}

//register dll (must)
extern "C" HRESULT CALLBACK DllRegisterServer()
{
	WCHAR szModule[1024];
	DWORD dwResult = GetModuleFileName(g_hModule, szModule, 1024); 
	if (0 == dwResult){
		return -1;
	}
	MessageBox(NULL, szModule, L"", MB_OK);
	myReg(szModule); 
	return 0;
}

int myDelKey(HKEY hk, LPCWSTR lp)
{
	if (ERROR_SUCCESS == RegDeleteKey(hk, lp)){
	}
	return 0;
}

int myDel() 
{
	HKEY thk;
	if (ERROR_SUCCESS == RegOpenKey(HKEY_CLASSES_ROOT, L"CLSID", &thk)){
		myDelKey(thk, L"{9CA9DBE8-C0B1-42c9-B6C7-856BE5756855}\\InprocServer32");
		myDelKey(thk, L"{9CA9DBE8-C0B1-42c9-B6C7-856BE5756855}");

		RegCloseKey(thk);
	}
	if (ERROR_SUCCESS == RegOpenKey(HKEY_CLASSES_ROOT, L"COMCTL.ComTest", &thk)){
		myDelKey(thk, L"CLSID");
	}
	myDelKey(HKEY_CLASSES_ROOT, L"COMCTL.ComTest");
	return 0;
}

//UnRegister dll (must)
extern "C" HRESULT CALLBACK DllUnregisterServer()
{
	myDel();

	return 0;
}

//CoFreeUnusedLibraries call (must)
extern "C" HRESULT CALLBACK DllCanUnloadNow()
{
	if (0 == g_num){
		return S_OK;
	}
	else{
		return S_FALSE;
	}
}

//CoCreateInstance call (must)
extern "C" HRESULT CALLBACK DllGetClassObject(__in REFCLSID rclsid, __in REFIID riid, LPVOID FAR* ppv)
{
	if (CLSID_CompTestClass == rclsid){
		CompTestFactory* pFactory = new CompTestFactory();
		if (NULL == pFactory){
			return E_OUTOFMEMORY;
		}
		HRESULT result = pFactory->QueryInterface(riid, ppv);
		return result;
	}
	else{
		return CLASS_E_CLASSNOTAVAILABLE;
	}
}


BOOL APIENTRY DllMain(HMODULE hModule,
	DWORD  ul_reason_for_call,
	LPVOID lpReserved
	)
{
	g_hModule = hModule;  
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH:
		break;
	}
	return TRUE;
}
