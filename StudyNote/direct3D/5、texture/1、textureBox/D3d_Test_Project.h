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


// windows statement
ATOM				MyRegisterClass(HINSTANCE hInstance);
BOOL				InitInstance(HINSTANCE, int);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
//windows const variable
#define MAX_LOADSTRING 100
#define WIDTH 800
#define HEIGHT 600
// windows Gloal variable 
HINSTANCE hInst;								// program instance
TCHAR szTitle[MAX_LOADSTRING] = L"AppName";					// title text
TCHAR szWindowClass[MAX_LOADSTRING] = L"WindowsClass";			// mian window class
HWND hWnd;


//d3d statement
HRESULT InitD3dDevice();
HRESULT Setup();
HRESULT Present();
void CleanUp();
D3DLIGHT9 InitDirectionalLight(D3DXVECTOR3* direction, D3DXCOLOR* color);
D3DLIGHT9 InitPointLight(D3DXVECTOR3* position, D3DXCOLOR* color);
D3DLIGHT9 InitSpotLight(D3DXVECTOR3* position, D3DXVECTOR3* direction, D3DXCOLOR* color);
D3DMATERIAL9 InitMtrl(D3DXCOLOR a, D3DXCOLOR d, D3DXCOLOR s, D3DXCOLOR e, float p);
//d3d Gloal variable
IDirect3D9* pDirect3D9 = NULL;
IDirect3DDevice9* pDirect3DDevice9 = NULL;
IDirect3DVertexBuffer9* pCubeVertexBuffer = NULL;
IDirect3DIndexBuffer9* pCubeIndexBuffer = NULL;
IDirect3DTexture9* pCubeTexture9 = NULL;
//d3d const variable
#define SLEEP_TIME 40
const D3DXCOLOR      WHITE(D3DCOLOR_XRGB(255, 255, 255));
const D3DXCOLOR      BLACK(D3DCOLOR_XRGB(0, 0, 0));
const D3DXCOLOR        RED(D3DCOLOR_XRGB(255, 0, 0));
const D3DXCOLOR      GREEN(D3DCOLOR_XRGB(0, 255, 0));
const D3DXCOLOR       BLUE(D3DCOLOR_XRGB(0, 0, 255));
const D3DXCOLOR     YELLOW(D3DCOLOR_XRGB(255, 255, 0));
const D3DXCOLOR       CYAN(D3DCOLOR_XRGB(0, 255, 255));
const D3DXCOLOR    MAGENTA(D3DCOLOR_XRGB(255, 0, 255));
const D3DMATERIAL9 WHITE_MTRL = InitMtrl(WHITE, WHITE, WHITE, BLACK, 2.0f);
const D3DMATERIAL9 RED_MTRL = InitMtrl(RED, RED, RED, BLACK, 2.0f);
const D3DMATERIAL9 GREEN_MTRL = InitMtrl(GREEN, GREEN, GREEN, BLACK, 2.0f);
const D3DMATERIAL9 BLUE_MTRL = InitMtrl(BLUE, BLUE, BLUE, BLACK, 2.0f);
const D3DMATERIAL9 YELLOW_MTRL = InitMtrl(YELLOW, YELLOW, YELLOW, BLACK, 2.0f);
//struct
struct Vertex
{
	Vertex(){}
	Vertex(
		float x, float y, float z,
		float nx, float ny, float nz,
		float u, float v)
	{
		_x = x;  _y = y;  _z = z;
		_nx = nx; _ny = ny; _nz = nz;
		_u = u;  _v = v;
	}
	float _x, _y, _z;
	float _nx, _ny, _nz;
	float _u, _v; // texture coordinates
};
#define FVF_VERTEX (D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_TEX1)