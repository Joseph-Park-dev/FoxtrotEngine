// ----------------------------------------------------------------
// Foxtrot Engine 2D
// Copyright (C) 2025 JungBae Park. All rights reserved.
//
// Released under the GNU General Public License v3.0
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

#include "EditorScene.h"

#include "Managers/ResourceManager.h"
#include "Managers/SceneManager.h"
#include "Renderer/FoxtrotRenderer.h"
#include "Components/SpriteRenderer.h"
#include "Components/TileMapRenderer.h"
#include "ResourceSystem/GeometryGenerator.h"

#include "FTCoreEditor.h"
#include "EditorLayer.h"
#include "EditorElement.h"
#include "ActorCommand.h"

void EditorScene::Initialize(FTCore* coreInst)
{
	for (EditorElement* ele : mEditorElements)
		ele->Initialize(coreInst);
}

void EditorScene::Setup()
{
	for (EditorElement* ele : mEditorElements)
		if (ele->IsActive())
			ele->Setup();
}

void EditorScene::ProcessInput(FTInputDevice* inputDevice)
{
	for (EditorElement* ele : mEditorElements)
		if (ele->IsActive())
			ele->ProcessInput(inputDevice);
}

void EditorScene::Update(float deltaTime)
{
	for (EditorElement* ele : mEditorElements)
	{
		if (ele->IsActive())
		{
			// The order of the Update functions should not be revised.
			ele->UpdateComponents(deltaTime);
			ele->UpdateActor(deltaTime);
		}
	}
}

void EditorScene::LateUpdate(float deltaTime)
{
	for (EditorElement* ele : mEditorElements)
	{
		if (ele->IsActive())
		{
			// The order of the Update functions should not be revised.
			ele->LateUpdateComponents(deltaTime);
			ele->LateUpdateActor(deltaTime);
		}
	}
}

void EditorScene::Render(FoxtrotRenderer* renderer)
{
	for (EditorElement* ele : mEditorElements)
	{
		if (ele->IsActive())
		{
			ele->RenderComponents(renderer);
			ele->RenderActor(renderer);
		}
	}
}

void EditorScene::DeleteAll()
{
	UnfocusEditorElements();

	for (EditorElement* ele : mEditorElements)
	{
		delete ele;
		ele = nullptr;
	}
	mEditorElements.clear();
}

void EditorScene::UnfocusEditorElements()
{
	for (EditorElement* ele : mEditorElements)
	{
		if (ele->GetIsFocused())
			ele->SetIsFocused(false);
	}
}

void EditorScene::AddEditorElement()
{
	UnfocusEditorElements();
	EditorElement* editorElement = DBG_NEW EditorElement();

	FTDS::String& name = editorElement->GetNameRef();
	name.Append(std::to_string(mEditorElements.size()).c_str());

	editorElement->SetIsFocused(true);

	mEditorElements.emplace_back(editorElement);
}

void EditorScene::AddEditorElement(Actor* actor)
{
	UnfocusEditorElements();

	EditorElement* element = DBG_NEW EditorElement(actor);
	int	 drawOrder = element->GetDrawOrder();
	auto iter	   = mEditorElements.begin();
	for (; iter != mEditorElements.end(); ++iter)
	{
		if (drawOrder < (*iter)->GetDrawOrder())
			break;
	}
	mEditorElements.insert(iter, element);
}

EditorElement* EditorScene::FindEditorElement(FTDS::String& name, Actor* filter)
{
	auto func = [&](EditorElement* actor) {
		if (filter)
			return actor->HasName(name) && actor != filter;
		else
			return actor->HasName(name);
	};

	auto iter = std::find_if(mEditorElements.begin(), mEditorElements.end(), func);
	if (iter != mEditorElements.end())
		return *iter;
	return nullptr;
}

EditorElement* EditorScene::FindEditorElement(const char* name, Actor* filter)
{
	FTDS::String str(name);
	return FindEditorElement(str, filter);
}

std::vector<EditorElement*>& EditorScene::GetEditorElements()
{
	return mEditorElements;
}

void EditorScene::EditorUpdate(float deltaTime)
{
	mIsUpdatingActors = true;
	for (EditorElement* ele : mEditorElements)
		ele->EditorUpdate(deltaTime);
	mIsUpdatingActors = false;
}

void EditorScene::EditorRender(FoxtrotRenderer* renderer)
{
	mIsUpdatingActors = true;
	for (EditorElement* ele : mEditorElements)
		ele->EditorRender(renderer);
	mIsUpdatingActors = false;
}

EditorScene::EditorScene()
	: mIsUpdatingActors(false)
	, mEditorElements()
{
}

EditorScene::~EditorScene()
{
	DeleteAll();
}