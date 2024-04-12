#include "BackgroundActor.h"
#include "Transform.h"
#include "TileMapComponent.h"
#include "Texture.h"
#include "ResourceManager.h"

void BackgroundActor::InitializeActor()
{
	SetName(L"Background");
	GetTransform()->SetWorldPosition(Vector2(0.f, 0.f));
	////bg->SetScreenSize(Vector2(1024.f, 768.f));
	Texture* bgTexs = nullptr;
	bgTexs = {
		ResourceManager::GetInstance()->GetLoadedTexture(L"tex01")
	};

	TileMapComponent* tiled1 = new TileMapComponent(this, 52);
	tiled1->SetTexture(bgTexs);
	tiled1->InitializeTileMap(L"Assets/MapLayer1.csv", 32, 32);

	TileMapComponent* tiled2 = new TileMapComponent(this, 51);
	tiled2->SetTexture(bgTexs);
	tiled2->InitializeTileMap(L"Assets/MapLayer2.csv", 32, 32);

	TileMapComponent* tiled3 = new TileMapComponent(this, 50);
	tiled3->SetTexture(bgTexs);
	tiled3->InitializeTileMap(L"Assets/MapLayer3.csv", 32, 32);
	this->AddLayer(tiled1);
	this->AddLayer(tiled2);
	this->AddLayer(tiled3);
}

void BackgroundActor::AddLayer(TileMapComponent* sprite)
{
	int myDrawOrder = sprite->GetDrawOrder();
	auto iter = backgroundLayers.begin();
	for (;
		iter != backgroundLayers.end();
		++iter)
	{
		if (myDrawOrder < (*iter)->GetDrawOrder())
		{
			break;
		}
	}
	backgroundLayers.insert(iter, sprite);
}

void BackgroundActor::RemoveLayer(TileMapComponent* sprite)
{
	auto iter = std::find(backgroundLayers.begin(), backgroundLayers.end(), sprite);
	backgroundLayers.erase(iter);
}

BackgroundActor::BackgroundActor(Scene* scene)
	: Actor(scene)
{
	InitializeActor();
}

void BackgroundActor::RenderActor(SDL_Renderer* renderer)
{
	for (TileMapComponent* layer : backgroundLayers)
		layer->Render(renderer);
}