#include "SpineAnimator.h"

#include "spine/spine.h"

#include "Managers/AnimationManager.h"
#include "Managers/ResourceManager.h"
#include "Managers/FTSpineLoader.h"
#include "ResourceSystem/Vertex.h"
#include "ResourceSystem/Mesh.h"
#include "ResourceSystem/FTTexture.h"
#include "ResourceSystem/FTBasicMeshGroup.h"
#include "ResourceSystem/FTMeshDataPack.h"
#include "ResourceSystem/Animation/FTSpineAnimation.h"
#include "Renderer/FoxtrotRenderer.h"
#include "Renderer/Camera.h"
#include "Core/FTCore.h"
#include "Actors/Transform.h"

void SpineAnimator::Initialize(FTCore* coreInst)
{
	SetRenderer(coreInst->GetGameRenderer());

	FTDS::String  path = "D:\\[2025_01]\\[GooCat Studio] VampiCat\\Assets\\BombCat_Spine\\Circle.json";
	FTDS::String  atlasPath = "D:\\[2025_01]\\[GooCat Studio] VampiCat\\Assets\\BombCat_Spine\\[Enemy] BombCat.atlas";

	spine::FTSpineLoader  spineLoader;
	mAtlas = new spine::Atlas(atlasPath.C_Str(), &spineLoader);
	spine::SkeletonData* skeletonData = spineLoader.ReadSkeletonJsonData(path.C_Str(), mAtlas, 1.0f);

	// Configure mixing.
	spine::AnimationStateData stateData(skeletonData);
	/*stateData.setMix("walk", "jump", 0.2f);
	stateData.setMix("jump", "run", 0.2f);*/

	mAnim = DBG_NEW FTSpineAnimation;
	mAnim->Initialize(skeletonData, &stateData);
	mAnim->SetTimeScale(1);
	//drawable.setUsePremultipliedAlpha(true);

	spine::Skeleton* skeleton = mAnim->GetSkeleton();
	skeleton->setToSetupPose();

	skeleton->setPosition(0.f, 0.f);
	skeleton->updateWorldTransform(spine::Physics_None);

	mAnim->GetAnimState()->addAnimation(0, "animation", true, 0);
}

void SpineAnimator::Update(float deltaTime)
{
	mAnim->Update(deltaTime, spine::Physics_None);
}

void SpineAnimator::Render(FoxtrotRenderer* renderer)
{
	mAnim->Render(renderer, GetOwner()->GetTransform(), Camera::GetInstance());
}

SpineAnimator::SpineAnimator(Actor* owner, int updateOrder)
	: MeshRenderer(owner, updateOrder)
	, mAnim(nullptr)
	, mAtlas(nullptr)
	, mTimeScale(1.f)
	, mUsePhysics(spine::Physics_Update)
{
}

SpineAnimator::~SpineAnimator()
{
	delete mAnim;
	delete mAtlas;
}

void SpineAnimator::CloneTo(Actor* actor)
{
	SpineAnimator* newComp = DBG_NEW SpineAnimator(actor, GetUpdateOrder());
}

#ifdef FOXTROT_EDITOR
void SpineAnimator::EditorUpdate(float deltaTime)
{
	Update(deltaTime);
}
#endif