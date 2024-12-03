#pragma once
#include "ResourceSystem/FTBasicMeshGroup.h"

#include <vector>
#include <string>

#include "Core/TemplateFunctions.h"
#include "Math/FTMath.h"
#include "ResourceSystem/Tile.h"

class Tile;
class AnimatorComponent;
class FTTexture;

struct AnimationFrame : Mesh
{ 
	float Duration;
};

class FTSpriteAnimation : public FTBasicMeshGroup
{
public:
	virtual void Initialize(
		std::vector<MeshData>& meshes,
		ComPtr<ID3D11Device>& device,
		ComPtr<ID3D11DeviceContext>& context
	) override;

	virtual void Update(float deltaTime) override;
	virtual	void Render(ComPtr<ID3D11DeviceContext>& context) override;

public:
	std::string& GetName() { return mName; }
	AnimationFrame* GetFrame()
	{
		if (mIsFinished)
			LogString("Reel has finished playing");
		return mReel[mCurrFrame];
	}

	UINT GetTexKey		() { return mTexKey; }
	UINT GetTileMapKey	() { return mTileMapKey; }
	bool GetIsFinished	() { return mIsFinished; }

	void SetName(std::string name) { mName.assign(name); }
	void SetFrame(int frameNumber) 
	{
		mIsFinished = false;
		mCurrFrame = frameNumber;
		mAccTime = 0.f;
	}
	void SetFrameDuration(int frameNum, float duration);
	void SetAnimator(AnimatorComponent* animator) { mAnimator = animator; }
	void SetIsFinished(bool val) { mIsFinished = val; }
	void SetIsRepeated(bool val) { mIsRepeated = val; }

	void SetTexKey		(UINT key)	 { mTexKey = key; }
	void SetTileMapKey	(UINT key)	 { mTileMapKey = key; }

public:
	FTSpriteAnimation();
	~FTSpriteAnimation() override;

protected:
	virtual void InitializeMeshes(
		ComPtr<ID3D11Device>& device, 
		std::vector<MeshData>& meshes
	) override;

private:
	// These fields need to be loaded from .chunk file
	std::string					 mName;
	float						 mAnimFPS;
	bool						 mIsRepeated;

	UINT						 mTexKey;
	UINT						 mTileMapKey;
	
	// These fields need to be initialized when the component is added.
	AnimatorComponent*			 mAnimator;
	std::vector<AnimationFrame*> mReel;
	float						 mMaxFrame;
	float						 mCurrFrame;
	float						 mAccTime;
	bool						 mIsFinished;

private:
	bool FrameIsWithinIndexRange();

public:
	virtual void SaveProperties(std::ofstream& ofs, UINT key) override;
	virtual UINT LoadProperties(std::ifstream& ifs)           override;

#ifdef FOXTROT_EDITOR
public:
	// Note that this member function is not overriden.
	// Values can be modified only in Animator Component.
	void UpdateUI();

private:
	void UpdateIsRepeated();
	void OnConfirmUpdate();

#endif // FOXTROT_EDITOR
};

