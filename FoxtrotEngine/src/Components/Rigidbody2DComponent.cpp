#include "Components/Rigidbody2DComponent.h"

#include "Physics/Physics2D.h"
#include "Actors/Actor.h"
#include "Actors/Transform.h"
#include "Math/FTMath.h"
#include "FileSystem/ChunkLoader.h"
#include "FileSystem/ChunkFileKeys.h"

#ifdef FOXTROT_EDITOR
#include "CommandHistory.h"
#endif // FOXTROT_EDITOR


Rigidbody2DComponent::Rigidbody2DComponent(class Actor* owner, int drawOrder, int updateOrder)
	: Component(owner, drawOrder, updateOrder)
	, mMass(1.f)
	, mForce(FTVector2::Zero)
	, mAcceleration(FTVector2::Zero)
	, mAccelerationA(FTVector2::Zero)
	, mVelocity(FTVector2::Zero)
	, mMaxVelocity(FTVector2(200.f, 600.f))
	, mFrictionCoeff(0.1f)
	, mIsGrounded(false)
	, mIsBlockedUp(false)
#ifdef FOXTROT_EDITOR
	, mBodyDefCache(b2DefaultBodyDef())
#endif // FOXTROT_EDITOR

{}

Rigidbody2DComponent::~Rigidbody2DComponent()
{
	b2DestroyBody(mBodyID);
}

void Rigidbody2DComponent::CloneTo(Actor* actor)
{
	Rigidbody2DComponent* newComp = new Rigidbody2DComponent(actor, GetDrawOrder(), GetUpdateOrder());
	newComp->mMass = mMass;
	newComp->mMaxVelocity = mMaxVelocity;
}

void Rigidbody2DComponent::AddForce(FTVector2 force)
{
	mForce += force;
}

void Rigidbody2DComponent::AddVelocity(FTVector2 velocity)
{
	mVelocity += velocity;
}

void Rigidbody2DComponent::Initialize(FTCore* coreInstance)
{
	if(b2Body_IsValid(mBodyID))
		b2DestroyBody(mBodyID);

	b2BodyDef bodyDef = b2DefaultBodyDef();

	switch (mBodyType)
	{
	case BodyType::STATIC:
		bodyDef.type = b2_staticBody;
		break;
	case BodyType::KINEMATIC:
		bodyDef.type = b2_kinematicBody;
		break;
	case BodyType::DYNAMIC:
		bodyDef.type = b2_dynamicBody;
		break;
	default:
		break;
	}
	bodyDef.position = GetOwner()->GetTransform()->GetWorldPosition().GetB2Vec2();
	mBodyID = b2CreateBody(Physics2D::GetInstance()->GetCurrentWorldID(), &bodyDef);

	ColliderComponent* collider = GetOwner()->GetComponent<ColliderComponent>();
	if (collider) 
	{
		if (!collider->GetIsInitialized())
			collider->Initialize(coreInstance);
		b2Polygon& polygon = collider->GetPolygonRef();
		b2ShapeDef polygonShapeDef = b2DefaultShapeDef();
		collider->CreateShape(mBodyID, &polygonShapeDef, &polygon);
	}
	Component::Initialize(coreInstance);
}

void Rigidbody2DComponent::LateUpdate(float deltaTime)
{
	b2Vec2 position = b2Body_GetPosition(mBodyID);
	GetOwner()->GetTransform()->SetWorldPosition(FTVector2(position.x, position.y));

	/*UpdateLinearPosition(deltaTime);
	UpdateAcceleration(deltaTime);
	UpdateGravity(deltaTime);
	UpdateVelocity(deltaTime);
	ClearForceAndAccel();*/
}

void Rigidbody2DComponent::UpdateLinearPosition(float deltaTime)
{
	FTVector2 pos = GetOwner()->GetTransform()->GetWorldPosition();
	pos += mVelocity * deltaTime;
	GetOwner()->GetTransform()->SetWorldPosition(pos);
}

void Rigidbody2DComponent::UpdateAcceleration(float deltaTime)
{
	if (mForce.Length() != 0.f)
	{
		FTVector2 resultingAcc = mForce / mMass;
		mAcceleration += resultingAcc;
	}
	mAcceleration += mAccelerationA;
}

void Rigidbody2DComponent::UpdateGravity(float deltaTime)
{
	if (!mIsGrounded)
		mAccelerationA = FTVector2(0.f, 600.f);
	else
	{
		mAccelerationA = FTVector2::Zero;
		mVelocity = FTVector2(mVelocity.x, 0.f);
	}
	if (mIsBlockedUp)
	{
		mAccelerationA = FTVector2(0.f, 600.f);
		mVelocity = FTVector2(mVelocity.x, 0.f);
	}
}

void Rigidbody2DComponent::UpdateVelocity(float deltaTime)
{
	mVelocity += mAcceleration * deltaTime;

	// Applying friction to the object
	if (mIsGrounded && mVelocity.x != 0)
	{
		int frictionDir = -(mVelocity.x / Math::Abs(mVelocity.x));
		if (Math::Abs(mVelocity.x) < 0.1f)
			mVelocity.x = 0.f;
		else
			mVelocity *= powf(mFrictionCoeff, deltaTime);
	}

	if (Math::Abs(mMaxVelocity.x) <= Math::Abs(mVelocity.x))
	{
		mVelocity.x = (mVelocity.x / Math::Abs(mVelocity.x))
			* Math::Abs(mMaxVelocity.x);
	}
	if (Math::Abs(mMaxVelocity.y) <= Math::Abs(mVelocity.y))
	{
		mVelocity.y = (mVelocity.y / Math::Abs(mVelocity.y))
			* Math::Abs(mMaxVelocity.y);
	}
}

void Rigidbody2DComponent::ClearForceAndAccel()
{
	mForce = FTVector2::Zero;
	mAcceleration = FTVector2::Zero;
}

void Rigidbody2DComponent::SaveProperties(std::ofstream& ofs)
{
	Component::SaveProperties(ofs);
	switch (mBodyType)
	{
	case BodyType::STATIC:
		FileIOHelper::SaveInt(ofs, ChunkKeys::BODY_TYPE, ChunkKeys::BODY_TYPE_STATIC);
		break;
	case BodyType::KINEMATIC:
		FileIOHelper::SaveInt(ofs, ChunkKeys::BODY_TYPE, ChunkKeys::BODY_TYPE_KINEMATIC);
		break;
	case BodyType::DYNAMIC:
		FileIOHelper::SaveInt(ofs, ChunkKeys::BODY_TYPE, ChunkKeys::BODY_TYPE_DYNAMIC);
		break;
	default:
		break;
	}
}

void Rigidbody2DComponent::LoadProperties(std::ifstream& ifs)
{
	int bodyTypeInt = VALUE_NOT_ASSIGNED;
	FileIOHelper::LoadInt(ifs, bodyTypeInt);
	Component::LoadProperties(ifs);

	mBodyType = (BodyType)bodyTypeInt;
}

#ifdef FOXTROT_EDITOR
void Rigidbody2DComponent::SaveProperties(nlohmann::ordered_json& out)
{
	Component::SaveProperties(out);
}

void Rigidbody2DComponent::EditorUpdate(float deltaTime)
{
}

void Rigidbody2DComponent::EditorRender(FoxtrotRenderer* renderer)
{
}

void Rigidbody2DComponent::EditorUIUpdate()
{
	CommandHistory::GetInstance()->UpdateBoolValue("Allow Fast Rotation", mBodyDefCache.allowFastRotation);
	CommandHistory::GetInstance()->UpdateFloatValue("Angular Damping", &mBodyDefCache.angularDamping, FLOATMOD_SPEED);
	CommandHistory::GetInstance()->UpdateFloatValue("Angular Velocity", &mBodyDefCache.angularVelocity, FLOATMOD_SPEED);
	CommandHistory::GetInstance()->UpdateBoolValue("Enable Sleep", mBodyDefCache.enableSleep);
	CommandHistory::GetInstance()->UpdateBoolValue("Fixed Rotation", mBodyDefCache.fixedRotation);
	CommandHistory::GetInstance()->UpdateFloatValue("Gravity Scale", &mBodyDefCache.gravityScale, FLOATMOD_SPEED);

	CommandHistory::GetInstance()->UpdateBoolValue("Is Awake", mBodyDefCache.isAwake);
	CommandHistory::GetInstance()->UpdateBoolValue("Is Bullet", mBodyDefCache.isBullet);
	CommandHistory::GetInstance()->UpdateBoolValue("Is Enabled", mBodyDefCache.isEnabled);

	CommandHistory::GetInstance()->UpdateVector2Value("Initial Velocity", mBodyDefCache.linearVelocity, FLOATMOD_SPEED);

	CommandHistory::GetInstance()->UpdateFloatValue("Sleep Threshold", &mBodyDefCache.sleepThreshold, FLOATMOD_SPEED);
	UpdateBodyType();

	OnConfirmUpdate();

	ImGui::SeparatorText("Body Info");
	mBodyDefCache.position = GetOwner()->GetTransform()->GetWorldPosition().GetB2Vec2();
	std::string pos = {
		"x : " + std::to_string(mBodyDefCache.position.x) +
		"y : " + std::to_string(mBodyDefCache.position.y)
	};
	ImGui::TextColored(ImVec4(0.f, 200.f, 0.f, 255), pos.c_str());

	mBodyDefCache.rotation = b2MakeRot(GetOwner()->GetTransform()->GetRotation().z);
	std::string rot = {
		"Rotation : " + std::to_string(b2Rot_GetAngle(mBodyDefCache.rotation))
	};
	ImGui::TextColored(ImVec4(0.f, 200.f, 0.f, 255), rot.c_str());

	ImGui::Separator();
}

void Rigidbody2DComponent::OnConfirmUpdate()
{
	if(ImGui::Button("Update")) 
	{
		mBodyID = b2CreateBody(Physics2D::GetInstance()->GetCurrentWorldID(), &mBodyDefCache);
	}
}

void Rigidbody2DComponent::UpdateBodyType()
{
	const char* items[] = { "Static", "Kinematic", "Dynamic" };
	static int currentItem = VALUE_NOT_ASSIGNED;

	switch (mBodyType)
	{
	case BodyType::STATIC:
		currentItem = 0;
		break;
	case BodyType::KINEMATIC:
		currentItem = 1;
		break;
	case BodyType::DYNAMIC:
		currentItem = 2;
		break;
	default:
		break;
	}

	const char* comboPreview = items[currentItem];
	if (ImGui::BeginCombo("Body Type", comboPreview))
	{
		for (int n = 0; n < IM_ARRAYSIZE(items); n++)
		{
			const bool is_selected = (currentItem == n);
			if (ImGui::Selectable(items[n], is_selected)) {
				currentItem = n;
				mBodyType = (BodyType)currentItem;
			}

			// Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
			if (is_selected)
				ImGui::SetItemDefaultFocus();

		}
		ImGui::EndCombo();
	}
}

void Rigidbody2DComponent::UpdateGravityScale()
{
}
#endif // FOXTROT_EDITOR