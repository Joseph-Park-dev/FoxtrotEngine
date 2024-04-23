#include <algorithm>
#include <functional>

#include "Actor.h"
#include "CCore.h"
#include "Scene.h"
#include "KeyInputManager.h"
#include "Camera2D.h"
#include "ActorGroup.h"
#include "ActorTag.h"
#include "Transform.h"
#include "ChunkLoader.h"
#include "UIActor.h"

int Actor::g_NextID = 0;

Actor::Actor(Scene* scene)
	: mState(EActive)
	, mTransform(new Transform)
	, mActorGroup(ACTOR_GROUP::DEFAULT)
	, mParent(nullptr)
	, mID(g_NextID++)
	, mComponents{}
	, mChild{}
{}

// Constructor to copy Actor
Actor::Actor(const Actor& origin)
	: mState(origin.mState)
	, mTransform(new Transform)
	, mActorGroup(origin.mActorGroup)
	, mParent(nullptr)
	, mID(g_NextID++)
	, mComponents{}
	, mChild{}
{ 
	for (size_t i = 0; i < origin.mComponents.size(); ++i)
		this->mComponents.emplace_back(origin.mComponents[i]->Clone());
}

// Constructor to copy Actor
Actor::Actor(const Actor* origin)
	: mState(origin->mState)
	, mTransform(new Transform)
	, mActorGroup(origin->mActorGroup)
	, mParent(nullptr)
	, mID(g_NextID++)
	, mComponents{}
	, mChild{}
{
	for (size_t i = 0; i < origin->mComponents.size(); ++i)
		this->mComponents.emplace_back(origin->mComponents[i]->Clone());
}

Actor::~Actor()
{
	delete mTransform;
	mComponents.clear();
	mChild.clear();
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
		LogInt(mComponents.size());
	}
}

void Actor::OnCollisionEnter(ColliderComponent* _pOther)
{
}

void Actor::OnCollisionStay(ColliderComponent* _pOther)
{
}

void Actor::OnCollisionExit(ColliderComponent* _pOther)
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
	FileIOHelper::AddWString	 (ofs, mName);
	FileIOHelper::AddInt		 (ofs, mID);
	FileIOHelper::AddBasicString (ofs, GetActorGroupStr(GetActorGroup()));
	FileIOHelper::AddBasicString (ofs, GetStateStr());

	FileIOHelper::AddVector2 (ofs, mTransform->GetWorldPosition());
	FileIOHelper::AddVector2 (ofs, mTransform->GetLocalPosition());
	FileIOHelper::AddFloat	 (ofs, Math::ToDegrees(mTransform->GetRotation()));
	FileIOHelper::AddVector2 (ofs, mTransform->GetScale());
}

void Actor::SaveComponents(std::ofstream& ofs)
{
	size_t count = mComponents.size();
	FileIOHelper::AddSize(ofs, count);
	for (size_t i = 0; i < count; ++i)
	{
		FileIOHelper::AddWString(ofs, mComponents[i]->GetName());
		mComponents[i]->SaveProperties(ofs);
	}
}

void Actor::LoadProperties(std::ifstream& ifs)
{
	FileIOHelper::LoadWString(ifs, mName);
	FileIOHelper::LoadInt(ifs, mID);
	std::string stringBuffer;
	FileIOHelper::LoadBasicString(ifs, stringBuffer);
	mActorGroup = ActorGroupMap[stringBuffer];
	FileIOHelper::LoadBasicString(ifs, stringBuffer);
	SetState(stringBuffer);

	mTransform->SetWorldPosition(FileIOHelper::LoadVector2(ifs));
	mTransform->SetLocalPosition(FileIOHelper::LoadVector2(ifs));
	mTransform->SetRotation		(FileIOHelper::LoadFloat(ifs));
	mTransform->SetScale		(FileIOHelper::LoadVector2(ifs));
}

void Actor::LoadComponents(std::ifstream& ifs)
{
	size_t count = FileIOHelper::LoadSize(ifs);
	for (size_t i = 0; i < count; ++i)
	{
		std::wstring compName = FileIOHelper::LoadWString(ifs);
		ChunkLoader::GetInstance()->GetCompLoadMap()[compName](this, ifs);
	}
}