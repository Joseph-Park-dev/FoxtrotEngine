#include "BGSpriteComponent.h"
#include "Actor.h"
#include "Transform.h"

BGSpriteComponent::BGSpriteComponent(Actor* owner, int drawOrder, int updateOrder)
	:SpriteComponent(owner, drawOrder)
	, mScrollSpeed(0.0f)
{}

void BGSpriteComponent::Update(float deltaTime)
{
	SpriteComponent::Update(deltaTime);
	for (auto& bg : mBGTextures)
	{
		bg.mOffset.x += mScrollSpeed * deltaTime;
		if (bg.mOffset.x < -mScreenSize.x)
		{
			bg.mOffset.x = (mBGTextures.size() - 1) * mScreenSize.x - 2;
		}
	}
}

void BGSpriteComponent::Render(SDL_Renderer* renderer)
{
	for (auto& bg : mBGTextures)
	{
		SDL_Rect r;
		r.w = static_cast<int>(mScreenSize.x);
		r.h = static_cast<int>(mScreenSize.y);
		r.x = static_cast<int>(GetOwner()->GetTransform()->GetWorldPosition().x - r.w / 2 + bg.mOffset.x);
		r.y = static_cast<int>(GetOwner()->GetTransform()->GetWorldPosition().y - r.h / 2 + bg.mOffset.y);

		SDL_RenderCopy(renderer,
			bg.mTexture,
			nullptr,
			&r);
	}
}

void BGSpriteComponent::SetBGTextures(const std::vector<SDL_Texture*>& textures)
{
	int count = 0;
	for (auto tex : textures)
	{
		BGTexture temp;
		temp.mTexture = tex;
		temp.mOffset.x = count * mScreenSize.x;
		temp.mOffset.y = 0;
		mBGTextures.emplace_back(temp);
		count++;
	}
}