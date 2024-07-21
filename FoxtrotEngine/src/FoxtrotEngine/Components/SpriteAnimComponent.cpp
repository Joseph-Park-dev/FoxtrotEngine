#include "FoxtrotEngine/Components/SpriteAnimComponent.h"

#include "FoxtrotEngine/ResourceSystem/FTTexture.h"

SpriteAnimComponent::SpriteAnimComponent(Actor* owner, int drawOrder, int updateOrder)
	: SpriteRendererComponent(owner, drawOrder)
	, mCurrFrame(0.f)
	, mAnimFPS(24.f)
{}

void SpriteAnimComponent::Initialize(FTCore* coreInstance){
	SpriteRendererComponent::Initialize(coreInstance);
}

void SpriteAnimComponent::Update(float deltaTime){
	SpriteRendererComponent::Update(deltaTime);

	if (mAnimTextures.size() > 0)
	{
		mCurrFrame += mAnimFPS * deltaTime;
		while (mCurrFrame >= mAnimTextures.size())
		{
			mCurrFrame -= mAnimTextures.size();
		}
	}
	SetTexture(mAnimTextures[static_cast<int>(mCurrFrame)]);
}

void SpriteAnimComponent::Render(FoxtrotRenderer* renderer){
	SpriteRendererComponent::Render(renderer);
}

//void SpriteAnimComponent::SetAnimTextures(const std::vector<FTTexture*>& textures)
//{
//	mAnimTextures = textures;
//	if (mAnimTextures.size() > 0)
//	{
//		mCurrFrame = 0.0f;
//		//SetTexture(mAnimTextures[0]);
//	}
//}
