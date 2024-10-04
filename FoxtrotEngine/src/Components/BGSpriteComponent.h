#pragma once
#include "Components/SpriteRendererComponent.h"

#include <vector>

#include "Core/TemplateFunctions.h"

class FTVector2;

struct BGTexture {
	FTTexture *mTexture;
	FTVector2 mOffset;
};

class BGSpriteComponent : public SpriteRendererComponent {
  public:
	void SetBGTextures(const std::vector<FTTexture *> &textures);

  public:
	float GetScrollSpeed() const { return mScrollSpeed; }
	std::wstring GetName() const override { return L"BGSpriteComponent"; }

	void SetScreenSize(const FTVector2 &size) { mScreenSize = size; }
	void SetScrollSpeed(float speed) { mScrollSpeed = speed; }

  public:
	void Update(float deltaTime) override;
	virtual void Render(FoxtrotRenderer *renderer) override;

  public:
	BGSpriteComponent(class Actor *owner, int drawOrder = 10,
					  int updateOrder = DEFAULT_UPDATEORDER);

  private:
	std::vector<BGTexture> mBGTextures;
	FTVector2 mScreenSize;
	float mScrollSpeed;
};
