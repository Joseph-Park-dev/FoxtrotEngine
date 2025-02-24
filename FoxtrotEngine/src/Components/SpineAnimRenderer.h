#pragma once
#include "MeshRenderer.h"

#include "ResourceSystem/FTSpineAnimation.h"

class SpineAnimRenderer :
	public MeshRenderer
{
public:
	SpineAnimRenderer(Actor* owner, int updateOrder = DefaultVal::UPDATE_ORDER);

	virtual void CloneTo(Actor* actor) override;

public:
	virtual void Initialize(FTCore* coreInstance) override;
	virtual void Update(float deltaTime) override;
	virtual void Render(FoxtrotRenderer* renderer) override;

private:
	FTSpineAnimation* animation;
};