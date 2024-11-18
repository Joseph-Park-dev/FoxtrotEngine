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
	//CollisionManager::GetInstance()->MarkGroup(ActorGroup::PLAYER, ActorGroup::ENEMY);
	//CollisionManager::GetInstance()->MarkGroup(ActorGroup::PLAYER, ActorGroup::GROUND);
	//CollisionManager::GetInstance()->MarkGroup(ActorGroup::DEFAULT, ActorGroup::DEFAULT);


	//FoxtrotRenderer* currRenderer = FTCoreEditor::GetInstance()->GetGameRenderer();
	//std::string asteroidSprite = "Asteroid.png";
	//ResourceManager::GetInstance()->LoadTexture("Asteroid1", asteroidSprite);
	//ResourceManager::GetInstance()->LoadTexture("Asteroid2", asteroidSprite);
	//ResourceManager::GetInstance()->LoadTexture("Asteroid3", asteroidSprite);

	//ResourceManager::GetInstance()->LoadTexture("HenryRunning", "Henry_Running_TEST/Henry_Running_TEST.png");
	//ResourceManager::GetInstance()->LoadTexture("SpriteAnim", "ShipTileMap.png");

	//ResourceManager::GetInstance()->GetLoadedTexture("TEST");

	/*std::string pngPath = "SpineTest/Skel/[Player] ver.01_Spine - File.png";
	ResourceManager::GetInstance()->LoadTexture(currRenderer, );
	std::string atlasPath = "D:/[2024_01]/FoxtrotEngine_DirectX/assets/SpineTest/Skel/[Player] ver.01_Spine-File.atlas";
	std::string skeletonDataPath = "D:/[2024_01]/FoxtrotEngine_DirectX/assets/SpineTest/Skel/skeleton.skel";
	ResourceManager::GetInstance()->LoadSpineAnimation(
		"RunningGrandpa",
		atlasPath,
		skeletonDataPath
	);

	std::string tileMapSprite = "D:/[2024_01]/FoxtrotEngine_DirectX/assets/Tiles.png";
	ResourceManager::GetInstance()->LoadTexture(currRenderer, tileMapSprite);*/

 // _DEBUG
}

void EditorScene::Exit()
{
	//CollisionManager::GetInstance()->Reset();
}

void EditorScene::DeleteAll()
{
	std::vector<Actor*>* actors = GetActors();
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
