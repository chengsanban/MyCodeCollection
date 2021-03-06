#include "application.h"

int APIENTRY _tWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPTSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	MSG msg;

	MyRegisterClass(hInstance);

	if (!InitInstance (hInstance, nCmdShow))
	{
		return FALSE;
	}

	// Main msg loop
	while (GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

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
   HWND hWnd;

   hInst = hInstance;

   hWnd = CreateWindowEx(WS_EX_ACCEPTFILES, szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, NULL, NULL, hInstance, NULL);

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
	case WM_DROPFILES:
		OnDropFiles(hWnd, (HDROP)wParam);
		break;
	case WM_PAINT:
		HDC hdc;
		PAINTSTRUCT ps;
		hdc = BeginPaint(hWnd, &ps);
		TextOut(hdc, 200,200, L"Please drag mp4 on there!!!", lstrlen(L"Please drag mp4 on there!!!"));
		EndPaint(hWnd, &ps);
		break;
	case WM_CLOSE:
		if (MessageBox(hWnd, L"Are you sure close?", L"message", MB_YESNO) == IDYES)
			DestroyWindow(hWnd);
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}


VOID OnDropFiles(HWND hwnd, HDROP hDropInfo)
{
	UINT  nFileCount = ::DragQueryFile(hDropInfo, (UINT)-1, NULL, 0);
	TCHAR szFileName[_MAX_PATH] = _T("");
	DWORD dwAttribute;

	for (UINT i = 0; i < nFileCount; i++)
	{
		::DragQueryFile(hDropInfo, i, szFileName, sizeof(szFileName));
		dwAttribute = ::GetFileAttributes(szFileName);

		//folder
		if (dwAttribute & FILE_ATTRIBUTE_DIRECTORY)
		{
			::SetCurrentDirectory(szFileName);
			EnumerateFiles();
		}
		else //file
		{
			Mp4Box* mp4box = new Mp4Box(szFileName);
			mp4box->StartGenerateMp4BoxTree();
			mp4box->StartParseBoxTreeData();
			mp4box->StartPrintBoxTreeInfo();
			delete mp4box;
			MessageBox(NULL, L"parser success!!!", L"message", 0);
		}
	}

	::DragFinish(hDropInfo);
}


VOID EnumerateFiles()
{
	WIN32_FIND_DATA fd;
	HANDLE hFind = ::FindFirstFile(_T("*.*"), &fd);

	if (hFind != INVALID_HANDLE_VALUE)
	{
		do
		{
			//folder
			if (fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
			{
				if (_tcscmp(fd.cFileName, _T(".")) && _tcscmp(fd.cFileName, _T("..")))
				{
					::SetCurrentDirectory(fd.cFileName);
					EnumerateFiles();
					::SetCurrentDirectory(_T(".."));
				}
			}
			else //file
			{
				wstring strDir;
				TCHAR      lpDir[MAX_PATH];

				::GetCurrentDirectory(MAX_PATH, lpDir);
				wstring strdir(lpDir);
				strDir = lpDir;
				strDir += fd.cFileName;
			}
		} while (::FindNextFile(hFind, &fd));

		::FindClose(hFind);
	}
}