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

namespace Core
{
	class IInputDevice;
}

namespace D3D11
{
	class FTRectangle;
} // namespace D3D11

namespace Editor
{
	class EditorCamera :
		public Core::ICamera
	{
	public:
		void DisplayGameCameraMenu(Core::ICamera* gameCam);
		void DisplayEditorCameraMenu();

	public:
		D3D11::FTRectangle* GetDebugRect();

		float& ZoomFactor() override;

	public:
		void		 Initialize(Core::IWindow* window, unsigned int pixels, float unit);
		virtual void ProcessInput(Core::IInputDevice* inputDevice);
		virtual void Update(Core::ICamera* gameCamera);

	public:
		EditorCamera(Core::CameraData* data);
		~EditorCamera() override;

	private:
		Core::CameraData* mData;

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