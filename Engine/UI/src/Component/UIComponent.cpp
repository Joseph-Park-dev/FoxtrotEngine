// ----------------------------------------------------------------
// Foxtrot Engine 2D
// Copyright (C) 2026 JungBae Park. All rights reserved.
//
// Released under the GNU General Public License v3.0
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

#include "Component/UIComponent.h"

#include "Actor/IActor.h"
#include "Debugging/DebugMemAlloc.h"
#include "FileSystem/FileIOHelper.h"
#include "FTDS/Dynamic/DynamicArray.h"
#include "Plugin/UI.h"

#ifdef FOXTROT_EDITOR
	#include "Plugin/ICommandHistory.h"
#endif

namespace UI
{
	const char* UIComponent::GetName()
	{
		return ChunkKey::UIComponent::NAME;
	}

	Common::IActor* UIComponent::GetOwner()
	{
		return mOwner;
	}

	Math::FTVector2 UIComponent::GetSize() const
	{
		return mSize;
	}

	Math::FTVector3 UIComponent::GetColor() const
	{
		return mColor;
	}

	float UIComponent::GetAlpha() const
	{
		return mAlpha;
	}

	bool UIComponent::IsVisible() const
	{
		return mVisible;
	}

	void UIComponent::SetSize(Math::FTVector2 size)
	{
		mSize = size;
	}

	void UIComponent::SetColor(Math::FTVector3 color)
	{
		mColor = color;
	}

	void UIComponent::SetAlpha(float alpha)
	{
		mAlpha = alpha;
	}

	void UIComponent::SetVisible(bool visible)
	{
		mVisible = visible;
	}

	void UIComponent::Initialize()
	{
	}

	void UIComponent::Setup()
	{
	}

	void UIComponent::ProcessInput(InputSystem::IInputDevice* inputDevice)
	{
		(void)inputDevice;
	}

	void UIComponent::Update(float deltaTime)
	{
		(void)deltaTime;
	}

	void UIComponent::LateUpdate(float deltaTime)
	{
		(void)deltaTime;
	}

	void UIComponent::Render(Graphics::IRenderer* renderer, Graphics::ICamera* camInst)
	{
		(void)renderer;
		(void)camInst;
	}

	UIComponent::UIComponent(Common::IActor* owner)
		: mOwner(owner)
		, mSize(100.f, 40.f)
		, mColor(1.f, 1.f, 1.f)
		, mAlpha(1.f)
		, mVisible(true)
	{
	}

	UIComponent::~UIComponent()
	{
		UnregisterOwnedComponent(this);
	}

	void UIComponent::CloneTo(Common::IActor* actor)
	{
		UIComponent* copy = DBG_NEW UIComponent(actor);
		CopyStyleTo(copy);
		if (actor && actor->GetComponents())
			actor->GetComponents()->PushBack(copy);
		RegisterOwnedComponent(copy);
	}

	void UIComponent::CopyStyleTo(UIComponent* target) const
	{
		if (!target)
			return;
		target->mSize	 = mSize;
		target->mColor	 = mColor;
		target->mAlpha	 = mAlpha;
		target->mVisible = mVisible;
	}

	void UIComponent::SaveProperties(std::ofstream& ofs)
	{
		Common::FileIOHelper::SaveBool(ofs, ChunkKey::UIComponent::VISIBLE, mVisible);
		Common::FileIOHelper::SaveFloat(ofs, ChunkKey::UIComponent::ALPHA, mAlpha);
		Common::FileIOHelper::SaveVector3(ofs, ChunkKey::UIComponent::COLOR, mColor);
		Common::FileIOHelper::SaveVector2(ofs, ChunkKey::UIComponent::SIZE, mSize);
	}

	void UIComponent::LoadProperties(std::ifstream& ifs)
	{
		Common::FileIOHelper::LoadVector2(ifs, mSize);
		Common::FileIOHelper::LoadVector3(ifs, mColor);
		Common::FileIOHelper::LoadFloat(ifs, mAlpha);
		Common::FileIOHelper::LoadBool(ifs, mVisible);
	}

#ifdef FOXTROT_EDITOR
	void UIComponent::EditorUpdate(float deltaTime)
	{
		(void)deltaTime;
	}

	void UIComponent::EditorRender(Graphics::IRenderer* renderer, Graphics::ICamera* camInst)
	{
		Render(renderer, camInst);
	}

	void UIComponent::EditorUIUpdate()
	{
		Editor::ICommandHistory* history = Editor::GetCommandHistory();
		if (!history)
			return;
		history->UpdateBoolValue(ChunkKey::UIComponent::VISIBLE, mVisible);
		history->UpdateFloatValue(ChunkKey::UIComponent::ALPHA, mAlpha, Editor::FLOATMOD_SPEED);
		history->UpdateVector3Value(ChunkKey::UIComponent::COLOR, mColor, Editor::FLOATMOD_SPEED);
		history->UpdateVector2Value(ChunkKey::UIComponent::SIZE, mSize, Editor::FLOATMOD_SPEED);
	}
#endif
} // namespace UI
