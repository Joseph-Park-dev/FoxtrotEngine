#include "AnimSpriteComponent.h"
#include "Texture.h"

AnimSpriteComponent::AnimSpriteComponent(Actor* owner, int drawOrder, int updateOrder)
	: SpriteComponent(owner, drawOrder)
	, mCurrFrame(0.f)
	, mAnimFPS(24.f)
{}

void AnimSpriteComponent::Update(float deltaTime)
{
	SpriteComponent::Update(deltaTime);

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

void AnimSpriteComponent::SetAnimTextures(const std::vector<Texture*>& textures)
{
	mAnimTextures = textures;
	if (mAnimTextures.size() > 0)
	{
		mCurrFrame = 0.0f;
		SetTexture(mAnimTextures[0]);
	}
}
