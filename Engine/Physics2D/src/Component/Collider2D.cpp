// ----------------------------------------------------------------
// Foxtrot Engine 2D
// Copyright (C) 2025 JungBae Park. All rights reserved.
//
// Released under the GNU General Public License v3.0
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

#include "Component/Collider2D.h"

#include "Actor/IActor.h"
#include "Actor/Transform.h"
#include "Debugging/DebugMemAlloc.h"
#include "FileSystem/FileIOHelper.h"
#include "FTDS/Dynamic/DynamicArray.h"

#ifdef FOXTROT_EDITOR
	#include "CommandHistory.h"
#endif

namespace Physics2D
{
	Common::IActor* Collider2D::GetOwner()
	{
		return mOwner;
	}

	b2ShapeId& Collider2D::GetShapeID()
	{
		return mShapeID;
	}

	Math::FTVector2 Collider2D::GetOffsetPos() const
	{
		return mOffset;
	}

	Math::FTVector2 Collider2D::GetFinalPosition() const
	{
		return mFinalPosition;
	}

	CollisionStatesMap& Collider2D::GetCollisionStates()
	{
		return mCollisionStates;
	}

	Math::FTVector2& Collider2D::GetOffsetPosRef()
	{
		return mOffset;
	}

	void Collider2D::SetOffsetPos(Math::FTVector2 offsetPos)
	{
		mOffset = offsetPos;
	}

	void Collider2D::Initialize()
	{
	}

	void Collider2D::Setup()
	{
	}

	void Collider2D::ProcessInput(InputSystem::IInputDevice* inputDevice)
	{
		(void)inputDevice;
	}

	void Collider2D::Update(float deltaTime)
	{
		(void)deltaTime;
	}

	void Collider2D::LateUpdate(float deltaTime)
	{
		(void)deltaTime;
		if (!GetOwner() || !GetOwner()->GetTransform())
			return;

		const Math::FTVector3& ownerPos = GetOwner()->GetTransform()->GetWorldPosition();
		mFinalPosition					= Math::FTVector2(ownerPos.x, ownerPos.y) + mOffset;
	}

	void Collider2D::Render(Graphics::IRenderer* renderer, Graphics::ICamera* camInst)
	{
		(void)renderer;
		(void)camInst;
	}

	Collider2D::Collider2D(Common::IActor* owner)
		: mOwner(owner)
		, mShapeID(b2_nullShapeId)
		, mOffset(0.f, 0.f)
		, mFinalPosition(0.f, 0.f)
	{
	}

	Collider2D::Collider2D(const Collider2D& origin)
		: mOwner(origin.mOwner)
		, mShapeID(b2_nullShapeId)
		, mOffset(origin.mOffset)
		, mFinalPosition(origin.mFinalPosition)
	{
	}

	Collider2D::~Collider2D()
	{
		if (b2Shape_IsValid(mShapeID))
			b2DestroyShape(mShapeID, true);
	}

	void Collider2D::CloneTo(Common::IActor* actor)
	{
		Collider2D* newCollider = DBG_NEW Collider2D(actor);
		newCollider->mOffset	= mOffset;
		if (actor && actor->GetComponents())
			actor->GetComponents()->PushBack(newCollider);
	}

	void Collider2D::SaveProperties(std::ofstream& ofs)
	{
		Common::FileIOHelper::SaveVector2(ofs, ChunkKey::Collider2D::OFFSET, mOffset);
	}

	void Collider2D::LoadProperties(std::ifstream& ifs)
	{
		Common::FileIOHelper::LoadVector2(ifs, mOffset);
	}

#ifdef FOXTROT_EDITOR
	void Collider2D::EditorUpdate(float deltaTime)
	{
		(void)deltaTime;
	}

	void Collider2D::EditorRender(Graphics::IRenderer* renderer, Graphics::ICamera* camInst)
	{
		(void)renderer;
		(void)camInst;
	}

	void Collider2D::EditorUIUpdate()
	{
		UpdateOffsetPos();
	}

	void Collider2D::UpdateOffsetPos()
	{
		Math::FTVector2 updatedVal = GetOffsetPos();
		Editor::UpdateVector2Value("Offset Position", updatedVal, FLOATMOD_SPEED);
		mOffset = updatedVal;
	}
#endif
} // namespace Physics2D
