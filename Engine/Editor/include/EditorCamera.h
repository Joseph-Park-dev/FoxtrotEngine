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
#include "Renderer/ICamera.h"
#include "Renderer/CameraData.h"
#include "Core/InterfaceAliases.h"

#define IMGUI_DEFINE_MATH_OPERATORS
#include <imgui.h>


namespace D3D11
{
	class FTRectangle;
} // namespace D3D11

namespace Editor
{
	class EditorCamera :
		public Graphics::ICamera
	{
	public:
		//////////////////////////////////////////
		////// Coordinate Conversion /////////////
		//////////////////////////////////////////

		virtual Math::FTVector3 ScreenToWorld(const Math::FTVector2& screenPos) override;
		virtual Math::FTVector2 WorldToScreen(const Math::FTVector3& worldPos) const override;
		virtual Math::FTVector2 ScreenToNDC(const Math::FTVector2& screenPos) const override;

	public:
		void SaveProperties(std::ofstream& ofs) override;
		void LoadProperties(std::ifstream& ifs) override;

	public:
		void DisplayGameCameraMenu(Core::ICamera* gameCam);
		void DisplayEditorCameraMenu();

	public:
		virtual D3D11::CameraData* Data();

		//////////////////////////////////////////
		////// Transform Properties //////////////
		//////////////////////////////////////////

		virtual const Math::FTVector3& GetPosition() const override;

		//////////////////////////////////////////
		////// Matrix Accessors //////////////////
		//////////////////////////////////////////

		virtual void GetViewMatrix(Math::FTMatrix4& outViewMat) override;
		virtual void GetProjectionMatrix(Math::FTMatrix4& outProjMat) override;

	public:
		const Core::Viewtype GetViewType() override;
		const float			 GetProjFOVAngleY() override;
		const float			 GetAspectRatio() override;
		const float			 GetUnitsPerPixel() override;
		const float			 GetNearZ() override;
		const float			 GetFarZ() override;

		const Math::FTVector3&	GetOffSet() const override;
		const float				GetZoomFactor() const override;
		const Math::FTVector2&& GetResolution() const override;

		D3D11::FTRectangle* GetDebugRect();

		void SetPosition(const Math::FTVector3& pos) override;
		void SetViewType(Core::Viewtype viewType) override;
		void SetTargetActor(Core::IActor* actor) override;
		void SetOffset(Math::FTVector3 offset) override;

		float& ZoomFactor() override;

	public:
		void		 Initialize(Core::IWindow* window, unsigned int pixels, float unit);
		virtual void ProcessInput(Core::IInputDevice* inputDevice);
		virtual void Update(Core::ICamera* gameCamera);

	public:
		EditorCamera();
		~EditorCamera() override;

	protected:
		void InitializePixelsPerUnit(unsigned int pixels, float units) override;

	private:
		D3D11::CameraData* mData;

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