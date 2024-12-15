#include "Actors/Actor.h"

#include <algorithm>
#include <functional>

#include "Scenes/Scene.h"
#include "Managers/KeyInputManager.h"

#include "Actors/ActorGroup.h"
#include "Actors/ActorTag.h"
#include "Actors/Transform.h"
#include "FileSystem/ChunkLoader.h"
#include "FileSystem/ChunkFileKeys.h"
#include "FileSystem/FileIOHelper.h"
#include "Components/Component.h"
#include "Core/FTCore.h"
#include "Debugging/DebugMemAlloc.h"

#ifdef FOXTROT_EDITOR
#include "EditorElement.h"
#endif // FOXTROT_EDITOR

Actor::Actor()
	: mTransform(DBG_NEW Transform)
	, mParent(nullptr)
{
	// THIS SHOULD BE REMAINED EMPTY;
}

Actor::Actor(Actor* origin)
	: mState(EActive)
	, mTransform(DBG_NEW Transform)
	, mActorGroup(origin->mActorGroup)
	, mParent(origin->mParent)
	, mComponents{}
	, mChild{}
{
	CopyTransformFrom(origin);
	CopyComponentsFrom(origin);
	CopyChildObject(origin);
}

Actor::Actor(Scene* scene)
	: mState(EActive)
	, mTransform(DBG_NEW Transform)
	, mActorGroup(ActorGroup::DEFAULT)
	, mParent(nullptr)
	, mComponents{}
	, mChild{}
{
	scene->AddActor(this, mActorGroup);
}

// Constructor to copy Actor
Actor::Actor(Actor& origin, Scene* scene)
	: mState(origin.mState)
	, mTransform(DBG_NEW Transform)
	, mActorGroup(origin.mActorGroup)
	, mParent(nullptr)
	, mComponents{}
	, mChild{}
{ 
	CopyTransformFrom(origin);
	CopyComponentsFrom(origin);
	CopyChildObject(origin);
	scene->AddActor(this, mActorGroup);
}

// Constructor to copy Actor
Actor::Actor(Actor* origin, Scene* scene)
	: mState(origin->mState)
	, mTransform(DBG_NEW Transform)
	, mActorGroup(origin->mActorGroup)
	, mParent(nullptr)
	, mComponents{}
	, mChild{}
{
	CopyTransformFrom(origin);
	CopyComponentsFrom(origin);
	CopyChildObject(origin);
	scene->AddActor(this, mActorGroup);
}

Actor::~Actor()
{
	if (mTransform)
	{
		delete mTransform;
		mTransform = nullptr;
	}

	for (size_t i = 0; i < mComponents.size(); ++i)
		delete mComponents[i];
	mComponents.clear();

	for (size_t i = 0; i < mChild.size(); ++i)
		mChild.clear();	
}

void Actor::CloneTo(Actor* target)
{
	target->mState = this->mState;
	target->mActorGroup = this->mActorGroup;
	target->CopyTransformFrom(this);
	target->CopyChildObject(this);
	target->CopyComponentsFrom(this);
	target->mParent = this->mParent;
}

void Actor::CopyTransformFrom(Actor& origin)
{
	Transform* originTransf = origin.GetTransform();
	mTransform->SetWorldPosition	(originTransf->GetWorldPosition());
	mTransform->SetLocalPosition	(originTransf->GetLocalPosition());
	mTransform->SetScale			(originTransf->GetScale());
	mTransform->SetRotation			(originTransf->GetRotation());
	mTransform->SetCurrentDirection(originTransf->GetCurrentDirection());
}

void Actor::CopyComponentsFrom(Actor& origin)
{
	this->RemoveAllComponents();

	std::vector<Component*>& compsToCopy = origin.GetComponents();
	for (size_t i = 0; i < compsToCopy.size(); ++i)
		compsToCopy[i]->CloneTo(this);
}

void Actor::CopyTransformFrom(Actor* origin)
{
	Transform* originTransf = origin->GetTransform();
	mTransform->SetWorldPosition(originTransf->GetWorldPosition());
	mTransform->SetLocalPosition(originTransf->GetLocalPosition());
	//copied->SetScreenPosition(originTransf->GetScreenPosition());
	mTransform->SetScale(originTransf->GetScale());
	mTransform->SetRotation(originTransf->GetRotation());
}

void Actor::CopyComponentsFrom(Actor* origin)
{
	this->RemoveAllComponents();

	std::vector<Component*>& compsToCopy = origin->GetComponents();
	for (size_t i = 0; i < compsToCopy.size(); ++i)
		compsToCopy[i]->CloneTo(this);
}

void Actor::CopyChildObject(Actor* origin)
{
	std::vector<Actor*>& childObjects = origin->GetChildActors();
	for (size_t i = 0; i < childObjects.size(); ++i)
		this->AddChild(DBG_NEW Actor(childObjects[i]));
}

void Actor::CopyChildObject(Actor& origin)
{
	std::vector<Actor*>& childObjects = origin.GetChildActors();
	for (size_t i = 0; i < childObjects.size(); ++i)
		this->AddChild(DBG_NEW Actor(childObjects[i]));
}

void Actor::Initialize(FTCore* coreInst)
{
	// Initializes Component
	for (size_t i = 0; i < mComponents.size(); ++i)
		if (!mComponents[i]->GetIsInitialized())
			mComponents[i]->Initialize(coreInst);
}

void Actor::ProcessInput(KeyInputManager* keyInputManager)
{
	if (IsActive())
	{
		for (auto comp : mComponents)
		{
			comp->ProcessInput(keyInputManager);
		}
	}
}

void Actor::UpdateComponents(float deltaTime)
{
	if (IsActive())
	{
		for (auto comp : mComponents)
		{
			comp->Update(deltaTime);
		}
	}
}

void Actor::LateUpdateComponents(float deltaTime)
{
	for (auto comp : mComponents)
	{
		comp->LateUpdate(deltaTime);
	}
}

void Actor::RenderComponents(FoxtrotRenderer* renderer)
{
	for (auto comp : mComponents)
	{
		comp->Render(renderer);
	}
}

//void Actor::RenderComponents(FoxtrotRenderer* renderer)
//{
//	for (auto comp : mComponents)
//	{
//		comp->Render(renderer);
//	}
//}

void Actor::AddComponent(Component* component)
{
	int updateOrder = component->GetUpdateOrder();
	int drawOrder = component->GetDrawOrder();
	auto iter = mComponents.begin();
	for (;iter != mComponents.end();++iter)
	{
		if (updateOrder < (*iter)->GetUpdateOrder())
		{
			if(drawOrder < (*iter)->GetDrawOrder())
				break;
		}
	}
	mComponents.insert(iter, component);
}

void Actor::RemoveComponent(Component* component)
{
	auto iter = std::find(mComponents.begin(), mComponents.end(), component);
	if (iter != mComponents.end())
	{
		delete component;
		mComponents.erase(iter);
	}
}

void Actor::RemoveAllComponents()
{
	for (size_t i = 0; i < mComponents.size(); ++i)
		delete mComponents[i];
	mComponents.clear();
}

void Actor::OnCollisionEnter(Collider2DComponent* _pOther)
{
	LogString("CollisionEnter");
}

void Actor::OnCollisionStay(Collider2DComponent* _pOther)
{
}

void Actor::OnCollisionExit(Collider2DComponent* _pOther)
{
}

void Actor::OnRayEnter()
{
}

std::string Actor::GetStateStr() const
{
	std::string state = "active";
	if (mState == EPaused)
		state = "paused";
	else if (mState == EDead)
		state = "dead";
	return state;
}

void Actor::SetState(std::string state)
{
	if (state == "paused")
		mState = EPaused;
	else if (state == "dead")
		mState = EDead;
	else
		mState = EActive;
}

//void Actor::SaveCharacterData(FILE* file)
//{
//	fwrite(&GetTransform()->GetWorldPosition(), sizeof(Vector2), 1, file);
//}

void Actor::SaveProperties(std::ofstream& ofs)
{
	FileIOHelper::BeginDataPackSave(ofs, ChunkKeys::ACTOR_PROPERTIES);
	FileIOHelper::SaveString		(ofs, ChunkKeys::NAME, GetName());
	FileIOHelper::SaveInt			(ofs, ChunkKeys::ID, mID);
	FileIOHelper::SaveString		(ofs, ChunkKeys::ACTOR_GROUP, ActorGroupUtil::GetActorGroupStr(mActorGroup));
	FileIOHelper::SaveString		(ofs, ChunkKeys::STATE, GetStateStr());
	if (mParent)
		FileIOHelper::SaveString	(ofs, ChunkKeys::PARENT, mParent->GetName());
	else
		FileIOHelper::SaveString	(ofs, ChunkKeys::PARENT, "nullptr");

	// Changing the call location of Transform is NOT recommended
	// Nested .chunk DataPack has unknown problem.
	mTransform->SaveProperties		(ofs);
	FileIOHelper::EndDataPackSave	(ofs, ChunkKeys::ACTOR_PROPERTIES);
}

void Actor::SaveComponents(std::ofstream& ofs)
{
	size_t count = mComponents.size();
	FileIOHelper::BeginDataPackSave(ofs, ChunkKeys::COMPONENTS);
	for (size_t i = 0; i < count; ++i) {
		FileIOHelper::BeginDataPackSave(ofs, mComponents[i]->GetName());
		mComponents[i]->SaveProperties(ofs);
		FileIOHelper::EndDataPackSave(ofs, mComponents[i]->GetName());
	}
	FileIOHelper::EndDataPackSave(ofs, ChunkKeys::COMPONENTS);
}

void Actor::LoadProperties(std::ifstream& ifs)
{
	FileIOHelper::BeginDataPackLoad(ifs, ChunkKeys::ACTOR_PROPERTIES);
	// Changing the call location of Transform is NOT recommended
	// Nested .chunk DataPack has unknown problem.
	mTransform->LoadProperties(ifs);

	std::string parentName;
	FileIOHelper::LoadBasicString(ifs, parentName);
	// <Parent finding feature here!>

	std::string stateStr;
	FileIOHelper::LoadBasicString(ifs, stateStr);
	SetState(stateStr);

	std::string actorGroupStr;
	FileIOHelper::LoadBasicString(ifs, actorGroupStr);
	mActorGroup = ActorGroupUtil::GetActorGroup(actorGroupStr);

	FileIOHelper::LoadInt(ifs, mID);
	FileIOHelper::LoadBasicString(ifs, mName);
}

void Actor::LoadComponents(std::ifstream& ifs)
{
	std::pair<int, std::string>&& pack = FileIOHelper::BeginDataPackLoad(ifs, ChunkKeys::COMPONENTS);
	mComponents.reserve(pack.first);
	for (size_t i = 0; i < pack.first; ++i) {
		std::pair<size_t, std::string> compPack = FileIOHelper::BeginDataPackLoad(ifs);
		ChunkLoader::GetInstance()->GetComponentLoadMap().at(compPack.second)(this, ifs);
	}
}