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

// windows Gloal variable 
HINSTANCE hInst;								// program instance
TCHAR szTitle[MAX_LOADSTRING] = L"AppName";					// title text
TCHAR szWindowClass[MAX_LOADSTRING] = L"WindowsClass";			// mian window class
HWND hWnd;
//d3d Gloal variable
IDirect3D9* pDirect3D9 = NULL;
IDirect3DDevice9* pDirect3DDevice9 = NULL;
IDirect3DVertexBuffer9* pCubeVertexBuffer = NULL;
IDirect3DIndexBuffer9* pCubeIndexBuffer = NULL;

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
struct Vertex
{
	Vertex(){}

	Vertex(float x, float y, float z)
	{
		_x = x;	 _y = y;  _z = z;
	}

	float _x, _y, _z;

	static const DWORD FVF;
};
const DWORD Vertex::FVF = D3DFVF_XYZ;