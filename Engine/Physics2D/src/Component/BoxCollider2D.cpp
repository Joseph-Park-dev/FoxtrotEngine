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
#include "Core/FTCore.h"
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

			b2Polygon polygon = MakePolygon();
			GetShapeID()	  = b2CreatePolygonShape(rb->GetBodyID(), &polygonShapeDef, &polygon);

			if (!mDebugRect)
				mDebugRect = D3D11::DebugShapes::GetInstance()->AddRectangle();
		}
		Collider2D::Initialize();
	}

	void BoxCollider2D::Render(Graphics::IRenderer* renderer, Graphics::ICamera* camInst)
	{
		SyncDebugRect(renderer, camInst);
	}

	BoxCollider2D::BoxCollider2D(Common::IActor* owner)
		: Collider2D(owner)
		, mScale(1.f, 1.f)
		, mDebugRect(nullptr)
		, mDebugGpuFailed(false)
	{
	}

	BoxCollider2D::~BoxCollider2D()
	{
		if (b2Shape_IsValid(GetShapeID()))
			b2DestroyShape(GetShapeID(), true);
	}

	void BoxCollider2D::CloneTo(Common::IActor* actor)
	{
		BoxCollider2D* newComp = DBG_NEW BoxCollider2D(actor);
		newComp->SetShowDebugShape(IsShowingDebugShape());
		newComp->SetOffsetPos(GetOffsetPos());
		newComp->mScale		= mScale;

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

	b2Polygon BoxCollider2D::MakePolygon()
	{
		float halfWidth	 = Math::Abs(mScale.x) * 0.5f;
		float halfHeight = Math::Abs(mScale.y) * 0.5f;
		if (GetOwner() && GetOwner()->GetTransform())
		{
			const Math::FTVector3& ownerScale = GetOwner()->GetTransform()->GetWorldScale();
			halfWidth *= Math::Abs(ownerScale.x);
			halfHeight *= Math::Abs(ownerScale.y);
		}

		const Math::FTVector2 offset = GetOffsetPos();
		return b2MakeOffsetBox(halfWidth, halfHeight, b2Vec2{ offset.x, offset.y }, b2MakeRot(0.f));
	}

	void BoxCollider2D::ApplyPolygon()
	{
		if (!b2Shape_IsValid(GetShapeID()))
			return;

		b2Polygon polygon = MakePolygon();
		b2Shape_SetPolygon(GetShapeID(), &polygon);
	}

#ifdef FOXTROT_EDITOR
	void BoxCollider2D::SyncDebugRect(Graphics::IRenderer* renderer, Graphics::ICamera* camInst)
	{
		if (mDebugGpuFailed)
			return;

		if (!mDebugRect || !mDebugRect->GetMesh() || !mDebugRect->GetMesh()->VertexBuffer)
			return;

		mDebugRect->SetIsActive(true);

		if (!GetOwner() || !GetOwner()->GetTransform())
			return;

		Core::Transform*	   transform  = GetOwner()->GetTransform();
		const Math::FTVector2  offset	= GetOffsetPos();
		const Math::FTVector3& worldPos = transform->GetWorldPosition();
		const Math::FTVector3  worldRot = transform->GetWorldRotation();
		Math::FTVector3		   pos		= worldPos;
		float				   bodyAngle = worldRot.z;

		Rigidbody2D* rb = nullptr;
		if (GetOwner()->GetData() && GetOwner()->GetData()->Components)
			rb = GetOwner()->GetData()->GetComponent<Rigidbody2D>();
		if (rb && b2Body_IsValid(rb->GetBodyID()))
		{
			if (!Core::FTCore::GetInstance()->GetIsUpdating())
			{
				b2Body_SetTransform(
					rb->GetBodyID(),
					b2Vec2{ worldPos.x, worldPos.y },
					b2MakeRot(bodyAngle));
			}

			bodyAngle				= b2Rot_GetAngle(b2Body_GetRotation(rb->GetBodyID()));
			const b2Pos shapeCenter = b2Body_GetWorldPoint(rb->GetBodyID(), b2Vec2{ offset.x, offset.y });
			pos.x					= static_cast<float>(shapeCenter.x);
			pos.y					= static_cast<float>(shapeCenter.y);
		}
		else
		{
			const float cosine = Math::Cos(bodyAngle);
			const float sine   = Math::Sin(bodyAngle);
			pos.x += offset.x * cosine - offset.y * sine;
			pos.y += offset.x * sine + offset.y * cosine;
		}

		// DebugShapeVS builds the quad from one matrix column. A non-uniform scale
		// bends that column, so only the body yaw goes into the matrix.
		const Math::FTVector3  boxRot(0.f, 0.f, bodyAngle);
		const Math::FTVector3& worldScale = transform->GetWorldScale();
		const Math::FTVector3  scale(
			Math::Abs(worldScale.x * mScale.x),
			Math::Abs(worldScale.y * mScale.y),
			1.f);
		UpdateDebugShapes(pos, boxRot, scale, camInst);
	}

	void BoxCollider2D::EditorUpdate(float deltaTime)
	{
	}

	void BoxCollider2D::EditorRender(Graphics::IRenderer* renderer, Graphics::ICamera* camInst)
	{
		SyncDebugRect(renderer, camInst);
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
			mDebugRect->UpdateModelMatrix(pos, rot, Math::FTVector3(1.f, 1.f, 1.f));
			mDebugRect->GetGSCData().size = Math::FTVector2(scale.x, scale.y);
			mDebugRect->UpdateGC(cameraInst);
			mDebugRect->UpdatePC();
		}
	}

	void BoxCollider2D::ToggleDebugShape()
	{
		Collider2D::ToggleDebugShape();
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

			ApplyPolygon();
		}
	}
#endif
} // namespace Physics2D
