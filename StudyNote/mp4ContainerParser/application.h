#pragma once

#define WIN32_LEAN_AND_MEAN  
#include <windows.h>
#include <shellapi.h>

// C runtime
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>

//sdk version
#include <SDKDDKVer.h>

//other header
#include "mp4Box.h"

#define MAX_LOADSTRING 100

// Gloal variable 
HINSTANCE hInst;								// program instance
TCHAR szTitle[MAX_LOADSTRING] = L"AppName";					// title text
TCHAR szWindowClass[MAX_LOADSTRING] = L"WindowsClass";			// mian window class

// statement
ATOM				MyRegisterClass(HINSTANCE hInstance);
BOOL				InitInstance(HINSTANCE, int);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
VOID OnDropFiles(HWND hwnd, HDROP hDropInfo);
VOID EnumerateFiles();