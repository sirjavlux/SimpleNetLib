#include "stdafx.h"

#include "InputManager.h"
#include "Windowsx.h"

using namespace Tga;

InputManager::InputManager(HWND aWindowHandle)
{
	myOwnerHWND = aWindowHandle;
	
	#ifndef HID_USAGE_PAGE_GENERIC
	#define HID_USAGE_PAGE_GENERIC         ((USHORT) 0x01)
	#endif
	#ifndef HID_USAGE_GENERIC_MOUSE
	#define HID_USAGE_GENERIC_MOUSE        ((USHORT) 0x02)
	#endif

	RAWINPUTDEVICE Rid[1];
	Rid[0].usUsagePage = HID_USAGE_PAGE_GENERIC;
	Rid[0].usUsage = HID_USAGE_GENERIC_MOUSE;
	Rid[0].dwFlags = RIDEV_INPUTSINK;
	Rid[0].hwndTarget = myOwnerHWND;
	RegisterRawInputDevices(Rid, 1, sizeof(Rid[0]));
	
	myTentativeMousePosition = { 0,0 };
	myCurrentMousePosition = { 0,0 };
	myPreviousMousePosition = { 0,0 };
	myMouseDelta = { 0,0 };

	myTentativeMouseWheelDelta = 0;
	myMouseWheelDelta = 0;
}

bool InputManager::IsKeyHeld(const int aKeyCode) const
{
	return myCurrentState[aKeyCode] && myPreviousState[aKeyCode];
}

bool InputManager::IsKeyPressed(const int aKeyCode) const
{
	return myCurrentState[aKeyCode] && !myPreviousState[aKeyCode];
}

bool InputManager::IsKeyReleased(const int aKeyCode) const
{
	return !myCurrentState[aKeyCode] && myPreviousState[aKeyCode];
}

Vector2f InputManager::GetMouseDelta() const
{
	return myMouseDelta;
}

Vector2f InputManager::GetMousePosition() const
{
	return myCurrentMousePosition;
}

void InputManager::ShowMouse() const
{
	ShowCursor(true);
}

void InputManager::HideMouse() const
{
	ShowCursor(false);
}

void InputManager::CaptureMouse() const
{
	RECT clipRect;

	GetClientRect(myOwnerHWND, &clipRect);

	POINT upperLeft;
	upperLeft.x = clipRect.left;
	upperLeft.y = clipRect.top;

	POINT lowerRight;
	lowerRight.x = clipRect.right;
	lowerRight.y = clipRect.bottom;

	MapWindowPoints(myOwnerHWND, nullptr, &upperLeft, 1);
	MapWindowPoints(myOwnerHWND, nullptr, &lowerRight, 1);

	clipRect.left = upperLeft.x;
	clipRect.top = upperLeft.y;
	clipRect.right = lowerRight.x;
	clipRect.bottom = lowerRight.y;

	ClipCursor(&clipRect);
}

void InputManager::ReleaseMouse() const
{
	ClipCursor(nullptr);
}

bool InputManager::UpdateEvents(UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_KEYDOWN:
		myTentativeState[wParam] = true;
		return true;
	case WM_KEYUP:
		myTentativeState[wParam] = false;
		return true;

	case WM_LBUTTONDOWN:
		myTentativeState[VK_LBUTTON] = true;
		return true;

	case WM_LBUTTONUP:
		myTentativeState[VK_LBUTTON] = false;
		return true;

	case WM_RBUTTONDOWN:
		myTentativeState[VK_RBUTTON] = true;
		return true;

	case WM_RBUTTONUP:
		myTentativeState[VK_RBUTTON] = false;
		return true;

	case WM_MBUTTONDOWN:
		myTentativeState[VK_MBUTTON] = true;
		return true;

	case WM_MBUTTONUP:
		myTentativeState[VK_MBUTTON] = false;
		return true;
	case WM_SYSKEYDOWN:
		myTentativeState[wParam] = true;
		return true;
	case WM_SYSKEYUP:
		myTentativeState[wParam] = false;
		return true;

	case WM_XBUTTONDOWN:
		{
			const int xButton = GET_XBUTTON_WPARAM(wParam);
			if (xButton == 1)
				myTentativeState[VK_XBUTTON1] = true;
			else
				myTentativeState[VK_XBUTTON2] = true;

			return true;
		}

	case WM_XBUTTONUP:
		{
			const int xButton = GET_XBUTTON_WPARAM(wParam);
			if (xButton == 1)
				myTentativeState[VK_XBUTTON1] = false;
			else
				myTentativeState[VK_XBUTTON2] = false;
			return true;
		}

	case WM_MOUSEWHEEL:
		myTentativeMouseWheelDelta = GET_WHEEL_DELTA_WPARAM(wParam);
		return true;

	// This is only used for when you want X/Y coordinates.
	// Reason being is that it's clunky to rely on for delta
	// movements of the mouse. ClipRect and SetMousePos all
	// cause their own problems for input which are easily
	// solved by registering for the raw HID data and listening
	// for WM_INPUT instead.
	case WM_MOUSEMOVE:
		{
			const int xPos = GET_X_LPARAM(lParam);
			const int yPos = GET_Y_LPARAM(lParam);

			myTentativeMousePosition.X = static_cast<float>(xPos);
			myTentativeMousePosition.Y = static_cast<float>(yPos);

			return true;
		}

	// Handles mouse delta, used in 3D navigation etc.
	case WM_INPUT:
		{
			UINT dwSize = sizeof(RAWINPUT);
			static BYTE lpb[sizeof(RAWINPUT)];

			GetRawInputData((HRAWINPUT)lParam, RID_INPUT, lpb, &dwSize, sizeof(RAWINPUTHEADER));

			RAWINPUT* raw = (RAWINPUT*)lpb;

			if (raw->header.dwType == RIM_TYPEMOUSE)
			{
				myTentativeMouseDelta.X = static_cast<float>(raw->data.mouse.lLastX);
				myTentativeMouseDelta.Y = static_cast<float>(raw->data.mouse.lLastY);
			}
			return true;
		}
	}

	return false;
}

void InputManager::Update()
{
	myPreviousMousePosition = myCurrentMousePosition;
	myCurrentMousePosition = myTentativeMousePosition;

	myMouseDelta = myTentativeMouseDelta;
	myTentativeMouseDelta = { 0, 0};

	myMouseWheelDelta = myTentativeMouseWheelDelta / abs(myTentativeMouseWheelDelta);
	myTentativeMouseWheelDelta = 0;
	
	myPreviousState = myCurrentState;
	myCurrentState = myTentativeState;
}
