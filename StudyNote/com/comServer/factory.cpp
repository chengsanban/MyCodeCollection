#include "factory.h"  
#include "ComTestClass.h"  

CompTestFactory::CompTestFactory()
{
	m_Ref = 0;
}

CompTestFactory::~CompTestFactory()
{

}

HRESULT _stdcall CompTestFactory::QueryInterface(const IID &riid, void **ppvObject)
{
	if (IID_IUnknown == riid){
		*ppvObject = (IUnknown*)this;
		((IUnknown*)(*ppvObject))->AddRef();
	}
	else if (IID_IClassFactory == riid){
		*ppvObject = (IClassFactory*)this;
		((IClassFactory*)(*ppvObject))->AddRef();
	}
	else{
		*ppvObject = NULL;
		return E_NOINTERFACE;
	}
	return S_OK;
}

ULONG _stdcall CompTestFactory::AddRef()
{
	m_Ref++;
	return m_Ref;
}

ULONG _stdcall CompTestFactory::Release()
{
	m_Ref--;
	if (0 == m_Ref){
		delete this;
		return 0;
	}
	return m_Ref;
}

HRESULT _stdcall CompTestFactory::CreateInstance(IUnknown *pUnkOuter, const IID &riid, void **ppvObject)
{
	if (NULL != pUnkOuter){
		return CLASS_E_NOAGGREGATION;
	}
	HRESULT hr = E_OUTOFMEMORY;
	//ComTestClass::Init();
	ComTestClass* pObj = new ComTestClass();
	if (NULL == pObj){
		return hr;
	}

	hr = pObj->QueryInterface(riid, ppvObject);
	if (S_OK != hr){
		delete pObj;
	}
	return hr;
}

HRESULT _stdcall CompTestFactory::LockServer(BOOL fLock)
{
	return NOERROR;
}