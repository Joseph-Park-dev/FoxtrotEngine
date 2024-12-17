#include "Components/GunFiringBehavior.h"

#include "Managers/KeyInputManager.h"
#include "Managers/ResourceManager.h"
#include "Actors/Transform.h"
#include "Actors/ActorGroup.h"
#include "Actors/Actor.h"
#include "Physics/Physics2D.h"
#include "ActorBehaviors/Bullets/BulletBehavior.h"

#include "FileSystem/ChunkLoader.h"
#include "Core/EventFunctions.h"

#ifdef FOXTROT_EDITOR
#include "EditorSceneManager.h"
#include "EditorElement.h"
#endif // FOXTROT_EDITOR


void GunFiringBehavior::Initialize(FTCore* coreInstance)
{
	mBullet = ResourceManager::GetInstance()->GetLoadedPremade("Bullet");
	Component::Initialize(coreInstance);
}

void GunFiringBehavior::ProcessInput(KeyInputManager* keyInputManager)
{
	mDirection = CalcDirection(GetOwner()->GetTransform()->GetWorldPosition(), MOUSE_WORLDPOS);
}

void GunFiringBehavior::Update(float deltaTime)
{
	if (MOUSE_HOLD(MOUSE::MOUSE_LEFT))
	{
		if (mCurrentTick == 0)
		{
			ShootGun();
			mCurrentTick = mDelay;
		}
	}

	mCurrentTick -= deltaTime;

	if (mCurrentTick < 0)
		mCurrentTick = 0;
}

void GunFiringBehavior::ShootGun()
{
	mBullet = ResourceManager::GetInstance()->GetLoadedPremade("Bullet");
	//mBullet->GetOrigin()->Initialize(FTCoreEditor::GetInstance());
	EditorElement* bulletCopy = mBullet->AddToScene(EditorSceneManager::GetInstance()->GetEditorScene());
	//bulletCopy->Initialize(FTCoreEditor::GetInstance());
	BulletBehavior* behavior = bulletCopy->GetComponent<BulletBehavior>();
	Rigidbody2DComponent* rigidBody = bulletCopy->GetComponent<Rigidbody2DComponent>();

	b2Vec2 pos = (this->GetOwner()->GetTransform()->GetWorldPosition() + FTVector3(-2.f,0.f,0.f)).GetB2Vec2();
	b2Rot rot = b2Body_GetRotation(rigidBody->GetBodyID());

	FTVector3 dir = FTVector3(GetOwner()->GetTransform()->GetCurrentDirection(), 1.0f, 1.0f);
	bulletCopy->GetTransform()->SetWorldPosition(pos * dir);
	b2Body_SetTransform(rigidBody->GetBodyID(), pos, rot);

	behavior->SetImpulseQuantity(mDirection * mSpeed);
	behavior->Thrust();
}

GunFiringBehavior::GunFiringBehavior(Actor* owner, int drawOrder, int updateorder)
	: Component(owner, drawOrder, updateorder)
	, mTriggerKey(MOUSE::MOUSE_LEFT)
	, mDelay(0.2f)
	, mCurrentTick(0.f)
	, mDirection(FTVector2(1.0f,0.f))
	, mSpeed (10.f)
	, mBullet(nullptr)
{
}

GunFiringBehavior::GunFiringBehavior(Actor* owner, int drawOrder, int updateorder, MOUSE triggerKey, float delay)
	: Component(owner, drawOrder, updateorder)
	, mTriggerKey(triggerKey)
	, mDelay(delay)
	, mCurrentTick(0.f)
{

}

GunFiringBehavior::~GunFiringBehavior()
{
}

void GunFiringBehavior::CloneTo(Actor* actor)
{
	CLONE_TO_NOT_IMPLEMENTED
}

FTVector2&& GunFiringBehavior::CalcDirection(FTVector2&& origin, FTVector2&& point)
{
	return FTVector2(point.x - origin.x, point.y + origin.y);
}

void GunFiringBehavior::LoadProperties(std::ifstream& ifs)
{
	Component::LoadProperties(ifs);
}