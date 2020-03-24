#include "ComTestClass.h"  
#include <windows.h>

ULONG ComTestClass::m_objNum = 0;
CRITICAL_SECTION ComTestClass::m_cs;

ComTestClass::ComTestClass()
{
	m_Ref = 0;
	//autoLock tlock(m_cs);
	//m_objNum++;  
}

ComTestClass::~ComTestClass()
{
/*	autoLock tlock(m_cs);
	m_objNum--;   */  
}

HRESULT _stdcall ComTestClass::QueryInterface(const IID &riid, void **ppvObject)
{
	if (IID_IUnknown == riid){
		*ppvObject = (IUnknown*)this;
		((IUnknown*)(*ppvObject))->AddRef();
	}
	else if (IID_IComTest == riid){
		*ppvObject = (IComTest*)this;
		((IComTest*)(*ppvObject))->AddRef();
	}
	else{
		*ppvObject = NULL;
		return E_NOINTERFACE;
	}
	return S_OK;
}

ULONG _stdcall ComTestClass::AddRef()
{
	m_Ref++;
	return m_Ref;
}

ULONG _stdcall ComTestClass::Release()
{
	m_Ref--;
	if (0 == m_Ref){
		delete this;
		return 0;
	}
	return m_Ref;
}

int _stdcall ComTestClass::HelloWorld()
{
	return 89;
}

//int ComTestClass::Init()
//{
//	m_objNum = 0;
//	InitializeCriticalSection(&m_cs);
//	return 0;
//}
//
//ULONG ComTestClass::ObjNum()
//{
//	return m_objNum;
//}