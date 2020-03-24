#include "D3d_Test_Project.h"


int APIENTRY _tWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPTSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	MSG msg;
	ZeroMemory(&msg, sizeof(msg));

	MyRegisterClass(hInstance);

	if (!InitInstance (hInstance, nCmdShow))
		return FALSE;

	//init d3d device
	if (FAILED(InitD3dDevice()))
		return FALSE;

	//set render status
	if (FAILED(Setup()))
		return FALSE;

	// Main msg loop
	while (msg.message != WM_QUIT)
	{
		//PeekMessage, not GetMessage
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else
		{
			Sleep(SLEEP_TIME);
			Present();
		}
	}
	CleanUp();
	return (int) msg.wParam;
}

ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= WndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInstance;
	wcex.hIcon			= LoadIcon(NULL, IDI_ERROR);
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName	= NULL;
	wcex.lpszClassName	= szWindowClass;
	wcex.hIconSm		= LoadIcon(NULL, IDI_ERROR);

	return RegisterClassEx(&wcex);
}

BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
 

   hInst = hInstance;

   hWnd = CreateWindow(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      0, 0, WIDTH, HEIGHT, NULL, NULL, hInstance, NULL);

   if (!hWnd)
   {
      return FALSE;
   }

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);
   return TRUE;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

//d3d function

HRESULT InitD3dDevice()
{
	HRESULT res = S_OK;
	pDirect3D9 = Direct3DCreate9(D3D_SDK_VERSION);

	D3DCAPS9 caps;
	pDirect3D9->GetDeviceCaps(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, &caps);
	//check vertex processing style
	int vp = 0;
	if (caps.DevCaps & D3DDEVCAPS_HWTRANSFORMANDLIGHT)
		vp = D3DCREATE_HARDWARE_VERTEXPROCESSING;
	else
		vp = D3DCREATE_SOFTWARE_VERTEXPROCESSING;

	//set d3d device param
	//D3DPRESENT_PARAMETERS Describes the presentation parameters.
	D3DPRESENT_PARAMETERS d3dpp;
	ZeroMemory(&d3dpp, sizeof(d3dpp));
	d3dpp.BackBufferWidth = WIDTH;
	d3dpp.BackBufferHeight = HEIGHT;
	d3dpp.BackBufferFormat = D3DFMT_A8R8G8B8;
	d3dpp.BackBufferCount = 1;
	d3dpp.MultiSampleType = D3DMULTISAMPLE_NONE;
	d3dpp.MultiSampleQuality = 0;
	d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
	d3dpp.hDeviceWindow = hWnd;
	d3dpp.Windowed = true;
	d3dpp.EnableAutoDepthStencil = true;
	d3dpp.AutoDepthStencilFormat = D3DFMT_D24S8; //32bit 24bit DepthStencil 8bit TemplateStencil
	d3dpp.Flags = 0;
	d3dpp.FullScreen_RefreshRateInHz = D3DPRESENT_RATE_DEFAULT;
	d3dpp.PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;

	res = pDirect3D9->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hWnd, vp, &d3dpp, &pDirect3DDevice9);
	
	return res;
}

HRESULT Setup()
{
	HRESULT res = S_OK;

	res |= pDirect3DDevice9->CreateVertexBuffer(
		3 * sizeof(ColorVertex),
		D3DUSAGE_WRITEONLY,
		ColorVertex::FVF,
		D3DPOOL_MANAGED,
		&pTriangleVertexBuffer,
		0);

	// Fill the buffer with the triangle data.

	ColorVertex* v;
	res |= pTriangleVertexBuffer->Lock(0, 0, (void**)&v, 0);
	v[0] = ColorVertex(-1.0f, 0.0f, 2.0f, D3DCOLOR_XRGB(255, 0, 0));
	v[1] = ColorVertex(0.0f, 1.0f, 2.0f, D3DCOLOR_XRGB(0, 255, 0));
	v[2] = ColorVertex(1.0f, 0.0f, 2.0f, D3DCOLOR_XRGB(0, 0, 255));
	res |= pTriangleVertexBuffer->Unlock();

	// Set the projection matrix.

	D3DXMATRIX proj;
	D3DXMatrixPerspectiveFovLH(
		&proj,
		D3DX_PI * 0.5f, // 90 - degree
		(float)WIDTH / (float)HEIGHT,
		1.0f,
		1000.0f);
	res |= pDirect3DDevice9->SetTransform(D3DTS_PROJECTION, &proj);

	// Turn off lighting.

	res |= pDirect3DDevice9->SetRenderState(D3DRS_LIGHTING, false);

	return res;
}

HRESULT Present()
{
	HRESULT res = S_OK;
	if (pDirect3DDevice9)
	{
		res |= pDirect3DDevice9->Clear(0, 0, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 0xffffffff, 1.0f, 0);
		res |= pDirect3DDevice9->BeginScene();

		res |= pDirect3DDevice9->SetFVF(ColorVertex::FVF);
		res |= pDirect3DDevice9->SetStreamSource(0, pTriangleVertexBuffer, 0, sizeof(ColorVertex));

		// draw the triangle to the left with flat shading
		D3DXMatrixTranslation(&WorldMatrix, -1.25f, 0.0f, 0.0f);
		res |= pDirect3DDevice9->SetTransform(D3DTS_WORLD, &WorldMatrix);

		res |= pDirect3DDevice9->SetRenderState(D3DRS_SHADEMODE, D3DSHADE_FLAT);
		res |= pDirect3DDevice9->DrawPrimitive(D3DPT_TRIANGLELIST, 0, 1);

		// draw the triangle to the right with gouraud shading
		D3DXMatrixTranslation(&WorldMatrix, 1.25f, 0.0f, 0.0f);
		res |= pDirect3DDevice9->SetTransform(D3DTS_WORLD, &WorldMatrix);

		res |= pDirect3DDevice9->SetRenderState(D3DRS_SHADEMODE, D3DSHADE_GOURAUD);
		res |= pDirect3DDevice9->DrawPrimitive(D3DPT_TRIANGLELIST, 0, 1);

		res |= pDirect3DDevice9->EndScene();
		res |= pDirect3DDevice9->Present(0, 0, 0, 0);
	}
	return res;
}

void CleanUp()
{
	SAFE_RELEASE(pDirect3D9);
	SAFE_RELEASE(pDirect3DDevice9);
	SAFE_RELEASE(pTriangleVertexBuffer);
}