// ----------------------------------------------------------------
// Foxtrot Engine 2D
// Copyright (C) 2026 JungBae Park. All rights reserved.
//
// Released under the GNU General Public License v3.0
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------
/// <summary>
/// Clickable UI control that draws a font string.
/// </summary>
#pragma once
#include "Component/UIComponent.h"
#include "Plugin/UIExports.h"

#include "FTDS/Static/FTString.h"

namespace D3D11
{
	class FTRectangle;
}

namespace UI
{
	namespace ChunkKey
	{
		namespace Button
		{
			constexpr const char* NAME		   = "Button";
			constexpr const char* TEXT		   = "Text";
			constexpr const char* FONT		   = "Font";
			constexpr const char* INTERACTABLE = "Interactable";
		} // namespace Button
	} // namespace ChunkKey

	class Button : public UIComponent
	{
	public:
		const char* GetName() override;

		UI_API virtual bool IsHovered() const;
		UI_API virtual bool IsPressed() const;

	public:
		void ProcessInput(InputSystem::IInputDevice* inputDevice) override;
		void Render(Graphics::IRenderer* renderer, Graphics::ICamera* camInst) override;

	public:
		explicit Button(Common::IActor* owner);
		~Button() override;

		void CloneTo(Common::IActor* actor) override;

	public:
		void SaveProperties(std::ofstream& ofs) override;
		void LoadProperties(std::ifstream& ifs) override;

	private:
		D3D11::FTRectangle*	 mDebugRect;
		bool				 mInteractable;
		bool				 mHovered;
		bool				 mPressed;

	private:
		bool			 Contains(Graphics::ICamera* camera, InputSystem::IInputDevice* inputDevice) const;
		Math::FTVector2	 WorldSize(Graphics::ICamera* camera) const;
		void			 UpdateDebugShapes(Math::FTVector3 pos, Math::FTVector3 rot, Math::FTVector3 scale, Graphics::ICamera* cameraInst);

#ifdef FOXTROT_EDITOR
	public:
		void EditorRender(Graphics::IRenderer* renderer, Graphics::ICamera* camInst) override;
		void EditorUIUpdate() override;
#endif
	};
} // namespace UI
