#include "Scene_Start.h"
#include "Ship.h"
#include "BackgroundActor.h"
#include "BGSpriteComponent.h"
#include "TileMapComponent.h"
#include "ResourceManager.h"
#include "Asteroid.h"
#include "CollisionManager.h"
#include "EventFunctions.h"
#include "SceneType.h"
#include "Camera2D.h"
#include "UIActor.h"
#include "PanelUI.h"
#include "ButtonUI.h"
#include "UIManager.h"
#include "MonFactory.h"
#include "Random.h"
#include "GroundObject.h"
#include "SceneManager.h"
#include "Transform.h"
#include "MouseCursor.h"
#include "KeyInputManager.h"

void Scene_Start::Enter()
{
	LoadData();
}

void Scene_Start::LoadData()
{
	ResourceManager::GetInstance()->LoadTexture(L"tex01", L"Assets/Tiles.png");
	ResourceManager::GetInstance()->LoadTexture(L"Ship", L"Assets/ShipTileMap.png");
	ResourceManager::GetInstance()->LoadTexture(L"Enemy01", L"Assets/Asteroid.png");
	ResourceManager::GetInstance()->LoadTexture(L"MouseCursor", L"Assets/TileMapIMGs/MouseCursors_Tilemap.png");
	
	CollisionManager::GetInstance()->MarkGroup(ACTOR_GROUP::PLAYER, ACTOR_GROUP::ENEMY);
	CollisionManager::GetInstance()->MarkGroup(ACTOR_GROUP::PLAYER, ACTOR_GROUP::GROUND);
}

void Scene_Start::Exit()
{
	DeleteAll();
	CollisionManager::GetInstance()->Reset();
}

void Scene_Start::Update(float deltaTime)
{
	Scene::Update(deltaTime);

	if (KEY_TAP(KEY::LSHIFT))
	{
		SceneManager::GetInstance()->SwitchScene(SCENE_TYPE::TOOL);
	}
}