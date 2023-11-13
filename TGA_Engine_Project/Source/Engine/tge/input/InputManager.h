#pragma once
#include "Windows.h"
#include <bitset>
#include <tge/Math/Vector.h>

namespace Tga
{
/// <summary>
/// Very bare-bones InputManager to show the ropes of this assignment
/// Has nothing more than keyboard key checks but the
/// principle is the same for other types of input.
/// </summary>
class InputManager
{
	// Holds the "live" state that is being updated by
	// the message pump thread. This can be used in place
	// of myCurrentState but depending on how the game is
	// threaded it may be prudent to keep them separate.
	std::bitset<256> myTentativeState{};

	// The current snapshot when we last ran Update.
	std::bitset<256> myCurrentState{};

	// The previous snapshot.
	std::bitset<256> myPreviousState{};

	HWND myOwnerHWND;
	

	// TEMP PUBLIC
	Vector2f myTentativeMousePosition;
	Vector2f myCurrentMousePosition;
	Vector2f myPreviousMousePosition;

	Vector2f myTentativeMouseDelta;
	Vector2f myMouseDelta;

	float myTentativeMouseWheelDelta;
	float myMouseWheelDelta;

	
public:
	
	InputManager(HWND aWindowHandle);

	bool IsKeyHeld(const int aKeyCode) const;
	bool IsKeyPressed(const int aKeyCode) const;
	bool IsKeyReleased(const int aKeyCode) const;

	Vector2f GetMouseDelta() const;
	Vector2f GetMousePosition() const;

	void ShowMouse() const;
	void HideMouse() const;

	void CaptureMouse() const;
	void ReleaseMouse() const;

	bool UpdateEvents(UINT message, WPARAM wParam, LPARAM lParam);
	void Update();

	void BindAction(std::wstring Action, void (*actionInputFunction)());
	void BindAxis(std::wstring Axis, void (*axisInputFunction)(Vector2f axisValue));
};

} // namespace Tga