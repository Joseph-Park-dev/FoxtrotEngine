#pragma once
#include <vector>

#include "Components/TileMapComponent.h"
#include "Core/TemplateFunctions.h"

class SpriteAnimComponent : public TileMapComponent
{
public:
	virtual void Initialize(FTCore* coreInstance) override;
	virtual void Update(float deltaTime) override;
	virtual void Render(FoxtrotRenderer* renderer) override;

public:
	void Play();
	void Stop();

public:
	std::wstring GetName() const override { return L"SpriteAnimComponent"; }

public:
	float GetAnimFPS() const { return mAnimFPS; }

	void SetAnimFPS(float fps) { mAnimFPS = fps; }
	void SetAnimTextures(const std::vector<FTTexture*>& textures);

public:
	SpriteAnimComponent(class Actor* owner, int drawOrder = DEFAULT_DRAWORDER,
		int updateOrder = DEFAULT_UPDATEORDER);

private:
	// These fields need to be loaded from .chunk file
	float mAnimFPS;
	bool  mIsRepeated;

	// These fields need to be initialized when the component is added.
	float mCurrFrame;
	UINT  mMaxFrame;
	bool  mIsPlaying;

private:
	void InitializeAnimation();
	bool FrameIsWithinIndexRange();

#ifdef FOXTROT_EDITOR
public:
	virtual void EditorUIUpdate() override;
	void		 UpdateIsRepeated();
	void		 UpdatePlayAnim();
	virtual void OnConfirmUpdate() override;

	virtual void SaveProperties(nlohmann::ordered_json& out) override;
	// virtual void LoadProperties(std::ifstream& ifs) override;
	// _DEBUG
#endif
};
