#include "D3d_Test_Project.h"

bool ComputeBoundingSphere(ID3DXMesh* mesh, BoundingSphere* sphere);
bool ComputeBoundingBox(ID3DXMesh* mesh, BoundingBox*    box);

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

	// Load the XFile data.
	ID3DXBuffer* adjBuffer = 0;
	ID3DXBuffer* mtrlBuffer = 0;
	DWORD        numMtrls = 0;

	res |= D3DXLoadMeshFromX(
		L"bigship1.x",
		D3DXMESH_MANAGED,
		pDirect3DDevice9,
		&adjBuffer,
		&mtrlBuffer,
		0,
		&numMtrls,
		&pMesh);

	if (FAILED(res))
	{
		::MessageBox(0, L"D3DXLoadMeshFromX() - FAILED", 0, 0);
		return res;
	}

	// Extract the materials, load textures.
	if (mtrlBuffer != 0 && numMtrls != 0)
	{
		D3DXMATERIAL* mtrls = (D3DXMATERIAL*)mtrlBuffer->GetBufferPointer();

		for (int i = 0; i < numMtrls; i++)
		{
			// the MatD3D property doesn't have an ambient value set
			// when its loaded, so set it now:
			mtrls[i].MatD3D.Ambient = mtrls[i].MatD3D.Diffuse;

			// save the ith material
			mtrlsVector.push_back(mtrls[i].MatD3D);

			// check if the ith material has an associative texture
			if (mtrls[i].pTextureFilename != 0)
			{
				// yes, load the texture for the ith subset
				IDirect3DTexture9* tex = 0;
				D3DXCreateTextureFromFileA(
					pDirect3DDevice9,
					mtrls[i].pTextureFilename,
					&tex);

				// save the loaded texture
				texturesVector.push_back(tex);
			}
			else
			{
				// no texture for the ith subset
				texturesVector.push_back(0);
			}
		}
	}
	SAFE_RELEASE(mtrlBuffer); // done w/ buffer

	// Optimize the mesh.

	res |= pMesh->OptimizeInplace(
		D3DXMESHOPT_ATTRSORT |
		D3DXMESHOPT_COMPACT |
		D3DXMESHOPT_VERTEXCACHE,
		(DWORD*)adjBuffer->GetBufferPointer(),
		0, 0, 0);

	SAFE_RELEASE(adjBuffer); // done w/ buffer

	if (FAILED(res))
	{
		::MessageBox(0, L"OptimizeInplace() - FAILED", 0, 0);
		return res;
	}

	// Compute Bounding Sphere and Bounding Box.
	BoundingSphere boundingSphere;
	BoundingBox    boundingBox;

	ComputeBoundingSphere(pMesh, &boundingSphere);
	ComputeBoundingBox(pMesh, &boundingBox);

	D3DXCreateSphere(
		pDirect3DDevice9,
		boundingSphere._radius,
		20,
		20,
		&pSphereMesh,
		0);

	D3DXCreateBox(
		pDirect3DDevice9,
		boundingBox._max.x - boundingBox._min.x,
		boundingBox._max.y - boundingBox._min.y,
		boundingBox._max.z - boundingBox._min.z,
		&pBoxMesh,
		0);

	// Set texture filters.
	res |= pDirect3DDevice9->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
	res |= pDirect3DDevice9->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
	res |= pDirect3DDevice9->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_POINT);

	// Set Lights.
	D3DXVECTOR3 dir(1.0f, -1.0f, 1.0f);
	D3DXCOLOR col(1.0f, 1.0f, 1.0f, 1.0f);
	D3DLIGHT9 light = InitDirectionalLight(&dir, &col);

	res |= pDirect3DDevice9->SetLight(0, &light);
	res |= pDirect3DDevice9->LightEnable(0, true);
	res |= pDirect3DDevice9->SetRenderState(D3DRS_NORMALIZENORMALS, true);
	res |= pDirect3DDevice9->SetRenderState(D3DRS_SPECULARENABLE, true);

	// Set camera.
	D3DXVECTOR3 pos(4.0f, 12.0f, -20.0f);
	D3DXVECTOR3 target(0.0f, 0.0f, 0.0f);
	D3DXVECTOR3 up(0.0f, 1.0f, 0.0f);

	D3DXMATRIX V;
	D3DXMatrixLookAtLH(
		&V,
		&pos,
		&target,
		&up);

	res |= pDirect3DDevice9->SetTransform(D3DTS_VIEW, &V);

	// Set projection matrix.
	D3DXMATRIX proj;
	D3DXMatrixPerspectiveFovLH(
		&proj,
		D3DX_PI * 0.5f, // 90 - degree
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
		//
		// Update: Rotate the mesh.
		//

		static float y = 0.0f;
		D3DXMATRIX yRot;
		D3DXMatrixRotationY(&yRot, y);
		y += 0.1;

		if (y >= 6.28f)
			y = 0.0f;

		D3DXMATRIX World = yRot;

		res |= pDirect3DDevice9->SetTransform(D3DTS_WORLD, &World);

		// Render
		res |= pDirect3DDevice9->Clear(0, 0, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 0xffffffff, 1.0f, 0);
		res |= pDirect3DDevice9->BeginScene();

		// draw the mesh
		for (int i = 0; i < mtrlsVector.size(); i++)
		{
			res |= pDirect3DDevice9->SetMaterial(&mtrlsVector[i]);
			res |= pDirect3DDevice9->SetTexture(0, texturesVector[i]);
			pMesh->DrawSubset(i);
		}

		//
		// Draw bounding volume in blue and at 10% opacity
		D3DMATERIAL9 blue = BLUE_MTRL;
		blue.Diffuse.a = 0.10f; // 10% opacity

		res |= pDirect3DDevice9->SetMaterial(&blue);
		res |= pDirect3DDevice9->SetTexture(0, 0); // disable texture

		res |= pDirect3DDevice9->SetRenderState(D3DRS_ALPHABLENDENABLE, true);
		res |= pDirect3DDevice9->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
		res |= pDirect3DDevice9->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);

		if (RenderBoundingSphere)
			pSphereMesh->DrawSubset(0);
		else
			pBoxMesh->DrawSubset(0);

		res |= pDirect3DDevice9->SetRenderState(D3DRS_ALPHABLENDENABLE, false);

		res |= pDirect3DDevice9->EndScene();
		res |= pDirect3DDevice9->Present(0, 0, 0, 0);
	}
	return res;
}

void CleanUp()
{
	SAFE_RELEASE(pDirect3D9);
	SAFE_RELEASE(pDirect3DDevice9);
	SAFE_RELEASE(pMesh);
	SAFE_RELEASE(pSphereMesh);
	SAFE_RELEASE(pBoxMesh);
	for (int i = 0; i < texturesVector.size(); i++)
		SAFE_RELEASE(texturesVector[i]);
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

BoundingBox::BoundingBox()
{
	// infinite small 
	_min.x = I_INFINITY;
	_min.y = I_INFINITY;
	_min.z = I_INFINITY;

	_max.x = I_INFINITY;
	_max.y = I_INFINITY;
	_max.z = I_INFINITY;
}

bool BoundingBox::isPointInside(D3DXVECTOR3& p)
{
	if (p.x >= _min.x && p.y >= _min.y && p.z >= _min.z &&
		p.x <= _max.x && p.y <= _max.y && p.z <= _max.z)
	{
		return true;
	}
	else
	{
		return false;
	}
}

BoundingSphere::BoundingSphere()
{
	_radius = 0.0f;
}

bool ComputeBoundingSphere(ID3DXMesh* mesh, BoundingSphere* sphere)
{
	HRESULT hr = 0;

	BYTE* v = 0;
	mesh->LockVertexBuffer(0, (void**)&v);

	hr = D3DXComputeBoundingSphere(
		(D3DXVECTOR3*)v,
		mesh->GetNumVertices(),
		D3DXGetFVFVertexSize(mesh->GetFVF()),
		&sphere->_center,
		&sphere->_radius);

	mesh->UnlockVertexBuffer();

	if (FAILED(hr))
		return false;

	return true;
}

bool ComputeBoundingBox(ID3DXMesh* mesh, BoundingBox* box)
{
	HRESULT hr = 0;

	BYTE* v = 0;
	mesh->LockVertexBuffer(0, (void**)&v);

	hr = D3DXComputeBoundingBox(
		(D3DXVECTOR3*)v,
		mesh->GetNumVertices(),
		D3DXGetFVFVertexSize(mesh->GetFVF()),
		&box->_min,
		&box->_max);

	mesh->UnlockVertexBuffer();

	if (FAILED(hr))
		return false;

	return true;
}
