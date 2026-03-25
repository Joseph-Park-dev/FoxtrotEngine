#pragma once
#include "InputSystem/FTInputDevice.h"

#include <Windows.h>

class D3D11InputDevice :
	public FTInputDevice
{
public:
	void DetectKeyInput() override;
	void DetectMouseInput(MSG msg);

	void LockCursorInSceneViewport(FTVector2 mousePos);
	void UnlockCursorOutOfSceneViewport();

private:
	// Keyboard related data.
	FTDS::DynamicArray<tKeyInfo>* mVecKey;
	FTDS::DynamicArray<tKeyInfo>* mVecMouse;
	FTDS::DynamicArray<tKeyInfo>* mVecButton;

	// Mouse related data.
	unsigned int mMousePosX, mMousePosY;
	int			 mMouseState;
	float		 mMouseWheelDelta;
	bool		 mIsDragging;

private:
	int* mKeyCode;
	int* mMouseCode;
};