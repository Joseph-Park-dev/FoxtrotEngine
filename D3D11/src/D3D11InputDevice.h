#pragma once
#include "InputSystem/FTInputDevice.h"

class D3D11InputDevice :
	public FTInputDevice
{
public:
	void DetectKeyInput();
	void DetectMouseInput(MSG msg);

	void LockCursorInSceneViewport(FTVector2 mousePos);
	void UnlockCursorOutOfSceneViewport();
};