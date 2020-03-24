#pragma once

#define WIN32_LEAN_AND_MEAN  
#include <windows.h>

// C runtime
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>

//sdk version
#include <SDKDDKVer.h>

//d3d header
#include <d3d9.h>
#include <d3dx9.h>

//macro function
#define SAFE_RELEASE(x) if(x){(x)->Release();(x)=NULL;}

//windows const variable
#define MAX_LOADSTRING 100
#define WIDTH 800
#define HEIGHT 600
//d3d const variable
#define SLEEP_TIME 40
#define FVF_COLOR (D3DFVF_XYZ | D3DFVF_DIFFUSE)
#define FVF_NORMAL_TEX (D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_TEX1)
const D3DXCOLOR      WHITE(D3DCOLOR_XRGB(255, 255, 255));
const D3DXCOLOR      BLACK(D3DCOLOR_XRGB(0, 0, 0));
const D3DXCOLOR        RED(D3DCOLOR_XRGB(255, 0, 0));
const D3DXCOLOR      GREEN(D3DCOLOR_XRGB(0, 255, 0));
const D3DXCOLOR       BLUE(D3DCOLOR_XRGB(0, 0, 255));
const D3DXCOLOR     YELLOW(D3DCOLOR_XRGB(255, 255, 0));
const D3DXCOLOR       CYAN(D3DCOLOR_XRGB(0, 255, 255));
const D3DXCOLOR    MAGENTA(D3DCOLOR_XRGB(255, 0, 255));

// windows Gloal variable 
HINSTANCE hInst;								// program instance
TCHAR szTitle[MAX_LOADSTRING] = L"AppName";					// title text
TCHAR szWindowClass[MAX_LOADSTRING] = L"WindowsClass";			// mian window class
HWND hWnd;
//d3d Gloal variable
IDirect3D9* pDirect3D9 = NULL;
IDirect3DDevice9* pDirect3DDevice9 = NULL;
IDirect3DVertexBuffer9* pTriangleVertexBuffer = NULL;
D3DXMATRIX WorldMatrix;

// windows statement
ATOM				MyRegisterClass(HINSTANCE hInstance);
BOOL				InitInstance(HINSTANCE, int);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
//d3d statement
HRESULT InitD3dDevice();
HRESULT Setup();
HRESULT Present();
void CleanUp();

//struct
struct ColorVertex
{
	ColorVertex(){}

	ColorVertex(float x, float y, float z, D3DCOLOR c)
	{
		_x = x;	 _y = y;  _z = z;  _color = c;
	}

	float _x, _y, _z;
	D3DCOLOR _color;

	static const DWORD FVF;
};
const DWORD ColorVertex::FVF = FVF_COLOR;