#include "SpineAnimRenderer.h"

SpineAnimRenderer::SpineAnimRenderer(Actor* owner, int updateOrder)
	: MeshRenderer(owner, updateOrder)
	, animation(new FTSpineAnimation)
{
}

void SpineAnimRenderer::Update(float deltaTime)
{
	animation->Update(deltaTime);
}

void SpineAnimRenderer::Render(FoxtrotRenderer* renderer)
{
	animation->Render(renderer);
}