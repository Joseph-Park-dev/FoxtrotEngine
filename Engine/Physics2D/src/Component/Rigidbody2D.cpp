// ----------------------------------------------------------------
// Foxtrot Engine 2D
// Copyright (C) 2025 JungBae Park. All rights reserved.
//
// Released under the GNU General Public License v3.0
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

#include "Component/Rigidbody2D.h"

#include "Actor/IActor.h"
#include "Actor/Transform.h"
#include "Debugging/DebugMemAlloc.h"
#include "FileSystem/FileIOHelper.h"
#include "FTDS/Dynamic/DynamicArray.h"
#include "FTMath.h"
#include "Plugin/Physics2D.h"
#include "Component/Collider2D.h"

#ifdef FOXTROT_EDITOR
	#include "CommandHistory.h"
#endif

namespace Physics2D
{
	namespace
	{
		b2Vec2 ToB2Vec2(const Math::FTVector3& value)
		{
			return b2Vec2{ value.x, value.y };
		}

		b2Vec2 ToB2Vec2(const Math::FTVector2& value)
		{
			return b2Vec2{ value.x, value.y };
		}
	}

	Common::IActor* Rigidbody2D::GetOwner()
	{
		return mOwner;
	}

	b2BodyId& Rigidbody2D::GetBodyID()
	{
		return mBodyID;
	}

	void Rigidbody2D::Initialize()
	{
		PrepareForSimulation();
	}

	void Rigidbody2D::Setup()
	{
	}

	void Rigidbody2D::ProcessInput(InputSystem::IInputDevice* inputDevice)
	{
		(void)inputDevice;
	}

	void Rigidbody2D::Update(float deltaTime)
	{
		(void)deltaTime;
	}

	void Rigidbody2D::LateUpdate(float deltaTime)
	{
		(void)deltaTime;
		if (!GetOwner() || !GetOwner()->GetTransform() || !b2Body_IsValid(mBodyID))
			return;

		b2Pos		   position = b2Body_GetPosition(mBodyID);
		b2Rot		   rotation = b2Body_GetRotation(mBodyID);
		Math::FTVector3 current = GetOwner()->GetTransform()->GetWorldPosition();
		GetOwner()->GetTransform()->SetWorldPosition(Math::FTVector3(position.x, position.y, current.z));

		Math::FTVector3 prevRot	   = GetOwner()->GetTransform()->GetWorldRotation();
		Math::FTVector3 updatedRot = Math::FTVector3(prevRot.x, prevRot.y, -b2Rot_GetAngle(rotation));
		GetOwner()->GetTransform()->SetWorldRotation(updatedRot);
	}

	void Rigidbody2D::Render(Graphics::IRenderer* renderer, Graphics::ICamera* camInst)
	{
		(void)renderer;
		(void)camInst;
	}

	Rigidbody2D::Rigidbody2D(Common::IActor* owner)
		: mOwner(owner)
		, mBodyID(b2_nullBodyId)
#ifdef FOXTROT_EDITOR
		, mBodyDefCache(b2DefaultBodyDef())
#endif
	{
	}

	Rigidbody2D::~Rigidbody2D()
	{
		if (b2Body_IsValid(mBodyID))
			b2DestroyBody(mBodyID);
	}

	void Rigidbody2D::CloneTo(Common::IActor* actor)
	{
		Rigidbody2D* newComp = DBG_NEW Rigidbody2D(actor);
#ifdef FOXTROT_EDITOR
		newComp->mBodyDefCache = mBodyDefCache;
		if (GetPhysics2D())
			newComp->mBodyID = b2CreateBody(GetPhysics2DWorldID(), &newComp->mBodyDefCache);
#else
		if (Physics2D::GetInstance())
		{
			b2BodyDef bodyDef = b2DefaultBodyDef();
			if (actor && actor->GetTransform())
			{
				bodyDef.position = ToB2Vec2(actor->GetTransform()->GetWorldPosition());
				bodyDef.rotation = b2MakeRot(-actor->GetTransform()->GetWorldRotation().z);
			}
			newComp->mBodyID = b2CreateBody(Physics2D::GetInstance()->GetCurrentWorldID(), &bodyDef);
		}
#endif
		if (actor && actor->GetComponents())
			actor->GetComponents()->PushBack(newComp);
	}

	void Rigidbody2D::SaveProperties(std::ofstream& ofs)
	{
#ifdef FOXTROT_EDITOR
		Common::FileIOHelper::SaveBool(ofs, ChunkKey::Rigidbody2D::ALLOW_FAST_ROTATION, mBodyDefCache.allowFastRotation);
		Common::FileIOHelper::SaveFloat(ofs, ChunkKey::Rigidbody2D::ANGULAR_DAMPING, mBodyDefCache.angularDamping);
		Common::FileIOHelper::SaveFloat(ofs, ChunkKey::Rigidbody2D::ANGULAR_VELOCITY, mBodyDefCache.angularVelocity);
		Common::FileIOHelper::SaveBool(ofs, ChunkKey::Rigidbody2D::ENABLE_SLEEP, mBodyDefCache.enableSleep);
		Common::FileIOHelper::SaveBool(ofs, ChunkKey::Rigidbody2D::FIXED_ROTATION, mBodyDefCache.motionLocks.angularZ);
		Common::FileIOHelper::SaveFloat(ofs, ChunkKey::Rigidbody2D::GRAVITY_SCALE, mBodyDefCache.gravityScale);
		Common::FileIOHelper::SaveBool(ofs, ChunkKey::Rigidbody2D::IS_AWAKE, mBodyDefCache.isAwake);
		Common::FileIOHelper::SaveBool(ofs, ChunkKey::Rigidbody2D::IS_BULLET, mBodyDefCache.isBullet);
		Common::FileIOHelper::SaveBool(ofs, ChunkKey::Rigidbody2D::IS_ENABLED, mBodyDefCache.isEnabled);
		Common::FileIOHelper::SaveVector2(
			ofs,
			ChunkKey::Rigidbody2D::INITIAL_VELOCITY,
			Math::FTVector2(mBodyDefCache.linearVelocity.x, mBodyDefCache.linearVelocity.y));
		Common::FileIOHelper::SaveFloat(ofs, ChunkKey::Rigidbody2D::SLEEP_THRESHOLD, mBodyDefCache.sleepThreshold);

		int bodyType = ChunkKey::Rigidbody2D::BODY_TYPE_STATIC;
		switch (mBodyDefCache.type)
		{
		case b2_staticBody:
			bodyType = ChunkKey::Rigidbody2D::BODY_TYPE_STATIC;
			break;
		case b2_kinematicBody:
			bodyType = ChunkKey::Rigidbody2D::BODY_TYPE_KINEMATIC;
			break;
		case b2_dynamicBody:
			bodyType = ChunkKey::Rigidbody2D::BODY_TYPE_DYNAMIC;
			break;
		default:
			break;
		}
		Common::FileIOHelper::SaveInt(ofs, ChunkKey::Rigidbody2D::BODY_TYPE, bodyType);
#else
		(void)ofs;
#endif
	}

	void Rigidbody2D::LoadProperties(std::ifstream& ifs)
	{
		if (b2Body_IsValid(mBodyID))
			b2DestroyBody(mBodyID);

		b2BodyDef bodyDef	  = b2DefaultBodyDef();
		int		  bodyTypeInt = ChunkKey::Rigidbody2D::BODY_TYPE_STATIC;
		Common::FileIOHelper::LoadInt(ifs, bodyTypeInt);
		bodyDef.type = static_cast<b2BodyType>(bodyTypeInt);

		Common::FileIOHelper::LoadFloat(ifs, bodyDef.sleepThreshold);
		Math::FTVector2 linearVelocity(0.f, 0.f);
		Common::FileIOHelper::LoadVector2(ifs, linearVelocity);
		bodyDef.linearVelocity = ToB2Vec2(linearVelocity);
		Common::FileIOHelper::LoadBool(ifs, bodyDef.isEnabled);
		Common::FileIOHelper::LoadBool(ifs, bodyDef.isBullet);
		Common::FileIOHelper::LoadBool(ifs, bodyDef.isAwake);
		Common::FileIOHelper::LoadFloat(ifs, bodyDef.gravityScale);
		Common::FileIOHelper::LoadBool(ifs, bodyDef.motionLocks.angularZ);
		Common::FileIOHelper::LoadBool(ifs, bodyDef.enableSleep);
		Common::FileIOHelper::LoadFloat(ifs, bodyDef.angularVelocity);
		Common::FileIOHelper::LoadFloat(ifs, bodyDef.angularDamping);
		Common::FileIOHelper::LoadBool(ifs, bodyDef.allowFastRotation);

		if (GetOwner() && GetOwner()->GetTransform())
		{
			bodyDef.position = ToB2Vec2(GetOwner()->GetTransform()->GetWorldPosition());
			bodyDef.rotation = b2MakeRot(-GetOwner()->GetTransform()->GetWorldRotation().z);
		}

		if (GetPhysics2D())
			mBodyID = b2CreateBody(GetPhysics2DWorldID(), &bodyDef);

#ifdef FOXTROT_EDITOR
		mBodyDefCache = bodyDef;
#endif
	}

void Rigidbody2D::PrepareForSimulation()
	{
		if (!GetPhysics2D() || !b2World_IsValid(GetPhysics2DWorldID()))
			return;

		if (!b2Body_IsValid(mBodyID))
		{
#ifdef FOXTROT_EDITOR
			b2BodyDef bodyDef = mBodyDefCache;
#endif
			if (GetOwner() && GetOwner()->GetTransform())
			{
				bodyDef.position = ToB2Vec2(GetOwner()->GetTransform()->GetWorldPosition());
				bodyDef.rotation = b2MakeRot(-GetOwner()->GetTransform()->GetWorldRotation().z);
			}
			mBodyID = b2CreateBody(GetPhysics2DWorldID(), &bodyDef);
		}
		if (!b2Body_IsValid(mBodyID))
			return;

#ifdef FOXTROT_EDITOR
		if (b2Body_GetType(mBodyID) != mBodyDefCache.type)
			b2Body_SetType(mBodyID, mBodyDefCache.type);
		b2Body_SetGravityScale(mBodyID, mBodyDefCache.gravityScale);
#endif

		bool shapesAdded = false;
		if (GetOwner() && GetOwner()->GetComponents())
		{
			Common::FTDS::DynamicArray<Common::IComponent*>* components = GetOwner()->GetComponents();
			for (auto component = components->Begin(); component != components->End(); ++component)
			{
				if (Collider2D* col = static_cast<Collider2D*>(*component))
				{
					if (!b2Shape_IsValid(col->GetShapeID()))
					{
						col->Initialize();
						shapesAdded = shapesAdded || b2Shape_IsValid(col->GetShapeID());
					}
				}
			}
		}

		if (b2Body_GetType(mBodyID) != b2_dynamicBody)
			return;
		if (shapesAdded || b2Body_GetMass(mBodyID) <= 0.f)
		{
			b2Body_UpdateMassFromShapes(mBodyID);
			if (b2Body_GetMass(mBodyID) <= 0.f)
			{
				b2MassData mass		   = {};
				mass.mass			   = 1.f;
				mass.center			   = b2Vec2{ 0.f, 0.f };
				mass.rotationalInertia = 1.f;
				b2Body_SetMassData(mBodyID, mass);
			}
			b2Body_SetAwake(mBodyID, true);
		}
	}

#ifdef FOXTROT_EDITOR
	void Rigidbody2D::EditorUpdate(float deltaTime)
	{
		(void)deltaTime;
	}

	void Rigidbody2D::EditorRender(Graphics::IRenderer* renderer, Graphics::ICamera* camInst)
	{
		(void)renderer;
		(void)camInst;
	}

	void Rigidbody2D::EditorUIUpdate()
	{
		Editor::UpdateBoolValue(ChunkKey::Rigidbody2D::ALLOW_FAST_ROTATION, mBodyDefCache.allowFastRotation);
		Editor::UpdateFloatValue(ChunkKey::Rigidbody2D::ANGULAR_DAMPING, mBodyDefCache.angularDamping, FLOATMOD_SPEED);
		Editor::UpdateFloatValue(ChunkKey::Rigidbody2D::ANGULAR_VELOCITY, mBodyDefCache.angularVelocity, FLOATMOD_SPEED);
		Editor::UpdateBoolValue(ChunkKey::Rigidbody2D::ENABLE_SLEEP, mBodyDefCache.enableSleep);
		Editor::UpdateBoolValue(ChunkKey::Rigidbody2D::FIXED_ROTATION, mBodyDefCache.motionLocks.angularZ);
		Editor::UpdateFloatValue(ChunkKey::Rigidbody2D::GRAVITY_SCALE, mBodyDefCache.gravityScale, FLOATMOD_SPEED);
		Editor::UpdateBoolValue(ChunkKey::Rigidbody2D::IS_AWAKE, mBodyDefCache.isAwake);
		Editor::UpdateBoolValue(ChunkKey::Rigidbody2D::IS_BULLET, mBodyDefCache.isBullet);
		Editor::UpdateBoolValue(ChunkKey::Rigidbody2D::IS_ENABLED, mBodyDefCache.isEnabled);

		Math::FTVector2 velocity(mBodyDefCache.linearVelocity.x, mBodyDefCache.linearVelocity.y);
		Editor::UpdateVector2Value(ChunkKey::Rigidbody2D::INITIAL_VELOCITY, velocity, FLOATMOD_SPEED);
		mBodyDefCache.linearVelocity = ToB2Vec2(velocity);

		Editor::UpdateFloatValue(ChunkKey::Rigidbody2D::SLEEP_THRESHOLD, mBodyDefCache.sleepThreshold, FLOATMOD_SPEED);
		UpdateBodyType();

		if (GetOwner() && GetOwner()->GetTransform())
		{
			mBodyDefCache.position = ToB2Vec2(GetOwner()->GetTransform()->GetWorldPosition());
			mBodyDefCache.rotation = b2MakeRot(GetOwner()->GetTransform()->GetWorldRotation().z);
		}
	}

	void Rigidbody2D::UpdateBodyType()
	{
		int bodyType = static_cast<int>(mBodyDefCache.type);
		Editor::UpdateIntValue(ChunkKey::Rigidbody2D::BODY_TYPE, bodyType, INTMOD_SPEED);
		if (bodyType < static_cast<int>(b2_staticBody))
			bodyType = static_cast<int>(b2_staticBody);
		if (bodyType > static_cast<int>(b2_dynamicBody))
			bodyType = static_cast<int>(b2_dynamicBody);
		mBodyDefCache.type = static_cast<b2BodyType>(bodyType);
	}
#endif
} // namespace Physics2D
