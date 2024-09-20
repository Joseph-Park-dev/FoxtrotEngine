#include "FoxtrotEngine/Scenes/Scene_Start.h"

#include "FoxtrotEngine/Managers/CollisionManager.h"
#include "FoxtrotEngine/Managers/ResourceManager.h"
#include "FoxtrotEngine/Managers/SceneManager.h"
#include "FoxtrotEngine/Renderer/FoxtrotRenderer.h"
#include "FoxtrotEngine/Components/SpriteRendererComponent.h"
#include "FoxtrotEngine/Components/TileMapComponent.h"
#include "FoxtrotEngine/ResourceSystem/GeometryGenerator.h"
#include "FoxtrotEditor/FTCoreEditor.h"
#include "FoxtrotEditor/EditorLayer.h"

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
	//std::string asteroidSprite = "Asteroid.png";
	//ResourceManager::GetInstance()->LoadTexture("Asteroid1", asteroidSprite);
	//ResourceManager::GetInstance()->LoadTexture("Asteroid2", asteroidSprite);
	//ResourceManager::GetInstance()->LoadTexture("Asteroid3", asteroidSprite);
	ResourceManager::GetInstance()->LoadBasicMesh(MAPKEY_SPRITE_MESH, GeometryGenerator::MakeSquare(1.0f));

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