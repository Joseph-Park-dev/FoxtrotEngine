// ----------------------------------------------------------------
// Foxtrot Engine 2D
// Copyright (C) 2026 JungBae Park. All rights reserved.
//
// Released under the GNU General Public License v3.0
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

#include "Component/Button.h"

#include "Actor/IActor.h"
#include "Actor/Transform.h"
#include "Debugging/DebugMemAlloc.h"
#include "FileSystem/FileIOHelper.h"
#include "FTDS/Dynamic/DynamicArray.h"
#include "IInputDevice.h"
#include "Manager/ResourceManager.h"
#include "Renderer/ICamera.h"
#include "Plugin/UI.h"
#include "Renderer/Camera.h"
#include "Renderer/D3D11Renderer.h"
#include "ResourceSystem/Font/FTFont.h"
#include "ResourceSystem/ResourcePack.h"
#include "ResourceSystem/Shader/FTPixelShader.h"
#include "ResourceSystem/Shader/FTVertexShader.h"
#include "ResourceSystem/Mesh/Mesh.h"
#include "ResourceSystem/Shape/FTRectangle.h"
#include "Manager/DebugShapes.h"

#ifdef FOXTROT_EDITOR
	#include "Plugin/ICommandHistory.h"
#endif

namespace UI
{
	const char* Button::GetName()
	{
		return ChunkKey::Button::NAME;
	}

	bool Button::IsHovered() const
	{
		return mHovered;
	}

	bool Button::IsPressed() const
	{
		return mPressed;
	}

	bool Button::Contains(Graphics::ICamera* camera, InputSystem::IInputDevice* inputDevice) const
	{
		if (!mOwner || !mOwner->GetTransform() || !inputDevice)
			return false;

		Math::FTVector2 cursor(
			static_cast<float>(inputDevice->MOUSE_X()),
			static_cast<float>(inputDevice->MOUSE_Y()));
		Math::FTVector3 point  = camera ? camera->ScreenToWorld(cursor) : Math::FTVector3(cursor.x, cursor.y, 0.f);
		Math::FTVector3 origin = mOwner->GetTransform()->GetWorldPosition();
		const float		halfW  = mSize.x * 0.5f;
		const float		halfH  = mSize.y * 0.5f;
		return point.x >= origin.x - halfW && point.x <= origin.x + halfW && point.y >= origin.y - halfH && point.y <= origin.y + halfH;
	}

	void Button::UpdateDebugShapes(Math::FTVector3 pos, Math::FTVector3 rot, Math::FTVector3 scale, Graphics::ICamera* cameraInst)
	{
		if (!mDebugRect)
			return;

		if (mDebugRect->GetIsActive())
		{
			mDebugRect->UpdateModelMatrix(pos, rot, Math::FTVector3(1.f, 1.f, 1.f));
			mDebugRect->GetGSCData().size = Math::FTVector2(scale.x, scale.y);
			mDebugRect->UpdateGC(cameraInst);
			mDebugRect->UpdatePC();
		}
	}

	void Button::ProcessInput(InputSystem::IInputDevice* inputDevice)
	{
		mHovered = mVisible && Contains(GetUICamera(), inputDevice);
		if (!mInteractable || !mHovered || !inputDevice)
		{
			mPressed = false;
			return;
		}

		const bool leftDown = inputDevice->MOUSE_TAP(InputSystem::MOUSE::MOUSE_LEFT) || inputDevice->MOUSE_HOLD(InputSystem::MOUSE::MOUSE_LEFT);
		mPressed			= leftDown;
	}

	void Button::Render(Graphics::IRenderer* renderer, Graphics::ICamera* camInst)
	{
		(void)renderer;
		if (!mDebugRect)
			return;

		if (!mVisible || !camInst || !mOwner || !mOwner->GetTransform() || !mDebugRect->GetMesh() || !mDebugRect->GetMesh()->VertexBuffer)
		{
			mDebugRect->SetIsActive(false);
			return;
		}

		// Contains() tests an axis-aligned box of mSize centered on the actor.
		mDebugRect->SetIsActive(true);
		const Math::FTVector3 position = mOwner->GetTransform()->GetWorldPosition();
		const Math::FTVector3 rotation(0.f, 0.f, 0.f);
		const Math::FTVector3 scale(Math::Abs(mSize.x), Math::Abs(mSize.y), 1.f);
		UpdateDebugShapes(position, rotation, scale, camInst);
	}

	Button::Button(Common::IActor* owner)
		: UIComponent(owner)
		, mDebugRect(D3D11::DebugShapes::GetInstance()->AddRectangle())
		, mInteractable(true)
		, mHovered(false)
		, mPressed(false)
	{
	}

	Button::~Button() = default;

	void Button::CloneTo(Common::IActor* actor)
	{
		Button* copy = DBG_NEW Button(actor);
		CopyStyleTo(copy);
		copy->mInteractable = mInteractable;
		if (actor && actor->GetComponents())
			actor->GetComponents()->PushBack(copy);
		RegisterOwnedComponent(copy);
	}

	void Button::SaveProperties(std::ofstream& ofs)
	{
		UIComponent::SaveProperties(ofs);
		Common::FileIOHelper::SaveBool(ofs, ChunkKey::Button::INTERACTABLE, mInteractable);
	}

	void Button::LoadProperties(std::ifstream& ifs)
	{
		Common::FileIOHelper::LoadBool(ifs, mInteractable);
		UIComponent::LoadProperties(ifs);
	}

#ifdef FOXTROT_EDITOR
	void Button::EditorRender(Graphics::IRenderer* renderer, Graphics::ICamera* camInst)
	{
		Render(renderer, camInst);
	}

	void Button::EditorUIUpdate()
	{
		UIComponent::EditorUIUpdate();
		Editor::ICommandHistory* history = Editor::GetCommandHistory();
		if (!history)
			return;
		history->UpdateBoolValue(ChunkKey::Button::INTERACTABLE, mInteractable);
	}
#endif
} // namespace UI
