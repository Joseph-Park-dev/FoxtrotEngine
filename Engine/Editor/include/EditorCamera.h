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

#define IMGUI_DEFINE_MATH_OPERATORS
#include <imgui.h>

namespace D3D11
{
	class FTRectangle;
	class D3D11InputDevice;
} // namespace D3D11

namespace Editor
{
	class EditorWindow;
	class EditorCamera :
		public Core::ICamera
	{
	public:
		void DisplayMainCameraMenu();
		void DisplayEditorCameraMenu();

	public:
		D3D11::FTRectangle* GetDebugRect();

	public:
		void		 Initialize(Editor::EditorWindow* window, unsigned int pixels, float unit);
		virtual void ProcessInput(D3D11::D3D11InputDevice* inputDevice);
		virtual void Update(float deltaTime);

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