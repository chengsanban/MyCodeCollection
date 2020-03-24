#ifndef COMTESTCLASS_H  
#define COMTESTCLASS_H  


#include "IComTest.h"  

// {9CA9DBE8-C0B1-42c9-B6C7-856BE5756855}  
static const GUID CLSID_CompTestClass =
{ 0x9ca9dbe8, 0xc0b1, 0x42c9, { 0xb6, 0xc7, 0x85, 0x6b, 0xe5, 0x75, 0x68, 0x55 } };

class ComTestClass : public IComTest
{
public:

	ComTestClass();
	~ComTestClass();

	virtual HRESULT _stdcall QueryInterface(const IID& riid, void** ppvObject);
	virtual ULONG _stdcall AddRef();
	virtual ULONG _stdcall Release();

	virtual int _stdcall HelloWorld();
protected:
	ULONG m_Ref;
	static ULONG m_objNum;        //object number
	static CRITICAL_SECTION m_cs; //multi thread lock
};
#endif  