#pragma once
#include "MeshRenderer.h"

#include "ResourceSystem/FTSpineAnimation.h"

class SpineAnimRenderer :
	public MeshRenderer
{
public:
	SpineAnimRenderer(Actor* owner, int updateOrder = DEFAULT_UPDATEORDER);

public:
	virtual void Update(float deltaTime) override;
	virtual void Render(FoxtrotRenderer* renderer) override;

private:
	FTSpineAnimation* animation;
};