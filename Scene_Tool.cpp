#include "Scene_Tool.h"
#include "KeyInputManager.h"
#include "SceneManager.h"
#include "Ship.h"
#include "EventFunctions.h"
#include "BackgroundActor.h"
#include "TileMapComponent.h"
#include "ResourceManager.h"
#include "Asteroid.h"
#include "CollisionManager.h"
#include "Camera2D.h"
#include "PanelUI.h"
#include "ButtonUI.h"
#include <Windows.h>
#include "MouseCursor.h"
#include "Ship.h"
#include "Transform.h"

void Scene_Tool::Enter()
{
	LoadData();
}

void Scene_Tool::Exit()
{
	DeleteAll();
	CollisionManager::GetInstance()->Reset();
}

void Scene_Tool::LoadData()
{
	ResourceManager::GetInstance()->LoadTexture(L"MouseCursor", L"Assets/TileMapIMGs/MouseCursors_Tilemap.png");
	MouseCursor* mouseCursor = new MouseCursor(this);
	Instantiate(mouseCursor, ACTOR_GROUP::UI);

	/*Ship* mShip = new Ship(this);
	mShip->GetTransform()->SetWorldPosition(Vector2(100.0f, 384.0f));
	mShip->GetTransform()->SetScale(Vector2(1.5f, 1.5f));
	Instantiate(mShip, ACTOR_GROUP::PLAYER);
	Camera2D::GetInstance()->SetTarget(mShip);*/
	////bg->SetScreenSize(Vector2(1024.f, 768.f));
	//bg->SetBGTextures(bgTexs);
	//bg->SetScrollSpeed(-100.0f);

	//bg = new BGSpriteComponent(temp, 50);
	//bg->SetScreenSize(Vector2(1024.0f, 768.0f));
	//bgTexs = {
	//	temp->LoadTexture("Assets/Stars.png"),
	//	temp->LoadTexture("Assets/Stars.png")
	//};
	//bg->SetBGTextures(bgTexs);
	//bg->SetScrollSpeed(-200.0f);
	//FTTexture* bgTexs = nullptr;
	//bgTexs = {
	//	ResourceManager::GetInstance()->GetLoadedTexture(L"tex01")
	//};
	//TileMapComponent* tiled1 = new TileMapComponent(temp, 52);
	//tiled1->SetTexture(bgTexs);
	//tiled1->ReadCSV(L"Assets/MapLayer1.csv", 32, 32);

	//TileMapComponent* tiled2 = new TileMapComponent(temp, 51);
	//tiled2->SetTexture(bgTexs);
	//tiled2->ReadCSV(L"Assets/MapLayer2.csv", 32, 32);

	//TileMapComponent* tiled3 = new TileMapComponent(temp, 50);
	//tiled3->SetTexture(bgTexs);
	//tiled3->ReadCSV(L"Assets/MapLayer3.csv", 32, 32);
	//Instantiate(temp, ACTOR_GROUP::DEFAULT);

	//for (int i = 0; i < 1; i++)
	//{
	//	//Instantiate(new Asteroid(this), ACTOR_GROUP::ENEMY);
	//}

	PanelUI* panelUI = new PanelUI(this, false);
	panelUI->GetTransform()->SetScale(Vector2(300.f, 300.f));
	panelUI->GetTransform()->SetWorldPosition(Vector2(450, 450));
	panelUI->SetName(L"PanelUI");

	PanelUI* panelUI2 = panelUI->Clone();
	panelUI2->GetTransform()->SetWorldPosition(Vector2(400, 400));

	ButtonUI* childUI = new ButtonUI(this, false);
	childUI->GetTransform()->SetScale(Vector2(100.f, 100.f));
	childUI->GetTransform()->SetWorldPosition(Vector2(450, 450));
	childUI->SetName(L"ChildUI");
	//childUI->SetClickedCallBack(this, (SCENE_MEMFUNC)& Scene_Tool::SaveSceneData);
	panelUI->AddChild(childUI);

	Instantiate(panelUI, ACTOR_GROUP::UI);
	Instantiate(childUI, ACTOR_GROUP::UI);
	Instantiate(panelUI2, ACTOR_GROUP::UI);

	CollisionManager::GetInstance()->MarkGroup(ACTOR_GROUP::PLAYER, ACTOR_GROUP::ENEMY);
}

void Scene_Tool::Update(float deltaTime)
{
	Scene::Update(deltaTime);
}

Scene_Tool::Scene_Tool()
{
}

Scene_Tool::~Scene_Tool()
{
}
//
//void Scene_Tool::SaveSceneData()
//{
//	wchar_t name[256] = {};
//	OPENFILENAME ofn = {};
//	ofn.lStructSize = sizeof(ofn);
//	ofn.hwndOwner = nullptr;
//	ofn.lpstrFile = name;
//	ofn.nMaxFile = sizeof(name);
//	ofn.lpstrFilter = L"ALL\0*.*\0Tile\0*.tile\0";
//	ofn.nFilterIndex = 0;
//	ofn.lpstrFileTitle = nullptr;
//	ofn.nMaxFileTitle = 0;
//	ofn.lpstrInitialDir = name;
//	ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;
//
//	// Modal
//	if (GetSaveFileName(&ofn))
//	{
//		SaveScene(name);
//	}
//}

//void Scene_Tool::SaveScene(const std::wstring& path)
//{
//	//2 byte open
//	// FILE : Kernel object -> Organizes stream between RAM & Storage
//	FILE* file = nullptr;
//	_wfopen_s(&file, path.c_str(), L"wb");
//	assert(file);
//	for (size_t i = 0; i < (size_t)ACTOR_GROUP::END; ++i)
//	{
//		std::vector<class Actor*>& actors = GetActorGroup(ACTOR_GROUP(i));
//		for (Actor* actor : actors)
//		{
//			actor->SaveCharacterData(file);
//		}
//	}
//	fclose(file);
//}
//
//void Scene_Tool::LoadSceneData()
//{
//	wchar_t name[256] = {};
//	OPENFILENAME ofn = {};
//	ofn.lStructSize = sizeof(ofn);
//	ofn.hwndOwner = nullptr;
//	ofn.lpstrFile = name;
//	ofn.nMaxFile = sizeof(name);
//	ofn.lpstrFilter = L"ALL\0*.*\0Tile\0*.tile\0";
//	ofn.nFilterIndex = 0;
//	ofn.lpstrFileTitle = nullptr;
//	ofn.nMaxFileTitle = 0;
//	ofn.lpstrInitialDir = name;
//	ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;
//
//	// Modal
//	if (GetOpenFileName(&ofn))
//	{
//		LoadScene(name);
//	}
//}
//
//void Scene_Tool::LoadScene(const std::wstring& path)
//{
//	//2 byte open
//	// FILE : Kernel object -> Organizes stream between RAM & Storage
//	FILE* file = nullptr;
//	_wfopen_s(&file, path.c_str(), L"rb");
//	assert(file);
//	for (unsigned int i = 0; i < (unsigned int)ACTOR_GROUP::END; ++i)
//	{
//		for (size_t j = 0; j < GetActorGroup((ACTOR_GROUP)i).size(); ++j)
//		{
//			Vector2 pos = Vector2::Zero;
//			fread(&pos, sizeof(Vector2), 1, file);
//			GetActorGroup((ACTOR_GROUP)i)[j]->GetTransform()->SetWorldPosition(pos);
//		}
//	}
//	fclose(file);
//}