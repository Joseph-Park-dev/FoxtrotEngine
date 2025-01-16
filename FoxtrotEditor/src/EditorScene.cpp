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

void EditorScene::DeleteAll()
{
;	std::vector<Actor*>* actors = GetActors();
	UnfocusEditorElements();

	for (size_t i = 0; i < ActorGroupUtil::GetCount(); ++i)
	{
		for (size_t j = 0; j < actors[i].size(); ++j)
		{
			EditorElement* ele = dynamic_cast<EditorElement*>(actors[i][j]);
			delete ele;
		}
		actors[i].clear();
	}
	actors->clear();
	Scene::DeleteAll();
}

void EditorScene::UnfocusEditorElements()
{
	std::vector<Actor*>* actors = GetActors();
	size_t rowSize = GetArrayLength(actors);
	for (size_t i = 0; i < rowSize; ++i) {
		auto iter = actors->begin();
		for (; iter != GetActors()[i].end(); ++iter) {
			EditorElement* ele = dynamic_cast<EditorElement*>(*iter);
			if (ele->GetIsFocused())
				ele->SetIsFocused(false);
		}
	}
}

void EditorScene::AddEditorElement()
{
	UnfocusEditorElements();
	EditorElement* editorElement = DBG_NEW EditorElement(this);
	std::string name = "Game Object " + std::to_string(this->GetActorCount());
	editorElement->SetName(name);
	editorElement->SetIsFocused(true);
}

void EditorScene::AddEditorElement(Actor* actor)
{
	UnfocusEditorElements();
	//// Finds the actor with the same name and adds numbered suffix.
	//std::string name = actor->GetName();
	//std::vector<Actor*>* actors = GetActors();
	//int count = 0;
	//for (size_t i = 0; i < ActorGroupUtil::GetCount(); ++i)
	//{
	//	for (size_t j = 0; j < actors[i].size(); ++j)
	//	{
	//		std::string buf = actors[i][j]->GetName();
	//		name = ExtractUntil(name, '(');
	//		buf = ExtractUntil(buf, '(');
	//		if (name == buf)
	//			++count;
	//	}
	//}
	//if (0 < count)
	//	actor->SetName(actor->GetName() + "(" + std::to_string(count) + ")");
	this->AddActor(actor, actor->GetActorGroup());
}

void EditorScene::EditorUpdate(float deltaTime)
{
	mIsUpdatingActors = true;
	std::vector<Actor*>* actors = GetActors();
	for (size_t i = 0; i < ActorGroupUtil::GetCount(); ++i)
	{
		for (size_t j = 0; j < actors[i].size(); ++j)
		{
			EditorElement* ele = dynamic_cast<EditorElement*>(actors[i][j]);
			ele->EditorUpdate(deltaTime);
		}
	}
	mIsUpdatingActors = false;
}

void EditorScene::EditorRender(FoxtrotRenderer* renderer)
{
	mIsUpdatingActors = true;
	std::vector<Actor*>* actors = GetActors();
	for (size_t i = 0; i < ActorGroupUtil::GetCount(); ++i)
	{
		for (size_t j = 0; j < actors[i].size(); ++j)
		{
			EditorElement* ele = dynamic_cast<EditorElement*>(actors[i][j]);
			ele->EditorRender(renderer);
		}
	}
	mIsUpdatingActors = false;
}

EditorScene::EditorScene()
	: Scene()
	, mIsUpdatingActors(false)
{
}