#pragma once
#include "Actor.h"

class TileMapComponent;

class BackgroundActor
	: public Actor
{
public:
	void InitializeActor() override;

	void AddLayer(TileMapComponent* sprite);
	void RemoveLayer(TileMapComponent* sprite);

public:
	BackgroundActor(Scene* scene);

public:
	virtual void RenderActor(SDL_Renderer* renderer) override;

private:
	std::vector<TileMapComponent*> backgroundLayers;
};

