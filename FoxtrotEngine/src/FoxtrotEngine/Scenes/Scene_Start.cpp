#include "FoxtrotEngine/Scenes/Scene_Start.h"

#include "FoxtrotEngine/Managers/CollisionManager.h"
#include "FoxtrotEngine/Managers/ResourceManager.h"
#include "FoxtrotEngine/Renderer/FoxtrotRenderer.h"
#include "FoxtrotEditor/FTCoreEditor.h"

void Scene_Start::Enter()
{
	LoadData();
}

void Scene_Start::LoadData()
{
	CollisionManager::GetInstance()->MarkGroup(ACTOR_GROUP::PLAYER, ACTOR_GROUP::ENEMY);
	CollisionManager::GetInstance()->MarkGroup(ACTOR_GROUP::PLAYER, ACTOR_GROUP::GROUND);
	CollisionManager::GetInstance()->MarkGroup(ACTOR_GROUP::DEFAULT, ACTOR_GROUP::DEFAULT);

#ifdef _DEBUG
	FoxtrotRenderer* currRenderer = FTCoreEditor::GetInstance()->GetGameRenderer();
	std::string asteroidSprite = "D:/[2024_01]/FoxtrotEngine_DirectX/assets/Asteroid.png";
	ResourceManager::GetInstance()->LoadTexture(currRenderer, asteroidSprite);
#endif // _DEBUG
}

void Scene_Start::Exit()
{
	DeleteAll();
	CollisionManager::GetInstance()->Reset();
}

void Scene_Start::Update(float deltaTime)
{
	Scene::Update(deltaTime);

	/*if (KEY_TAP(KEY::LSHIFT))
	{
		SceneManager::GetInstance()->SwitchScene(SCENE_TYPE::TOOL);
	}*/
}