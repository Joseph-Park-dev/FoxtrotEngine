// ----------------------------------------------------------------
// Foxtrot Engine 2D
// Copyright (C) 2025 JungBae Park. All rights reserved.
//
// Released under the GNU General Public License v3.0
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

#include "Scenes/Scene.h"

#include <algorithm>

#include "Actors/Actor.h"
#include "Actors/ActorGroup.h"
#include "Components/SpriteRenderer.h"
#include "Components/UIs/PanelUI.h"
#include "Managers/EventManager.h"
#include "Core/TemplateFunctions.h"

#include "Actors/Transform.h"
#include "Renderer/FoxtrotRenderer.h"

#ifdef FOXTROT_EDITOR
	#include "EditorLayer.h"
#endif // FOXTROT_EDITOR

Scene::Scene()
	: mIsUpdatingActors(false)
	, mActors(DBG_NEW FTDS::DynamicArray<Actor*>)
	, mPendingActors(DBG_NEW FTDS::DynamicArray<Actor*>)
{
}

Scene::~Scene()
{
	DeleteAll();
	delete mActors;
	delete mPendingActors;
}

Actor* Scene::FindActor(FTDS::String& name, Actor* filter)
{
	for (auto iter = mActors->Begin(); iter != mActors->End(); ++iter)
	{
		if ((*iter)->GetName().Equal(name.C_Str()))
			if ((*iter) != filter)
				return (*iter);
			else
				continue;
		else
			continue;
	}
	return nullptr;
}

Actor* Scene::FindActor(const char* name, Actor* filter)
{
	FTDS::String str(name);
	return FindActor(str, filter);
}

const bool Scene::GetIsUpdatingActors() const
{
	return mIsUpdatingActors;
}

const FTDS::DynamicArray<Actor*>* Scene::GetActors() const
{
	return mActors;
}

FTDS::DynamicArray<Actor*>*& Scene::Actors()
{
	return mActors;
}

void Scene::Initialize(FTCore* coreInst)
{
	for (auto iter = mActors->Begin(); iter != mActors->End(); ++iter)
		(*iter)->Initialize(coreInst);
}

void Scene::Setup()
{
	for (auto iter = mActors->Begin(); iter != mActors->End(); ++iter)
		(*iter)->Setup();
}

void Scene::ProcessInput(FTInputDevice* inputDevice)
{
	for (auto iter = mActors->Begin(); iter != mActors->End(); ++iter)
		if ((*iter)->IsActive())
			(*iter)->ProcessInput(inputDevice);
}

void Scene::Update(float deltaTime)
{
	mIsUpdatingActors = true;
	for (auto iter = mActors->Begin(); iter != mActors->End(); ++iter)
	{
		if ((*iter)->IsActive())
		{
			(*iter)->UpdateComponents(deltaTime);
			(*iter)->UpdateActor(deltaTime);
		}
	}
	mIsUpdatingActors = false;
}

void Scene::LateUpdate(float deltaTime)
{
	for (auto iter = mActors->Begin(); iter != mActors->End(); ++iter)
	{
		if ((*iter)->IsActive())
		{
			(*iter)->LateUpdateComponents(deltaTime);
			(*iter)->LateUpdateActor(deltaTime);
		}
	}
}

void Scene::Render(FoxtrotRenderer* renderer)
{
	for (auto iter = mActors->Begin(); iter != mActors->End(); ++iter)
	{
		if ((*iter)->IsActive())
		{
			(*iter)->RenderComponents(renderer);
			(*iter)->RenderActor(renderer);
		}
	}
}

void Scene::AddActor(Actor* actor)
{
	if (mIsUpdatingActors)
		mPendingActors->PushBack(actor);
	else
	{
		if (!mActors->IsEmpty())
		{
			int	 drawOrder = actor->GetDrawOrder();
			auto iter	   = mActors->Begin();
			for (; iter != mActors->End(); ++iter)
			{
				if(*iter)
					if (drawOrder < (*iter)->GetDrawOrder())
						break;
			}
			mActors->Insert(iter.IterPos(), actor);
		}
		else
			mActors->PushBack(actor);
	}
}

void Scene::ProcessEvent()
{
	AddPendingActors();
	ClearDeadActors();
}

void Scene::DeleteAll()
{
	for (auto iter = mActors->Begin(); iter != mActors->End(); ++iter)
	{
		delete (*iter);
		(*iter) = nullptr;
	}

	for (auto iter = mPendingActors->Begin(); iter != mPendingActors->End(); ++iter)
	{
		delete (*iter);
		(*iter) = nullptr;
	}
	mActors->Clear();
	mPendingActors->Clear();
}

void Scene::AddPendingActors()
{
	for (auto iter = mPendingActors->Begin(); iter != mPendingActors->End(); ++iter)
	{
		mActors->PushBack(*iter);
		mPendingActors->Clear();
	}
}

void Scene::ClearDeadActors()
{
	for (auto iter = mActors->Begin(); iter != mActors->End(); ++iter)
	{
		if ((*iter)->IsDead())
			RemoveActor((*iter));
	}
}

void Scene::RemoveActor(Actor* actor)
{
	int pos = -1;

	pos = mPendingActors->Find(actor);
	if (pos != -1)
	{
		delete mPendingActors->At(pos);
		mPendingActors->Erase(pos);
	}

	pos = mActors->Find(actor);
	if (pos != -1)
	{
		delete mActors->At(pos);
		mActors->Erase(pos);
	}
}