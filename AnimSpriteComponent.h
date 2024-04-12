#pragma once
#include <vector>

#include "SpriteComponent.h"
#include "TemplateFunctions.h"

class AnimSpriteComponent :public SpriteComponent
{
public:
	void SetAnimTextures(const std::vector<Texture*>& textures);
	
public:
	float		 GetAnimFPS() const { return mAnimFPS; }
	void		 SetAnimFPS(float fps) { mAnimFPS = fps; }
	std::wstring GetName() const override
	{
		return L"AnimSpriteComponent";
	}

	CLONE(AnimSpriteComponent);

public:
	void Update(float deltaTime) override;

public:
	AnimSpriteComponent(class Actor* owner, int drawOrder = DEFAULT_DRAWORDER, int updateOrder = DEFAULT_UPDATEORDER);

private:
	std::vector<Texture*> mAnimTextures;
	float				   mCurrFrame;
	float				   mAnimFPS;
};

