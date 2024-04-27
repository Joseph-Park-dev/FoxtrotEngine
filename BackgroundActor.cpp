#include "BackgroundActor.h"
#include "Transform.h"
#include "TileMapComponent.h"
#include "FTTexture.h"
#include "ResourceManager.h"

void BackgroundActor::InitializeActor()
{
	SetName(L"Background");
	GetTransform()->SetWorldPosition(Vector2(0.f, 0.f));
	////bg->SetScreenSize(Vector2(1024.f, 768.f));

	TileMapComponent* tiled1 = new TileMapComponent(this, 52);
	//tiled1->SetTexture();
	tiled1->InitializeTileMap(L"Assets/MapLayer1.csv", 32, 32);

	TileMapComponent* tiled2 = new TileMapComponent(this, 51);
	//tiled2->SetTexture();
	tiled2->InitializeTileMap(L"Assets/MapLayer2.csv", 32, 32);

	TileMapComponent* tiled3 = new TileMapComponent(this, 50);
	//tiled3->SetTexture();
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

//void BackgroundActor::RenderActor(FoxtrotRenderer* renderer)
//{
//	for (TileMapComponent* layer : backgroundLayers)
//		layer->Render(renderer);
//}