#include "Components/CharacterAI/AgentBehavior.h"

#include "Actors/Actor.h"
#include "Actors/Transform.h"
#include "FileSystem/FileIOHelper.h"
#include "FileSystem/ChunkFileKeys.h"
#include "Managers/SceneManager.h"

#ifdef FOXTROT_EDITOR
#include "EditorSceneManager.h"
#include "EditorUtils.h"
#endif // FOXTROT_EDITOR

Steering AgentBehavior::CreateSteering()
{
	return Steering::Halt();
}

Actor* AgentBehavior::GetTarget()
{
	return mTarget;
}

std::string& AgentBehavior::GetTargetName()
{
	return mTargetName;
}

void AgentBehavior::SetTarget(Actor* target)
{
	mTarget = target;
}

void AgentBehavior::SetTargetName(std::string& targetName)
{
	mTargetName.assign(targetName);
}

void AgentBehavior::Initialize(FTCore* coreInstance)
{
	Component::Initialize(coreInstance);
}

void AgentBehavior::Setup()
{
#ifdef FOXTROT_EDITOR
	if(mTargetName != ChunkKeys::NULL_OBJ)
		mTarget = FIND_EDITOR_ELEMENT(mTargetName);
#else
	if (mTargetName != ChunkKeys::NULL_OBJ)
		mTarget = FIND_ACTOR(mTargetName);
#endif // FOXTROT_EDITOR
	Component::Setup();
}

void AgentBehavior::Update(float deltaTime)
{
	GetOwner()->GetTransform()->SetSteering(CreateSteering());
}

AgentBehavior::AgentBehavior(Actor* owner, int drawOrder, int updateOrder)
	: Component		(owner, drawOrder, updateOrder)
	, mTarget		(nullptr)
	, mTargetName	()
{}

AgentBehavior::~AgentBehavior()
{}

void AgentBehavior::SaveProperties(std::ofstream & ofs)
{
	Component::SaveProperties(ofs);
	if(mTarget)
		FileIOHelper::SaveString(ofs, ChunkKeys::TARGET_NAME, mTargetName);
	else
		FileIOHelper::SaveString(ofs, ChunkKeys::TARGET_NAME, ChunkKeys::NULL_OBJ);
}

void AgentBehavior::LoadProperties(std::ifstream& ifs)
{
	FileIOHelper::LoadBasicString(ifs, mTargetName);
	Component::LoadProperties(ifs);
}

#ifdef FOXTROT_EDITOR
void AgentBehavior::EditorUIUpdate()
{
	DisplayActorSelection(ChunkKeys::TARGET_NAME, mTarget);
	if(mTarget)
		mTargetName = mTarget->GetName();
}
#endif // FOXTROT_EDITOR