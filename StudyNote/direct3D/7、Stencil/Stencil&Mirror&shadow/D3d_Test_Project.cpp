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

	if (!InitInstance(hInstance, nCmdShow))
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
	return (int)msg.wParam;
}

ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = LoadIcon(NULL, IDI_ERROR);
	wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName = NULL;
	wcex.lpszClassName = szWindowClass;
	wcex.hIconSm = LoadIcon(NULL, IDI_ERROR);

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

	// Make walls have low specular reflectance - 20%.
	WallMtrl.Specular = WHITE * 0.2f;

	// Create the teapot.
	D3DXCreateTeapot(pDirect3DDevice9, &pTeapotMesh, 0);
	// Create and specify geometry.  For this sample we draw a floor
	// and a wall with a mirror on it.  We put the floor, wall, and
	// mirror geometry in one vertex buffer.
	//
	//   |----|----|----|
	//   |Wall|Mirr|Wall|
	//   |    | or |    |
	//   /--------------/
	//  /   Floor      /
	// /--------------/
	res |= pDirect3DDevice9->CreateVertexBuffer(
		24 * sizeof(Vertex),
		0, // usage
		FVF_VERTEX,
		D3DPOOL_MANAGED,
		&pVertexBuffer,
		0);

	Vertex* v = 0;
	res |= pVertexBuffer->Lock(0, 0, (void**)&v, 0);
	// floor
	v[0] = Vertex(-7.5f, 0.0f, -10.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f);
	v[1] = Vertex(-7.5f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f);
	v[2] = Vertex(7.5f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f);
	v[3] = Vertex(-7.5f, 0.0f, -10.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f);
	v[4] = Vertex(7.5f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f);
	v[5] = Vertex(7.5f, 0.0f, -10.0f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f);
	// wall
	v[6] = Vertex(-7.5f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f);
	v[7] = Vertex(-7.5f, 5.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f);
	v[8] = Vertex(-2.5f, 5.0f, 0.0f, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f);
	v[9] = Vertex(-7.5f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f);
	v[10] = Vertex(-2.5f, 5.0f, 0.0f, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f);
	v[11] = Vertex(-2.5f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f, 1.0f, 1.0f);
	// Note: We leave gap in middle of walls for mirror
	v[12] = Vertex(2.5f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f);
	v[13] = Vertex(2.5f, 5.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f);
	v[14] = Vertex(7.5f, 5.0f, 0.0f, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f);
	v[15] = Vertex(2.5f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f);
	v[16] = Vertex(7.5f, 5.0f, 0.0f, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f);
	v[17] = Vertex(7.5f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f, 1.0f, 1.0f);
	// mirror
	v[18] = Vertex(-2.5f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f);
	v[19] = Vertex(-2.5f, 5.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f);
	v[20] = Vertex(2.5f, 5.0f, 0.0f, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f);
	v[21] = Vertex(-2.5f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f);
	v[22] = Vertex(2.5f, 5.0f, 0.0f, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f);
	v[23] = Vertex(2.5f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f, 1.0f, 1.0f);
	res |= pVertexBuffer->Unlock();

	// Load Textures, set filters.
	D3DXCreateTextureFromFile(pDirect3DDevice9, L"floor.jpg", &pFloorTex);
	D3DXCreateTextureFromFile(pDirect3DDevice9, L"brick.jpg", &pWallTex);
	D3DXCreateTextureFromFile(pDirect3DDevice9, L"ice.bmp", &pMirrorTex);

	res |= pDirect3DDevice9->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
	res |= pDirect3DDevice9->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
	res |= pDirect3DDevice9->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_POINT);

	// Lights.
	D3DXVECTOR3 lightDir(0.707f, -0.707f, 0.707f);
	D3DXCOLOR color(1.0f, 1.0f, 1.0f, 1.0f);
	D3DLIGHT9 light = InitDirectionalLight(&lightDir, &color);

	res |= pDirect3DDevice9->SetLight(0, &light);
	res |= pDirect3DDevice9->LightEnable(0, true);

	res |= pDirect3DDevice9->SetRenderState(D3DRS_NORMALIZENORMALS, true);
	res |= pDirect3DDevice9->SetRenderState(D3DRS_SPECULARENABLE, true);

	// Set Camera.
	D3DXVECTOR3    pos(-10.0f, 3.0f, -15.0f);
	D3DXVECTOR3 target(0.0, 0.0f, 0.0f);
	D3DXVECTOR3     up(0.0f, 1.0f, 0.0f);
	D3DXMATRIX V;
	D3DXMatrixLookAtLH(&V, &pos, &target, &up);
	res |= pDirect3DDevice9->SetTransform(D3DTS_VIEW, &V);

	// Set projection matrix.
	D3DXMATRIX proj;
	D3DXMatrixPerspectiveFovLH(
		&proj,
		D3DX_PI / 4.0f, // 45 - degree
		(float)WIDTH / (float)HEIGHT,
		1.0f,
		1000.0f);
	res |= pDirect3DDevice9->SetTransform(D3DTS_PROJECTION, &proj);
	return res;
}

HRESULT Present()
{
	HRESULT res = S_OK;
	if (pDirect3DDevice9)
	{
		// Update the scene:
		static float radius = 20.0f;

		if (::GetAsyncKeyState(VK_LEFT) & 0x8000f)
			TeapotPosition.x -= 3.0f * 1;

		if (::GetAsyncKeyState(VK_RIGHT) & 0x8000f)
			TeapotPosition.x += 3.0f * 1;

		if (::GetAsyncKeyState(VK_UP) & 0x8000f)
			radius -= 2.0f * 1;

		if (::GetAsyncKeyState(VK_DOWN) & 0x8000f)
			radius += 2.0f * 1;


		static float angle = (3.0f * D3DX_PI) / 2.0f;

		if (::GetAsyncKeyState('A') & 0x8000f)
			angle -= 0.5f * 1;

		if (::GetAsyncKeyState('S') & 0x8000f)
			angle += 0.5f * 1;

		D3DXVECTOR3 position(cosf(angle) * radius, 3.0f, sinf(angle) * radius);
		D3DXVECTOR3 target(0.0f, 0.0f, 0.0f);
		D3DXVECTOR3 up(0.0f, 1.0f, 0.0f);
		D3DXMATRIX V;
		D3DXMatrixLookAtLH(&V, &position, &target, &up);
		res |= pDirect3DDevice9->SetTransform(D3DTS_VIEW, &V);

		// Draw the scene:
		res |= pDirect3DDevice9->Clear(0, 0,
			D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER | D3DCLEAR_STENCIL,
			0xff000000, 1.0f, 0L);

		res |= pDirect3DDevice9->BeginScene();

		res |= RenderScene();
		// draw shadow before mirror because the depth buffer hasn't been cleared
		// yet and we need the depth buffer so that the shadow is blended correctly.
		// That is, if an object obscures the shadow, we don't want to write the shadow
		// pixel.  Alternatively, we could redraw the scene to rewrite the depth buffer.
		// (RenderMirror clears the depth buffer).
		res |= RenderShadow();
		res |= RenderMirror();

		res |= pDirect3DDevice9->EndScene();
		res |= pDirect3DDevice9->Present(0, 0, 0, 0);
	}
	return res;
}

HRESULT RenderScene()
{
	HRESULT res = S_OK;
	if (pDirect3DDevice9)
	{
		// draw teapot
		res |= pDirect3DDevice9->SetMaterial(&TeapotMtrl);
		res |= pDirect3DDevice9->SetTexture(0, 0);
		D3DXMATRIX W;
		D3DXMatrixTranslation(&W,
			TeapotPosition.x,
			TeapotPosition.y,
			TeapotPosition.z);

		res |= pDirect3DDevice9->SetTransform(D3DTS_WORLD, &W);
		pTeapotMesh->DrawSubset(0);

		D3DXMATRIX I;
		D3DXMatrixIdentity(&I);
		res |= pDirect3DDevice9->SetTransform(D3DTS_WORLD, &I);

		res |= pDirect3DDevice9->SetStreamSource(0, pVertexBuffer, 0, sizeof(Vertex));
		res |= pDirect3DDevice9->SetFVF(FVF_VERTEX);

		// draw the floor
		res |= pDirect3DDevice9->SetMaterial(&FloorMtrl);
		res |= pDirect3DDevice9->SetTexture(0, pFloorTex);
		res |= pDirect3DDevice9->DrawPrimitive(D3DPT_TRIANGLELIST, 0, 2);

		// draw the walls
		res |= pDirect3DDevice9->SetMaterial(&WallMtrl);
		res |= pDirect3DDevice9->SetTexture(0, pWallTex);
		res |= pDirect3DDevice9->DrawPrimitive(D3DPT_TRIANGLELIST, 6, 4);

		// draw the mirror
		res |= pDirect3DDevice9->SetMaterial(&MirrorMtrl);
		res |= pDirect3DDevice9->SetTexture(0, pMirrorTex);
		res |= pDirect3DDevice9->DrawPrimitive(D3DPT_TRIANGLELIST, 18, 2);
	}
	return res;
}

HRESULT RenderMirror()
{
	HRESULT res = S_OK;
	if (pDirect3DDevice9)
	{
		//
		// Draw Mirror quad to stencil buffer ONLY.  In this way
		// only the stencil bits that correspond to the mirror will
		// be on.  Therefore, the reflected teapot can only be rendered
		// where the stencil bits are turned on, and thus on the mirror 
		// only.
		//
		res |= pDirect3DDevice9->SetRenderState(D3DRS_STENCILENABLE, true);
		res |= pDirect3DDevice9->SetRenderState(D3DRS_STENCILFUNC, D3DCMP_ALWAYS);
		res |= pDirect3DDevice9->SetRenderState(D3DRS_STENCILREF, 0x1);
		res |= pDirect3DDevice9->SetRenderState(D3DRS_STENCILMASK, 0xffffffff);
		res |= pDirect3DDevice9->SetRenderState(D3DRS_STENCILWRITEMASK, 0xffffffff);
		res |= pDirect3DDevice9->SetRenderState(D3DRS_STENCILZFAIL, D3DSTENCILOP_KEEP);
		res |= pDirect3DDevice9->SetRenderState(D3DRS_STENCILFAIL, D3DSTENCILOP_KEEP);
		res |= pDirect3DDevice9->SetRenderState(D3DRS_STENCILPASS, D3DSTENCILOP_REPLACE);

		// disable writes to the depth and back buffers
		res |= pDirect3DDevice9->SetRenderState(D3DRS_ZWRITEENABLE, false);
		res |= pDirect3DDevice9->SetRenderState(D3DRS_ALPHABLENDENABLE, true);
		res |= pDirect3DDevice9->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_ZERO);
		res |= pDirect3DDevice9->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ONE);

		// draw the mirror to the stencil buffer
		res |= pDirect3DDevice9->SetStreamSource(0, pVertexBuffer, 0, sizeof(Vertex));
		res |= pDirect3DDevice9->SetFVF(FVF_VERTEX);
		res |= pDirect3DDevice9->SetMaterial(&MirrorMtrl);
		res |= pDirect3DDevice9->SetTexture(0, pMirrorTex);
		D3DXMATRIX I;
		D3DXMatrixIdentity(&I);
		res |= pDirect3DDevice9->SetTransform(D3DTS_WORLD, &I);
		res |= pDirect3DDevice9->DrawPrimitive(D3DPT_TRIANGLELIST, 18, 2);

		// re-enable depth writes
		res |= pDirect3DDevice9->SetRenderState(D3DRS_ZWRITEENABLE, true);

		// only draw reflected teapot to the pixels where the mirror
		// was drawn to.
		res |= pDirect3DDevice9->SetRenderState(D3DRS_STENCILFUNC, D3DCMP_EQUAL);
		res |= pDirect3DDevice9->SetRenderState(D3DRS_STENCILPASS, D3DSTENCILOP_KEEP);

		// position reflection
		D3DXMATRIX W, T, R;
		D3DXPLANE plane(0.0f, 0.0f, 1.0f, 0.0f); // xy plane
		D3DXMatrixReflect(&R, &plane);

		D3DXMatrixTranslation(&T,
			TeapotPosition.x,
			TeapotPosition.y,
			TeapotPosition.z);

		W = T * R;

		// clear depth buffer and blend the reflected teapot with the mirror
		res |= pDirect3DDevice9->Clear(0, 0, D3DCLEAR_ZBUFFER, 0, 1.0f, 0);
		res |= pDirect3DDevice9->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_DESTCOLOR);
		res |= pDirect3DDevice9->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ZERO);

		// Finally, draw the reflected teapot
		res |= pDirect3DDevice9->SetTransform(D3DTS_WORLD, &W);
		res |= pDirect3DDevice9->SetMaterial(&TeapotMtrl);
		res |= pDirect3DDevice9->SetTexture(0, 0);

		// reverse cull mode
		res |= pDirect3DDevice9->SetRenderState(D3DRS_CULLMODE, D3DCULL_CW);

		pTeapotMesh->DrawSubset(0);

		// Restore render states.
		res |= pDirect3DDevice9->SetRenderState(D3DRS_ALPHABLENDENABLE, false);
		res |= pDirect3DDevice9->SetRenderState(D3DRS_STENCILENABLE, false);
		res |= pDirect3DDevice9->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
	}
	return res;
}

HRESULT RenderShadow()
{
	HRESULT res = S_OK;
	if (pDirect3DDevice9)
	{
		res |= pDirect3DDevice9->SetRenderState(D3DRS_STENCILENABLE, true);
		res |= pDirect3DDevice9->SetRenderState(D3DRS_STENCILFUNC, D3DCMP_EQUAL);
		res |= pDirect3DDevice9->SetRenderState(D3DRS_STENCILREF, 0x0);
		res |= pDirect3DDevice9->SetRenderState(D3DRS_STENCILMASK, 0xffffffff);
		res |= pDirect3DDevice9->SetRenderState(D3DRS_STENCILWRITEMASK, 0xffffffff);
		res |= pDirect3DDevice9->SetRenderState(D3DRS_STENCILZFAIL, D3DSTENCILOP_KEEP);
		res |= pDirect3DDevice9->SetRenderState(D3DRS_STENCILFAIL, D3DSTENCILOP_KEEP);
		res |= pDirect3DDevice9->SetRenderState(D3DRS_STENCILPASS, D3DSTENCILOP_INCR); // increment to 1

																	  // position shadow
		D3DXVECTOR4 lightDirection(0.707f, -0.707f, 0.707f, 0.0f);
		D3DXPLANE groundPlane(0.0f, -1.0f, 0.0f, 0.0f);

		D3DXMATRIX S;
		D3DXMatrixShadow(
			&S,
			&lightDirection,
			&groundPlane);

		D3DXMATRIX T;
		D3DXMatrixTranslation(
			&T,
			TeapotPosition.x,
			TeapotPosition.y,
			TeapotPosition.z);

		D3DXMATRIX W = T * S;
		res |= pDirect3DDevice9->SetTransform(D3DTS_WORLD, &W);

		// alpha blend the shadow
		res |= pDirect3DDevice9->SetRenderState(D3DRS_ALPHABLENDENABLE, true);
		res |= pDirect3DDevice9->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
		res |= pDirect3DDevice9->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);

		D3DMATERIAL9 mtrl = InitMtrl(BLACK, BLACK, BLACK, BLACK, 0.0f);
		mtrl.Diffuse.a = 0.5f; // 50% transparency.

							   // Disable depth buffer so that z-fighting doesn't occur when we
							   // render the shadow on top of the floor.
		res |= pDirect3DDevice9->SetRenderState(D3DRS_ZENABLE, false);

		res |= pDirect3DDevice9->SetMaterial(&mtrl);
		res |= pDirect3DDevice9->SetTexture(0, 0);
		pTeapotMesh->DrawSubset(0);

		res |= pDirect3DDevice9->SetRenderState(D3DRS_ZENABLE, true);
		res |= pDirect3DDevice9->SetRenderState(D3DRS_ALPHABLENDENABLE, false);
		res |= pDirect3DDevice9->SetRenderState(D3DRS_STENCILENABLE, false);
	}
	return res;
}

void CleanUp()
{
	SAFE_RELEASE(pDirect3D9);
	SAFE_RELEASE(pDirect3DDevice9);
	SAFE_RELEASE(pVertexBuffer);
	SAFE_RELEASE(pWallTex);
	SAFE_RELEASE(pFloorTex);
	SAFE_RELEASE(pMirrorTex);
	SAFE_RELEASE(pTeapotMesh);
}

D3DLIGHT9 InitDirectionalLight(D3DXVECTOR3* direction, D3DXCOLOR* color)
{
	D3DLIGHT9 light;
	::ZeroMemory(&light, sizeof(light));

	light.Type = D3DLIGHT_DIRECTIONAL;
	light.Ambient = *color * 0.6f;
	light.Diffuse = *color;
	light.Specular = *color * 0.6f;
	light.Direction = *direction;

	return light;
}

D3DLIGHT9 InitPointLight(D3DXVECTOR3* position, D3DXCOLOR* color)
{
	D3DLIGHT9 light;
	::ZeroMemory(&light, sizeof(light));

	light.Type = D3DLIGHT_POINT;
	light.Ambient = *color * 0.6f;
	light.Diffuse = *color;
	light.Specular = *color * 0.6f;
	light.Position = *position;
	light.Range = 1000.0f;
	light.Falloff = 1.0f;
	light.Attenuation0 = 1.0f;
	light.Attenuation1 = 0.0f;
	light.Attenuation2 = 0.0f;

	return light;
}

D3DLIGHT9 InitSpotLight(D3DXVECTOR3* position, D3DXVECTOR3* direction, D3DXCOLOR* color)
{
	D3DLIGHT9 light;
	::ZeroMemory(&light, sizeof(light));

	light.Type = D3DLIGHT_SPOT;
	light.Ambient = *color * 0.0f;
	light.Diffuse = *color;
	light.Specular = *color * 0.6f;
	light.Position = *position;
	light.Direction = *direction;
	light.Range = 1000.0f;
	light.Falloff = 1.0f;
	light.Attenuation0 = 1.0f;
	light.Attenuation1 = 0.0f;
	light.Attenuation2 = 0.0f;
	light.Theta = 0.4f;
	light.Phi = 0.9f;

	return light;
}

D3DMATERIAL9 InitMtrl(D3DXCOLOR a, D3DXCOLOR d, D3DXCOLOR s, D3DXCOLOR e, float p)
{
	D3DMATERIAL9 mtrl;
	mtrl.Ambient = a;
	mtrl.Diffuse = d;
	mtrl.Specular = s;
	mtrl.Emissive = e;
	mtrl.Power = p;
	return mtrl;
}