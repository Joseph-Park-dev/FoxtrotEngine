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

class FTRectangle;

namespace Editor
{
	class EditorCamera :
		public D3D11::Camera
	{
	public:
		void DisplayMainCameraMenu();
		void DisplayEditorCameraMenu();

	public:
		D3D11::FTRectangle* GetDebugRect();

	public:
		void		 Initialize(Core::FTWindow* renderWindow, unsigned int pixels, float unit) override;
		virtual void ProcessInput(D3D11::D3D11InputDevice* inputDevice);
		virtual void Update(float deltaTime) override;

	private:
		bool  mPanKeyPressed;
		float mPanValModSpeed;
		float mZoomValModSpeed;

		D3D11::FTRectangle* mDebugRect;

		ImVec2 mMiddleMouseClickedPos;
		ImVec2 mMiddleMouseClickedRot;

	private:
		void PanLocalXY(Math::FTVector2 vec2);
	};
} // namespace Editor