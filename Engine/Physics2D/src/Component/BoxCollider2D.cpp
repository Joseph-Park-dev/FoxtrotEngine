// ----------------------------------------------------------------
// Foxtrot Engine 2D
// Copyright (C) 2025 JungBae Park. All rights reserved.
//
// Released under the GNU General Public License v3.0
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

#include "Component/BoxCollider2D.h"
#include "Plugin/Physics2D.h"

#include "Actor/ActorData.h"
#include "Actor/IActor.h"
#include "Actor/Transform.h"
#include "Component/Rigidbody2D.h"
#include "Debugging/DebugMemAlloc.h"
#include "FileSystem/FileIOHelper.h"
#include "FTDS/Dynamic/DynamicArray.h"
#include "TemplateFunctions.h"

#ifdef FOXTROT_EDITOR
	#include "Core/InterfaceAliases.h"
	#include "Renderer/D3D11Renderer.h"
	#include "ResourceSystem/Shape/FTRectangle.h"
	#include "ResourceSystem/Mesh/Mesh.h"
	#include "Manager/DebugShapes.h"
	#include "Plugin/ICommandHistory.h"
	#include "Debugging/D3D11DebugFuncs.h"
#endif

namespace Physics2D
{
	const Math::FTVector2 BoxCollider2D::GetScale() const
	{
		return mScale;
	}

	void BoxCollider2D::SetScale(Math::FTVector2 scale)
	{
		mScale = scale;
	}

	void BoxCollider2D::Initialize()
	{
		if (b2Shape_IsValid(GetShapeID()))
			return;

		Rigidbody2D* rb = nullptr;
		if (GetOwner() && GetOwner()->GetData() && GetOwner()->GetData()->Components)
			rb = GetOwner()->GetData()->GetComponent<Rigidbody2D>();

		if (rb && b2Body_IsValid(rb->GetBodyID()) && GetOwner()->GetTransform())
		{
			b2ShapeDef polygonShapeDef			= b2DefaultShapeDef();
			polygonShapeDef.filter.categoryBits = static_cast<uint64_t>(GetOwner()->GetActorGroup());

			const Math::FTVector3& ownerScale = GetOwner()->GetTransform()->GetWorldScale();
			Math::FTVector2		   polygonScale(mScale.x * ownerScale.x, mScale.y * ownerScale.y);
			b2Polygon			   polygon = b2MakeBox(polygonScale.x / 2.f, polygonScale.y / 2.f);

			GetShapeID() = b2CreatePolygonShape(rb->GetBodyID(), &polygonShapeDef, &polygon);
		}

		Collider2D::Initialize();
	}

	BoxCollider2D::BoxCollider2D(Common::IActor* owner)
		: Collider2D(owner)
		, mScale(1.f, 1.f)
#ifdef FOXTROT_EDITOR
		, mDebugRect(nullptr)
		, mDebugGpuFailed(false)
#endif
	{
	}

	BoxCollider2D::~BoxCollider2D()
	{
		if (b2Shape_IsValid(GetShapeID()))
			b2DestroyShape(GetShapeID(), true);
#ifdef FOXTROT_EDITOR
		if (mDebugRect)
		{
			D3D11::DebugShapes::GetInstance()->RemoveShape(mDebugRect);
			mDebugRect = nullptr;
		}
#endif
	}

	void BoxCollider2D::CloneTo(Common::IActor* actor)
	{
		BoxCollider2D* newComp = DBG_NEW BoxCollider2D(actor);
		newComp->SetOffsetPos(GetOffsetPos());
		newComp->mScale = mScale;
		if (actor && actor->GetComponents())
			actor->GetComponents()->PushBack(newComp);
		RegisterOwnedComponent(newComp);
	}

	void BoxCollider2D::SaveProperties(std::ofstream& ofs)
	{
		Collider2D::SaveProperties(ofs);
		Common::FileIOHelper::SaveVector2(ofs, ChunkKey::BoxCollider2D::COLLIDER_SCALE, mScale);
	}

	void BoxCollider2D::LoadProperties(std::ifstream& ifs)
	{
		Common::FileIOHelper::LoadVector2(ifs, mScale);
		Collider2D::LoadProperties(ifs);
	}

	bool BoxCollider2D::RigidBodyExists()
	{
		return GetOwner()->GetData()->GetComponent<Rigidbody2D>() != nullptr;
	}

#ifdef FOXTROT_EDITOR
	void BoxCollider2D::EditorUpdate(float deltaTime)
	{
	}

	void BoxCollider2D::EditorRender(Graphics::IRenderer* renderer, Graphics::ICamera* camInst)
	{
		if (!IsShowingDebugShape() || mDebugGpuFailed)
			return;

		D3D11::D3D11Renderer* d3dRenderer = static_cast<D3D11::D3D11Renderer*>(renderer);
		if (!mDebugRect)
		{
			mDebugRect = D3D11::CreateFTRectangle();
			if (mDebugRect)
				D3D11::DebugShapes::GetInstance()->AddShape(mDebugRect);
		}

		// GPU buffers are created once a device exists. A failed create must not run again next frame.
		if (mDebugRect && d3dRenderer && mDebugRect->GetMesh() && !mDebugRect->GetMesh()->VertexBuffer)
		{
			try
			{
				mDebugRect->Initialize(d3dRenderer);
			}
			catch (const D3D11::DX::com_exception&)
			{
				D3D11::DebugShapes::GetInstance()->RemoveShape(mDebugRect);
				mDebugRect		 = nullptr;
				mDebugGpuFailed	 = true;
				return;
			}
		}

		if (mDebugRect)
			mDebugRect->SetIsActive(true);

		if (!GetOwner() || !GetOwner()->GetTransform())
			return;

		Core::Transform*	  transform = GetOwner()->GetTransform();
		const Math::FTVector2 offset	= GetOffsetPos();
		Math::FTVector3		  pos		= transform->GetWorldPosition();
		pos.x += offset.x;
		pos.y += offset.y;

		const Math::FTVector3  rot		  = transform->GetWorldRotation();
		const Math::FTVector3& worldScale = transform->GetWorldScale();
		const Math::FTVector3  scale(
			worldScale.x * mScale.x,
			worldScale.y * mScale.y,
			1.f);
		UpdateDebugShapes(pos, rot, scale, camInst);
	}

	void BoxCollider2D::EditorUIUpdate()
	{
		Collider2D::EditorUIUpdate();
		UpdateScale();
	}

	void BoxCollider2D::UpdateDebugShapes(Math::FTVector3 pos, Math::FTVector3 rot, Math::FTVector3 scale, Graphics::ICamera* cameraInst)
	{
		if (!mDebugRect)
			return;

		if (mDebugRect->GetIsActive())
		{
			mDebugRect->UpdateModelMatrix(pos, rot, scale);
			mDebugRect->GetGSCData().size = Math::FTVector2(scale.x, scale.y);
			mDebugRect->UpdateGC(cameraInst);
			mDebugRect->UpdatePC();
		}
	}

	void BoxCollider2D::ToggleDebugShape()
	{
		Collider2D::ToggleDebugShape();
		if (mDebugRect)
			mDebugRect->SetIsActive(IsShowingDebugShape());
	}

	void BoxCollider2D::UpdateScale()
	{
		Math::FTVector2 updatedVal = GetScale();
		Editor::GetCommandHistory()->UpdateVector2Value("Scale", updatedVal, Editor::FLOATMOD_SPEED);
		if (mScale != updatedVal)
		{
			mScale = updatedVal;
			if (mScale.x <= 0.f || mScale.y <= 0.f || !GetOwner() || !GetOwner()->GetTransform())
				return;

			const Math::FTVector3& ownerScale = GetOwner()->GetTransform()->GetWorldScale();
			Math::FTVector2		   resultantScale(mScale.x * ownerScale.x, mScale.y * ownerScale.y);
			b2Polygon			   polygon = b2MakeBox(resultantScale.x / 2.f, resultantScale.y / 2.f);
			if (b2Shape_IsValid(GetShapeID()))
				b2Shape_SetPolygon(GetShapeID(), &polygon);
		}
	}
#endif
} // namespace Physics2D
