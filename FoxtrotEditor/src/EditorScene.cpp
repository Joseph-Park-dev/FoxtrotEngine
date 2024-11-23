#include "EditorScene.h"

#include "Managers/ResourceManager.h"
#include "Managers/SceneManager.h"
#include "Renderer/FoxtrotRenderer.h"
#include "Components/SpriteRendererComponent.h"
#include "Components/TileMapComponent.h"
#include "ResourceSystem/GeometryGenerator.h"

#include "FTCoreEditor.h"
#include "EditorLayer.h"
#include "EditorElement.h"

void EditorScene::Enter()
{
	LoadData();
}

void EditorScene::LoadData()
{
}

void EditorScene::Exit()
{
	//CollisionManager::GetInstance()->Reset();
}

void EditorScene::DeleteAll()
{
;	std::vector<Actor*>* actors = GetActors();
	UnfocusEditorElements();

	for (size_t i = 0; i < (size_t)ActorGroup::END; ++i)
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
	EditorElement* editorElement = new EditorElement(this);
	std::string name = "Game Object " + std::to_string(this->GetActorCount());
	editorElement->SetName(name);
	editorElement->SetIsFocused(true);
}

void EditorScene::AddEditorElement(EditorElement* copied)
{
	EditorElement* editorElement = new EditorElement(copied, this);
	std::string name = copied->GetName() + " copy " + std::to_string(this->GetActorCount());
	editorElement->SetName(name);
}

void EditorScene::AddEditorElement(Actor* actor)
{
	UnfocusEditorElements();
	EditorElement* editorElement = new EditorElement(actor, this);
}

void EditorScene::EditorUpdate(float deltaTime)
{
	mIsUpdatingActors = true;
	std::vector<Actor*>* actors = GetActors();
	for (size_t i = 0; i < (size_t)ActorGroup::END; ++i)
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
	for (size_t i = 0; i < (size_t)ActorGroup::END; ++i)
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
