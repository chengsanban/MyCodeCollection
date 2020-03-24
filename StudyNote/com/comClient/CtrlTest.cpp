#include<iostream>
#include"IComTest.h"

using namespace std;

int main()
{
	CoInitialize(NULL);
	IUnknown* pUnknown = NULL;
	GUID CLSID_ComTestClass;
	HRESULT hResult = CLSIDFromProgID(L"COMCTL.ComTest", &CLSID_ComTestClass);

	if (S_OK != hResult)
	{
		printf("Can't find CLSID!\n");
		return -1;
	}
	else
	{
		LPOLESTR szCLSID;
		StringFromCLSID(CLSID_ComTestClass, &szCLSID);
		wprintf(L"find CLSID \"%s\"\n", szCLSID);
		CoTaskMemFree(szCLSID);  
	}

	hResult = CoCreateInstance(CLSID_ComTestClass, NULL, CLSCTX_INPROC_SERVER, IID_IUnknown, (void **)&pUnknown);

	if (S_OK != hResult || NULL == pUnknown){
		printf("Create Object Failed!\n");
		return -1;
	}

	IComTest* pComTest = NULL;
	hResult = pUnknown->QueryInterface(IID_IComTest, (void**)&pComTest);

	cout << pComTest->HelloWorld() << endl; 
	pComTest->Release();
	pUnknown->Release();
	CoUninitialize(); 
	return 0;
}