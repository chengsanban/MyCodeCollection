//interface file
#ifndef ICOMTEST_H
#define ICOMTEST_H

#include <unknwn.h>

static const GUID IID_IComTest =
{ 0x81a80687, 0x6cc4, 0x4996, { 0x8d, 0xd2, 0xf0, 0x58, 0x90, 0x7f, 0xdc, 0xa8 } };

class IComTest : public IUnknown
{
public:
	virtual int _stdcall HelloWorld() = 0;
};
#endif