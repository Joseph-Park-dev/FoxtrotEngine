// ----------------------------------------------------------------
// Foxtrot Engine 2D
// Copyright (C) 2025 JungBae Park. All rights reserved.
//
// Released under the GNU General Public License v3.0
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------
/// <summary>
/// Additional camera used to render the editor scene, & navigate
/// through it.
/// </summary>

#pragma once
#include "Renderer/Camera.h"

class EditorCamera :
	public Camera
{
	SINGLETON(EditorCamera)

public:
	FTRectangle* GetDebugRect();

public:
	virtual void ProcessInput(FTInputDevice* inputDevice);
	virtual void Update(float deltaTime) override;

private:
	bool  mPanKeyPressed;
	bool  mRotationKeyPressed;
	float mPanValModSpeed;
	float mZoomValModSpeed;
	float mZoomDelta;

	FTRectangle* mDebugRect;

private:
	void PanLocalXY(ImVec2 vec2);
	void Zoom();
};
