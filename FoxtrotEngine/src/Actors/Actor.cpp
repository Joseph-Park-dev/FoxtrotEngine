#include "Actors/Actor.h"

#include <algorithm>
#include <functional>
#include <nlohmann/json.hpp>

#include "Scenes/Scene.h"
#include "Managers/KeyInputManager.h"

#include "Actors/ActorGroup.h"
#include "Actors/ActorTag.h"
#include "Actors/Transform.h"
#include "FileSystem/ChunkLoader.h"
#include "FileSystem/ChunkFileKeys.h"
#include "Components/Component.h"

int Actor::g_NextID = 0;

Actor::Actor(Scene* scene)
	: mState(EActive)
	, mTransform(new Transform)
	, mActorGroup(ActorGroup::DEFAULT)
	, mParent(nullptr)
	, mID(g_NextID++)
	, mComponents{}
	, mChild{}
{}

// Constructor to copy Actor
Actor::Actor(Actor& origin)
	: mState(origin.mState)
	, mTransform(nullptr)
	, mActorGroup(origin.mActorGroup)
	, mParent(nullptr)
	, mID(g_NextID++)
	, mComponents{}
	, mChild{}
{ 
	CopyTransformFrom(origin);
	CopyComponentsFrom(origin);
}

// Constructor to copy Actor
Actor::Actor(Actor* origin)
	: mState(origin->mState)
	, mTransform(nullptr)
	, mActorGroup(origin->mActorGroup)
	, mParent(nullptr)
	, mID(g_NextID++)
	, mComponents{}
	, mChild{}
{
	CopyTransformFrom(origin);
	CopyComponentsFrom(origin);
}

Actor::~Actor()
{
	if(mTransform)
		delete mTransform;
	for (size_t i = 0; i < mComponents.size(); ++i)
		delete mComponents[i];
	mComponents.clear();
	mChild.clear();
}

void Actor::CopyTransformFrom(Actor& origin)
{
	Transform* originTransf = origin.GetTransform();
	Transform* copied = new Transform;
	copied->SetWorldPosition	(originTransf->GetWorldPosition());
	copied->SetLocalPosition	(originTransf->GetLocalPosition());
	//copied->SetScreenPosition	(originTransf->GetScreenPosition());
	copied->SetScale			(originTransf->GetScale());
	copied->SetRotation			(originTransf->GetRotation());
	this->SetTransform(copied);
}

void Actor::CopyComponentsFrom(Actor& origin)
{
	std::vector<Component*>& components = origin.GetComponents();
	for (size_t i = 0; i < components.size(); ++i)
	{
		components[i]->CloneTo(this);
		delete components[i];
	}
	components.clear();
}

void Actor::CopyTransformFrom(Actor* origin)
{
	Transform* originTransf = origin->GetTransform();
	Transform* copied = new Transform;
	copied->SetWorldPosition(originTransf->GetWorldPosition());
	copied->SetLocalPosition(originTransf->GetLocalPosition());
	//copied->SetScreenPosition(originTransf->GetScreenPosition());
	copied->SetScale(originTransf->GetScale());
	copied->SetRotation(originTransf->GetRotation());
	this->SetTransform(copied);
}

void Actor::CopyComponentsFrom(Actor* origin)
{
	std::vector<Component*>& components = origin->GetComponents();
	for (size_t i = 0; i < components.size(); ++i)
	{
		components[i]->CloneTo(this);
		delete components[i];
	}
	components.clear();
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

void Actor::LoadProperties(std::ifstream& ifs)
{
}

void Actor::LoadComponents(std::ifstream& ifs)
{
	size_t count = FileIOHelper::LoadSize(ifs);
	for (size_t i = 0; i < count; ++i)
	{
		std::wstring compName = FileIOHelper::LoadWString(ifs);
		//ChunkLoader::GetInstance()->GetCompLoadMap()[compName](this, ifs);
	}
}

void Actor::LoadProperties(nlohmann::ordered_json& in)
{
	mName.assign(FileIOHelper::LoadScalarValue<std::string>(in, "Name"));
	mID = FileIOHelper::LoadScalarValue<int>(in, "ID");

	std::string actorGroupStr = FileIOHelper::LoadScalarValue<std::string>(in, "ActorGroup");
	mActorGroup = ActorGroupUtil::GetActorGroup(actorGroupStr);

	std::string stateStr = FileIOHelper::LoadScalarValue<std::string>(in, "State");
	State state = State::EActive;
	if (stateStr == "paused")
		state = State::EPaused;
	else if (stateStr == "dead")
		state = State::EDead;
	mState = state;

}

void Actor::LoadComponents(nlohmann::ordered_json& in)
{
	size_t count = FileIOHelper::LoadScalarValue<size_t>(in, "Count");
	for (size_t i = 0; i < count; ++i)
	{
		//std::string compName = 
		//ChunkLoader::GetInstance()->GetCompLoadMap()[compName](this, ifs);
	}
}

void Actor::SaveProperties(std::ofstream& ofs)
{
	FileIOHelper::BeginDataPackSave	(ofs, ChunkKeys::ACTOR_PROPERTIES, 5);
	FileIOHelper::SaveString		(ofs, ChunkKeys::NAME, GetName());
	FileIOHelper::SaveInt			(ofs, ChunkKeys::ID, mID);
	FileIOHelper::SaveString		(ofs, ChunkKeys::ACTOR_GROUP, ActorGroupUtil::GetActorGroupStr(mActorGroup));
	FileIOHelper::SaveString		(ofs, ChunkKeys::STATE, GetStateStr());
	if (mParent)
		FileIOHelper::SaveString	(ofs, ChunkKeys::PARENT, mParent->GetName());
	else
		FileIOHelper::SaveString	(ofs, ChunkKeys::PARENT, "nullptr");
}

void Actor::SaveComponents(std::ofstream& ofs)
{
	size_t count = mComponents.size();
	FileIOHelper::BeginDataPackSave(ofs, ChunkKeys::ACTOR_PROPERTIES, 5);
	FileIOHelper::SaveInt(ofs, count);
	for (size_t i = 0; i < count; ++i)
	{
		mComponents[i]->SaveProperties(out["List"][i]);
	}
}

#ifdef FOXTROT_EDITOR
void Actor::SaveProperties(nlohmann::ordered_json& out)
{
	FileIOHelper::AddScalarValue(out["Name"], GetName());
	FileIOHelper::AddScalarValue(out["ID"], mID);
	FileIOHelper::AddScalarValue(out["ActorGroup"], ActorGroupUtil::GetActorGroupStr(mActorGroup));
	FileIOHelper::AddScalarValue(out["State"], GetStateStr());
	if (mParent)
		FileIOHelper::AddScalarValue(out["Parent"], mParent->GetName());
	else
		FileIOHelper::AddScalarValue(out["Parent"], "nullptr");

	mTransform->SaveProperties(out["Transform"]);
	
	// FileIOHelper::AddValue<std::string>("Child", GetStateStr());
}

void Actor::SaveComponents(nlohmann::ordered_json& out)
{
	size_t count = mComponents.size();
	FileIOHelper::AddScalarValue(out["Count"], count);
	for (size_t i = 0; i < count; ++i)
	{
		mComponents[i]->SaveProperties(out["List"][i]);
	}
}
#endif // FOXTROT_EDITOR