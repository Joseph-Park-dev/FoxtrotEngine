#include "Components/CharacterAI/AgentBehavior.h"

#include "Actors/Actor.h"
#include "Actors/Transform.h"
#include "FileSystem/FileIOHelper.h"
#include "Managers/SceneManager.h"

#include "Compare/StringEqual.h"
#include "Utils/StrAssign.h"

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

FTDS::String& AgentBehavior::GetTargetName()
{
	return mTargetName;
}

void AgentBehavior::SetTarget(Actor* target)
{
	mTarget = target;
}

void AgentBehavior::SetTargetName(FTDS::String& targetName)
{
	mTargetName.Assign(targetName);
}

void AgentBehavior::Initialize(FTCore* coreInstance)
{
	Component::Initialize(coreInstance);
}

void AgentBehavior::Setup()
{
#ifdef FOXTROT_EDITOR
	if(mTargetName.NotEqual(ChunkKey::NullVal::NULL_OBJECT))
		mTarget = FIND_EDITOR_ELEMENT(mTargetName);
#else
	if (!FTDS::StringEqual(mTargetName.C_Str(), ChunkKey::NullVal::NULL_OBJECT))
		mTarget = FIND_ACTOR(mTargetName);
#endif // FOXTROT_EDITOR
	Component::Setup();
}

void AgentBehavior::Update(float deltaTime)
{
	GetOwner()->GetTransform()->SetSteering(CreateSteering());
}

AgentBehavior::AgentBehavior(Actor* owner, int updateOrder)
	: Component		(owner, updateOrder)
	, mTarget		(nullptr)
	, mTargetName	()
{}

AgentBehavior::~AgentBehavior()
{}

void AgentBehavior::SaveProperties(std::ofstream & ofs)
{
	Component::SaveProperties(ofs);
	FileIOHelper::SaveString(ofs, ChunkKey::TARGET_NAME, mTargetName);
}

void AgentBehavior::LoadProperties(std::ifstream& ifs)
{
	FileIOHelper::LoadBasicString(ifs, mTargetName);
	Component::LoadProperties(ifs);
}

#ifdef FOXTROT_EDITOR
void AgentBehavior::EditorUIUpdate()
{
	FTEditorUtils::DisplayActorSelection(ChunkKey::TARGET_NAME, mTarget);
	if(mTarget)
		mTargetName = mTarget->GetName();
}
#endif // FOXTROT_EDITOR