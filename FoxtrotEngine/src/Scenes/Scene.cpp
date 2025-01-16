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
	:mIsUpdatingActors(false)
{}

Scene::~Scene()
{
	DeleteAll();
}

Actor* Scene::FindActor(std::string& name)
{
	for (size_t i = 0; i < (size_t)ActorGroup::END; ++i)
	{
		std::vector<Actor*>& actors = GetActorGroup(i);
		auto func = [&](Actor* actor) { return actor->HasName(name); };
		auto iter = std::find_if(actors.begin(), actors.end(), func);
		if (iter != actors.end())
			return *iter;
		else
			continue;
	}
}

Actor* Scene::FindActor(const char* name)
{
	for (size_t i = 0; i < (size_t)ActorGroup::END; ++i)
	{
		std::vector<Actor*>& actors = GetActorGroup(i);
		auto func = [&](Actor* actor) { return actor->HasName(name); };
		auto iter = std::find_if(actors.begin(), actors.end(), func);
		if (iter != actors.end())
			return *iter;
		else
			continue;
	}
}

void Scene::Setup()
{
	for (size_t i = 0; i < ActorGroupUtil::GetCount(); ++i)
	{
		for (size_t j = 0; j < mActors[i].size(); ++j)
		{
			Actor* actor = mActors[i][j];
			if (actor->IsActive())
				actor->Setup();
		}
	}
}

void Scene::ProcessInput(KeyInputManager* keyInputManager)
{
	for (size_t i = 0; i < ActorGroupUtil::GetCount(); ++i)
	{
		for (size_t j = 0; j < mActors[i].size(); ++j)
		{
			Actor* actor = mActors[i][j];
			if (actor->IsActive())
				actor->ProcessInput(keyInputManager);
		}
	}
}

void Scene::Update(float deltaTime)
{
	mIsUpdatingActors = true;
	for (size_t i = 0; i < ActorGroupUtil::GetCount(); ++i)
	{
		for (size_t j = 0; j < mActors[i].size(); ++j)
		{
			Actor* actor = mActors[i][j];
			if (actor->IsActive())
			{
				// The order of the Update functions should not be revised.
				actor->UpdateComponents(deltaTime);
				actor->UpdateActor(deltaTime);
			}
		}
	}
	mIsUpdatingActors = false;
}

void Scene::LateUpdate(float deltaTime)
{
	for (size_t i = 0; i < ActorGroupUtil::GetCount(); ++i)
	{
		for (size_t j = 0; j < mActors[i].size(); ++j)
		{
			Actor* actor = mActors[i][j];
			if (actor->IsActive())
			{
				actor->LateUpdateComponents(deltaTime);
				actor->LateUpdateActor(deltaTime);
			}
		}
	}
}

void Scene::Render(FoxtrotRenderer* renderer)
{
	for (size_t i = 0; i < ActorGroupUtil::GetCount(); ++i)
	{
		for (size_t j = 0; j < mActors[i].size(); ++j)
		{
			Actor* actor = mActors[i][j];
			if (actor->IsActive())
			{
				actor->RenderComponents(renderer);
				actor->RenderActor(renderer);
			}
		}
	}
}

void Scene::AddActor(Actor* actor, ActorGroup group)
{
	if (mIsUpdatingActors)
		mPendingActors[(UINT)group].emplace_back(actor);
	else
		mActors[(UINT)group].emplace_back(actor);
}

void Scene::ProcessEvent()
{
	AddPendingActors();
	ClearDeadActors();
}

void Scene::DeleteAll()
{
	for (size_t i = 0; i < ActorGroupUtil::GetCount(); ++i)
	{
		DeleteGroup((ActorGroup)i);
		DeletePendingGroup((ActorGroup)i);
	}
}

void Scene::DeleteGroup(ActorGroup group)
{
	for (Actor* actor : mActors[(UINT)group])
		delete actor;
	mActors[(UINT)group].clear();
}

void Scene::DeletePendingGroup(ActorGroup group)
{
	for (Actor* pending : mPendingActors[(UINT)group])
		delete pending;
	mPendingActors[(UINT)group].clear();
}

void Scene::AddPendingActors()
{
	for (size_t i = 0; i < ActorGroupUtil::GetCount(); ++i)
	{
		for (size_t j = 0; j < mActors[i].size(); ++j)
		{
			for (auto pending : mPendingActors[i])
			{
				mActors[i].emplace_back(pending);
			}
			mPendingActors[i].clear();
		}
	}
}

void Scene::ClearDeadActors()
{
	for (size_t i = 0; i < ActorGroupUtil::GetCount(); ++i)
	{
		for (size_t j = 0; j < mActors[i].size(); ++j)
		{
			if (mActors[i][j]->IsDead())
			{
				RemoveActor(mActors[i][j]);
			}
		}
	}
}

void Scene::RemoveActor(Actor* actor)
{
	for (size_t i = 0; i < ActorGroupUtil::GetCount(); ++i)
	{
		auto iter = std::find(mPendingActors[i].begin(), mPendingActors[i].end(), actor);
		if (iter != mPendingActors[i].end())
		{
			std::iter_swap(iter, mPendingActors[i].end() - 1);
			mPendingActors[i].pop_back();
		}

		iter = std::find(mActors[i].begin(), mActors[i].end(), actor);
		if (iter != mActors[i].end())
		{
			std::iter_swap(iter, mActors[i].end() - 1);
			mActors[i].pop_back();
		}
	}
}