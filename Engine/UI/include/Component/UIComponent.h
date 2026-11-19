// ----------------------------------------------------------------
// Foxtrot Engine 2D
// Copyright (C) 2026 JungBae Park. All rights reserved.
//
// Released under the GNU General Public License v3.0
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------
/// <summary>
/// Base component for screen-space UI controls.
/// </summary>
#pragma once
#include "Component/IComponent.h"

#include "FTMath.h"

namespace UI
{
	namespace ChunkKey
	{
		namespace UIComponent
		{
			constexpr const char* NAME	  = "UIComponent";
			constexpr const char* VISIBLE = "Visible";
			constexpr const char* ALPHA	  = "Alpha";
			constexpr const char* COLOR	  = "Color";
			constexpr const char* SIZE	  = "Size";
		} // namespace UIComponent
	} // namespace ChunkKey

	class UIComponent : public Common::IComponent
	{
	public:
		const char*		GetName() override;
		Common::IActor* GetOwner() override;

	public:
		Math::FTVector2	   GetSize() const;
		Math::FTVector3	   GetColor() const;
		float			   GetAlpha() const;
		bool			   IsVisible() const;

		void SetSize(Math::FTVector2 size);
		void SetColor(Math::FTVector3 color);
		void SetAlpha(float alpha);
		void SetVisible(bool visible);

	public:
		void Initialize() override;
		void Setup() override;
		void ProcessInput(InputSystem::IInputDevice* inputDevice) override;
		void Update(float deltaTime) override;
		void LateUpdate(float deltaTime) override;
		void Render(Graphics::IRenderer* renderer, Graphics::ICamera* camInst) override;

	public:
		explicit UIComponent(Common::IActor* owner);
		~UIComponent() override;

		void CloneTo(Common::IActor* actor) override;

	public:
		void SaveProperties(std::ofstream& ofs) override;
		void LoadProperties(std::ifstream& ifs) override;

	protected:
		/// @brief Copies the shared layout fields onto a component created for another actor.
		void CopyStyleTo(UIComponent* target) const;

		Common::IActor* mOwner;
		Math::FTVector2 mSize;
		Math::FTVector3 mColor;
		float			mAlpha;
		bool			mVisible;

#ifdef FOXTROT_EDITOR
	public:
		void EditorUpdate(float deltaTime) override;
		void EditorRender(Graphics::IRenderer* renderer, Graphics::ICamera* camInst) override;
		void EditorUIUpdate() override;
#endif
	};
} // namespace UI
