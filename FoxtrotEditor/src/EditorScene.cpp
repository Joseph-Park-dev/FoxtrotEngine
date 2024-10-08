#include "EditorScene.h"

#include "Managers/CollisionManager.h"
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
	CollisionManager::GetInstance()->MarkGroup(ActorGroup::PLAYER, ActorGroup::ENEMY);
	CollisionManager::GetInstance()->MarkGroup(ActorGroup::PLAYER, ActorGroup::GROUND);
	CollisionManager::GetInstance()->MarkGroup(ActorGroup::DEFAULT, ActorGroup::DEFAULT);


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
	CollisionManager::GetInstance()->Reset();
}

void EditorScene::DeleteAll()
{
	std::vector<Actor*>* actors = GetActors();

	for (size_t i = 0; i < (size_t)ActorGroup::END; ++i)
	{
		for (size_t j = 0; j < actors[i].size(); ++j)
		{
			UnfocusEditorElements();
			EditorElement* ele = (EditorElement*)actors[i][j];
			delete ele;
		}
		actors[i].clear();
	}
}

void EditorScene::ProcessInput(KeyInputManager* keyInputManager)
{
	for (size_t i = 0; i < (size_t)ActorGroup::END; ++i)
	{
		size_t groupSize = GetActorGroup(i).size();
		for (size_t j = 0; j < groupSize; ++j)
		{
			EditorElement* ele = (EditorElement*)GetActorGroup(i)[j];
			if (ele->IsActive())
				ele->ProcessInput(keyInputManager);
		}
	}
}

void EditorScene::Update(float deltaTime)
{
	SetIsUpdatingActors(true);
	for (size_t i = 0; i < (size_t)ActorGroup::END; ++i){
		size_t groupSize = GetActorGroup(i).size();
		for (size_t j = 0; j < groupSize; ++j){
			EditorElement* ele = (EditorElement*)GetActorGroup(i)[j];
			if (ele->IsActive()) {
				ele->EditorUpdateComponents(deltaTime);
				ele->EditorUpdateActor();
			}
		}
	}
	SetIsUpdatingActors(false);
}

void EditorScene::LateUpdate(float deltaTime)
{
	for (size_t i = 0; i < (size_t)ActorGroup::END; ++i) {
		size_t groupSize = GetActorGroup(i).size();
		for (size_t j = 0; j < groupSize; ++j) {
			EditorElement* ele = (EditorElement*)GetActorGroup(i)[j];
			if (ele->IsActive()) {
				ele->EditorLateUpdateComponents(deltaTime);
				ele->EditorLateUpdateActor();
			}
		}
	}
}
void EditorScene::Render(FoxtrotRenderer* renderer)
{
	for (size_t i = 0; i < (size_t)ActorGroup::END; ++i) {
		size_t groupSize = GetActorGroup(i).size();
		for (size_t j = 0; j < groupSize; ++j) {
			EditorElement* ele = (EditorElement*)GetActorGroup(i)[j];
			if (ele->IsActive()) {
				ele->Render(renderer);
			}
		}
	}
}

void EditorScene::UnfocusEditorElements()
{
	size_t rowSize = GetArrayLength(GetActors());
	for (size_t i = 0; i < rowSize; ++i) {
		std::vector<Actor*>* actors = GetActors();
		auto iter = actors->begin();
		for (; iter != GetActors()[i].end(); ++iter) {
			EditorElement* ele = (EditorElement*)(*iter);
			if (ele->GetIsFocused())
				ele->SetIsFocused(false);
		}
	}
}

void EditorScene::AddEditorElement()
{
	UnfocusEditorElements();
	EditorElement* editorElement = new EditorElement(this);
	editorElement->SetName(L"Game Object" + std::to_wstring(this->GetActorCount()));
	editorElement->SetIsFocused(true);
	AddActor(editorElement, ActorGroup::DEFAULT);
}

void EditorScene::AddEditorElement(Actor* actor)
{
	UnfocusEditorElements();
	EditorElement* editorElement = new EditorElement(actor);
	AddActor(editorElement, actor->GetActorGroup());
}

EditorScene::EditorScene()
	: Scene()
{
}
