#pragma once
#include "Actors/Actor.h"

class TileMapComponent;

class BackgroundActor
	: public Actor
{
public:
	void Initialize();

	void AddLayer(TileMapComponent* sprite);
	void RemoveLayer(TileMapComponent* sprite);

public:
	BackgroundActor(Scene* scene);

public:
	//virtual void RenderActor(FoxtrotRenderer* renderer) override;

private:
	std::vector<TileMapComponent*> backgroundLayers;
};