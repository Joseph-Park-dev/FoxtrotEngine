#include "SpineAnimRenderer.h"

SpineAnimRenderer::SpineAnimRenderer(Actor* owner, int updateOrder)
	: MeshRenderer(owner, updateOrder)
	, animation(new FTSpineAnimation(GetRenderer()))
{
}

void SpineAnimRenderer::CloneTo(Actor* actor)
{
}

void SpineAnimRenderer::Initialize(FTCore* coreInstance)
{
	animation->Initialize(coreInstance->GetGameRenderer());
	SetRenderer(coreInstance->GetGameRenderer());
	mMaterial = new FTMaterial;
	SetMeshGroup(animation);
}

void SpineAnimRenderer::Update(float deltaTime)
{
	MeshRenderer::Update(deltaTime);
	animation->Update(deltaTime);
}

void SpineAnimRenderer::Render(FoxtrotRenderer* renderer)
{
	animation->Render(renderer);
}