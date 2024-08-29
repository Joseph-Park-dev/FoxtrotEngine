#include "FoxtrotEngine/Scenes/Scene.h"

#include <algorithm>

#include "FoxtrotEngine/Actors/Actor.h"
#include "FoxtrotEngine/Actors/ActorGroup.h"
#include "FoxtrotEngine/Components/SpriteRendererComponent.h"
#include "FoxtrotEngine/Managers/EventManager.h"
#include "FoxtrotEngine/Core/TemplateFunctions.h"

#include "FoxtrotEngine/Actors/UIs/PanelUI.h"
#include "FoxtrotEngine/Actors/Transform.h"
#include "FoxtrotEngine/Renderer/FoxtrotRenderer.h"
#ifdef _DEBUG
#include "FoxtrotEditor/EditorLayer.h"
#endif // _DEBUG

Scene::Scene()
	:mIsUpdatingActors(false)
{}

Scene::~Scene()
{
	DeleteAll();
}

void Scene::ProcessInput(KeyInputManager* keyInputManager)
{
	mIsUpdatingActors = true;
	for (size_t i = 0; i < (size_t)ACTOR_GROUP::END; ++i)
	{
		for (size_t j = 0; j < mActors[i].size(); ++j)
		{
			Actor* actor = mActors[i][j];
			if (actor->IsActive())
				actor->ProcessInput(keyInputManager);
		}
	}
	mIsUpdatingActors = false;
}

void Scene::Update(float deltaTime)
{
	mIsUpdatingActors = true;
	for (size_t i = 0; i < (size_t)ACTOR_GROUP::END; ++i)
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
	for (size_t i = 0; i < (size_t)ACTOR_GROUP::END; ++i)
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
	for (size_t i = 0; i < (size_t)ACTOR_GROUP::END; ++i)
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

void Scene::AddActor(Actor* actor, ACTOR_GROUP group)
{
	if (mIsUpdatingActors)
	{
		mPendingActors[(UINT)group].emplace_back(actor);
	}
	else
	{
		mActors[(UINT)group].emplace_back(actor);
	}
}

void Scene::ProcessEvent()
{
	AddPendingActors();
	ClearDeadActors();
	EventManager::GetInstance()->ProcessEvent();
}

void Scene::DeleteAll()
{
	for (size_t i = 0; i < (size_t)ACTOR_GROUP::END; ++i)
	{
		DeleteGroup((ACTOR_GROUP)i);
		DeletePendingGroup((ACTOR_GROUP)i);
	}
}

void Scene::DeleteGroup(ACTOR_GROUP group)
{
	for (Actor* actor : mActors[(UINT)group])
		delete actor;
	mActors[(UINT)group].clear();
}

void Scene::DeletePendingGroup(ACTOR_GROUP group)
{
	for (Actor* pending : mPendingActors[(UINT)group])
		delete pending;
	mPendingActors[(UINT)group].clear();
}

#ifdef _DEBUG
void Scene::EditorUpdate(float deltaTime)
{
	std::vector<EditorElement*>& elements = EditorLayer::GetInstance()->GetEditorElements();
	for (size_t i = 0; i < elements.size(); ++i)
	{
		EditorElement* ele = elements[i];
		if (ele->IsActive())
		{
			// The order of the Update functions should not be revised.
			ele->EditorUpdateComponents(deltaTime);
			ele->EditorUpdateActor();
		}
	}
}

void Scene::EditorLateUpdate(float deltaTime)
{
	std::vector<EditorElement*>& elements = EditorLayer::GetInstance()->GetEditorElements();
	for (size_t i = 0; i < elements.size(); ++i)
	{
		EditorElement* ele = elements[i];
		if (ele->IsActive())
		{
			ele->EditorLateUpdateComponents(deltaTime);
			ele->EditorLateUpdateActor();
		}
	}
}
void Scene::EditorRender(FoxtrotRenderer* renderer)
{
	std::vector<EditorElement*>& elements = EditorLayer::GetInstance()->GetEditorElements();
	for (size_t i = 0; i < elements.size(); ++i)
	{
		EditorElement* ele = elements[i];
		if (ele->IsActive())
		{
			ele->Render(renderer);
		}
	}
}
#endif // _DEBUG

void Scene::AddPendingActors()
{
	for (size_t i = 0; i < (size_t)ACTOR_GROUP::END; ++i)
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
	for (size_t i = 0; i < (size_t)ACTOR_GROUP::END; ++i)
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
	for (size_t i = 0; i < (size_t)ACTOR_GROUP::END; ++i)
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