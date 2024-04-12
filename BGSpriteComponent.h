#pragma once
#include <vector>

#include "SpriteComponent.h"
#include "TemplateFunctions.h"

class Vector2;

struct BGTexture
{
	SDL_Texture* mTexture;
	Vector2		 mOffset;
};

class BGSpriteComponent : public SpriteComponent
{
public:
	void SetBGTextures(const std::vector<SDL_Texture*>& textures);

public:
	float GetScrollSpeed() const { return mScrollSpeed; }
	std::wstring GetName() const override
	{
		return L"BGSpriteComponent";
	}
	void SetScreenSize(const Vector2& size) { mScreenSize = size; }
	void SetScrollSpeed(float speed) { mScrollSpeed = speed; }

public:
	void Update(float deltaTime) override;
	virtual void Render(SDL_Renderer* renderer) override;

	CLONE(BGSpriteComponent);

public:
	BGSpriteComponent(class Actor* owner, int drawOrder = 10, int updateOrder = DEFAULT_UPDATEORDER);

private:
	std::vector<BGTexture> mBGTextures;
	Vector2		mScreenSize;
	float		mScrollSpeed;
};

