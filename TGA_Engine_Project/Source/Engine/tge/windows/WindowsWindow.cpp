#include "stdafx.h"
#include <tge/windows/WindowsWindow.h>
#include "resource.h"
#include <WinUser.h>
#include <tge/imguiinterface/ImGuiInterface.h>

using namespace Tga;

WindowsWindow::WindowsWindow(void)
	:myWndProcCallback(nullptr)
{
}


WindowsWindow::~WindowsWindow(void)
{
}

bool WindowsWindow::Init(const EngineConfiguration &aWindowConfig, HINSTANCE &aHInstanceToFill, HWND*& aHwnd)
{
	myWndProcCallback = aWindowConfig.myWinProcCallback;
	HINSTANCE instance = GetModuleHandle(NULL);
	aHInstanceToFill = instance;

	ZeroMemory(&myWindowClass, sizeof(WNDCLASSEX));
	myWindowClass.cbSize = sizeof(WNDCLASSEX);
	myWindowClass.style = CS_HREDRAW | CS_VREDRAW;
	myWindowClass.lpfnWndProc = WindowProc;
	myWindowClass.hInstance = instance;
	myWindowClass.hCursor = LoadCursor(NULL, IDC_ARROW);
	myWindowClass.hbrBackground = (HBRUSH)COLOR_WINDOW;
	myWindowClass.lpszClassName = L"WindowClass1";
	myWindowClass.hIcon = ::LoadIcon(instance, MAKEINTRESOURCE(IDI_ICON1));
	myWindowClass.hIconSm = LoadIcon(instance, MAKEINTRESOURCE(IDI_ICON1));
	RegisterClassEx(&myWindowClass);

	const auto& windowSize = aWindowConfig.myWindowSize;

	RECT wr = {0, 0, static_cast<long>(windowSize.x), static_cast<long>(windowSize.y)};    // set the size, but not the position
	//AdjustWindowRect(&wr, WS_OVERLAPPEDWINDOW, FALSE);    // adjust the size

	DWORD windowStyle = 0;
	switch (aWindowConfig.myWindowSetting)
	{
	case WindowSetting::Overlapped:
		windowStyle = WS_OVERLAPPEDWINDOW;
		break;	
	case WindowSetting::Borderless:
		windowStyle = WS_VISIBLE | WS_POPUP | WS_CLIPSIBLINGS | WS_CLIPCHILDREN;
		break;
	default:
		break;
	}

	if (!aHwnd)
	{
		myWindowHandle = CreateWindowEx(WS_EX_APPWINDOW,
			L"WindowClass1",    // name of the window class
			aWindowConfig.myApplicationName.c_str(),    // title of the window
			windowStyle,    // window style
			CW_USEDEFAULT,    // x-position of the window
			CW_USEDEFAULT,    // y-position of the window
			wr.right - wr.left,    // width of the window
			wr.bottom - wr.top,    // height of the window
			NULL,    // we have no parent window, NULL
			NULL,    // we aren't using menus, NULL
			instance,    // application handle
			NULL);    // used with multiple windows, NULL
		
		ShowWindow(myWindowHandle, true);
		aHwnd = &myWindowHandle;
	}
	else
	{
		myWindowHandle = *aHwnd;
	}

	SetWindowLongPtr(myWindowHandle, GWLP_USERDATA, (LONG_PTR)this);




	// Fix to set the window to the actual resolution as the borders will mess with the resolution wanted
	myResolution = windowSize;
	myResolutionWithBorderDifference = myResolution;
	if (aWindowConfig.myWindowSetting == WindowSetting::Overlapped)
	{
		RECT r;
		GetClientRect(myWindowHandle, &r); //get window rect of control relative to screen
		int horizontal = r.right - r.left;
		int vertical = r.bottom - r.top;

		int diffX = windowSize.x - horizontal;
		int diffY = windowSize.y - vertical;

		SetResolution(windowSize + Vector2ui(diffX, diffY));
		myResolutionWithBorderDifference = windowSize + Vector2ui(diffX, diffY);
	}



	INFO_PRINT("%s %i %i", "Windows created with size ", windowSize.x, windowSize.y);

	return true;
}


#ifndef _RETAIL
IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
#endif
LRESULT WindowsWindow::LocWindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
#ifndef _RETAIL
	if (ImGui_ImplWin32_WndProcHandler(hWnd, message, wParam, lParam))
	{
		return S_OK;
	}
#endif
	if (myWndProcCallback)
	{
		return myWndProcCallback(hWnd, message, wParam, lParam);
	}
	return S_OK;
}

LRESULT CALLBACK WindowsWindow::WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	WindowsWindow* windowsClass = (WindowsWindow*)GetWindowLongPtr(hWnd, GWLP_USERDATA);
	if (windowsClass)
	{
		LRESULT result = windowsClass->LocWindowProc(hWnd, message, wParam, lParam);
		if (result)
		{
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
	}

	switch(message)
	{
		case WM_DESTROY:
			{
				PostQuitMessage(0);
				return 0;
			} break;

		case WM_SIZE:
		{
			if (Engine::GetInstance())
				Engine::GetInstance()->SetWantToUpdateSize();
			break;
		}
		

	}
	return DefWindowProc (hWnd, message, wParam, lParam);
}

void Tga::WindowsWindow::SetResolution(Vector2ui aResolution)
{
	::SetWindowPos(myWindowHandle, 0, 0, 0, aResolution.x, aResolution.y, SWP_NOMOVE | SWP_NOOWNERZORDER | SWP_NOZORDER);
}

void Tga::WindowsWindow::Close()
{
	DestroyWindow(myWindowHandle);
}
