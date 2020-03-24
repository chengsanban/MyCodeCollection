#ifndef FACTORY_H  
#define FACTORY_H  

#include <unknwn.h>  


class CompTestFactory :
	public IClassFactory
{
public:
	CompTestFactory();
	~CompTestFactory();

	virtual HRESULT _stdcall QueryInterface(const IID& riid, void** ppvObject);
	virtual ULONG _stdcall AddRef();
	virtual ULONG _stdcall Release();

	virtual HRESULT _stdcall CreateInstance(IUnknown *pUnkOuter, const IID& riid, void **ppvObject);
	virtual HRESULT _stdcall LockServer(BOOL fLock);

protected:
	ULONG m_Ref;
};

#endif  