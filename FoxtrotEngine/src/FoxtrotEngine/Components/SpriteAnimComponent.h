#pragma once
#include "FoxtrotEngine/Components/SpriteRendererComponent.h"

#include <vector>

#include "FoxtrotEngine/Core/TemplateFunctions.h"

class SpriteAnimComponent :public SpriteRendererComponent
{
public:
	virtual void Initialize(FTCore* coreInstance)	override;
	virtual void Update(float deltaTime)			override;
	virtual void Render(FoxtrotRenderer* renderer)	override;	

public:
	float		 GetAnimFPS() const { return mAnimFPS; }
	void		 SetAnimFPS(float fps) { mAnimFPS = fps; }
	std::wstring GetName() const override
	{
		return L"SpriteAnimComponent";
	}

	void SetAnimTextures(const std::vector<FTTexture*>& textures);

public:
	SpriteAnimComponent(class Actor* owner, int drawOrder = DEFAULT_DRAWORDER, int updateOrder = DEFAULT_UPDATEORDER);

private:
	std::vector<FTTexture*> mAnimTextures;
	float				   mCurrFrame;
	float				   mAnimFPS;
};

