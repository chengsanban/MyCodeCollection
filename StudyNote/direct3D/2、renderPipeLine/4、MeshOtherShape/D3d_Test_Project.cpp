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

	D3DXCreateTeapot(
		pDirect3DDevice9,
		&Objects[0],
		0);

	D3DXCreateBox(
		pDirect3DDevice9,
		2.0f, // width
		2.0f, // height
		2.0f, // depth
		&Objects[1],
		0);

	// cylinder is built aligned on z-axis
	D3DXCreateCylinder(
		pDirect3DDevice9,
		1.0f, // radius at negative z end
		1.0f, // radius at positive z end
		3.0f, // length of cylinder
		10,   // slices
		10,   // stacks
		&Objects[2],
		0);

	D3DXCreateTorus(
		pDirect3DDevice9,
		1.0f, // inner radius
		3.0f, // outer radius
		10,   // sides
		10,   // rings
		&Objects[3],
		0);

	D3DXCreateSphere(
		pDirect3DDevice9,
		1.0f, // radius
		10,   // slices
		10,   // stacks
		&Objects[4],
		0);

	D3DXMatrixTranslation(&ObjWorldMatrices[0], 0.0f, 0.0f, 0.0f);
	D3DXMatrixTranslation(&ObjWorldMatrices[1], -5.0f, 0.0f, 5.0f);
	D3DXMatrixTranslation(&ObjWorldMatrices[2], 5.0f, 0.0f, 5.0f);
	D3DXMatrixTranslation(&ObjWorldMatrices[3], -5.0f, 0.0f, -5.0f);
	D3DXMatrixTranslation(&ObjWorldMatrices[4], 5.0f, 0.0f, -5.0f);

	// Set the projection matrix.
	D3DXMATRIX proj;
	D3DXMatrixPerspectiveFovLH(
		&proj,
		D3DX_PI * 0.5f, // 90 - degree
		(float)WIDTH / (float)HEIGHT,
		1.0f,
		1000.0f);
	res |= pDirect3DDevice9->SetTransform(D3DTS_PROJECTION, &proj);

	// Switch to wireframe mode.
	res |= pDirect3DDevice9->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);

	return res;
}

HRESULT Present()
{
	HRESULT res = S_OK;
	if (pDirect3DDevice9)
	{
		// Animate the camera:
		// The camera will circle around the center of the scene.  We use the
		// sin and cos functions to generate points on the circle, then scale them
		// by 10 to further the radius.  In addition the camera will move up and down
		// as it circles about the scene.
		static float angle = (3.0f * D3DX_PI) / 2.0f;
		static float cameraHeight = 0.0f;
		static float cameraHeightDirection = 5.0f;

		D3DXVECTOR3 position(cosf(angle) * 10.0f, cameraHeight, sinf(angle) * 10.0f);

		// the camera is targetted at the origin of the world
		D3DXVECTOR3 target(0.0f, 0.0f, 0.0f);

		// the worlds up vector
		D3DXVECTOR3 up(0.0f, 1.0f, 0.0f);

		D3DXMATRIX V;
		D3DXMatrixLookAtLH(&V, &position, &target, &up);
		pDirect3DDevice9->SetTransform(D3DTS_VIEW, &V);

		// compute the position for the next frame
		angle += 0.1;
		if (angle >= 6.28f)
			angle = 0.0f;

		// compute the height of the camera for the next frame
		cameraHeight += cameraHeightDirection * 0.1;
		if (cameraHeight >= 10.0f)
			cameraHeightDirection = -5.0f;

		if (cameraHeight <= -10.0f)
			cameraHeightDirection = 5.0f;

		//
		// Draw the Scene:
		//

		pDirect3DDevice9->Clear(0, 0, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 0xffffffff, 1.0f, 0);
		pDirect3DDevice9->BeginScene();

		for (int i = 0; i < 5; i++)
		{
			// Set the world matrix that positions the object.
			pDirect3DDevice9->SetTransform(D3DTS_WORLD, &ObjWorldMatrices[i]);

			// Draw the object using the previously set world matrix.
			Objects[i]->DrawSubset(0);
		}

		pDirect3DDevice9->EndScene();
		pDirect3DDevice9->Present(0, 0, 0, 0);
	}
	return res;
}

void CleanUp()
{
	SAFE_RELEASE(pDirect3D9);
	SAFE_RELEASE(pDirect3DDevice9);
	for (int i = 0; i < 5; i++)
		SAFE_RELEASE(Objects[i]);
}