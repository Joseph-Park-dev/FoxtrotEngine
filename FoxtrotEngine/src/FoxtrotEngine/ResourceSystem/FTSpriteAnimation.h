#pragma once
#include "FoxtrotEngine/ResourceSystem/FTBasicMeshGroup.h"

#include <vector>
#include <string>

#include "FoxtrotEngine/Core/TemplateFunctions.h"
#include "FoxtrotEngine/Math/FTMath.h"
#include "FoxtrotEngine/ResourceSystem/Tile.h"

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

public:
	virtual void Update(float deltaTime) override;

public:
	std::string& GetName() { return mName; }
	AnimationFrame* GetFrame()
	{
		if (mIsFinished)
			LogString("Reel has finished playing");
		return mReel[mCurrFrame];
	}
	void SetName(std::string name) { mName.assign(name); }
	void SetFrame(int frameNumber) 
	{
		mIsFinished = false;
		mCurrFrame = frameNumber;
		mAccTime = 0.f;
	}
	void SetFrameDuration(int frameNum, float duration);
	void SetAnimator(AnimatorComponent* animator) { mAnimator = animator; }
	bool IsFinished() { return mIsFinished; }

protected:
	virtual void InitializeMeshes(
		ComPtr<ID3D11Device>& device, 
		std::vector<MeshData>& meshes
	) override;

public:
	FTSpriteAnimation();

private:
	// These fields need to be loaded from .chunk file
	std::string					 mName;
	float						 mAnimFPS;
	bool						 mIsRepeated;

	// These fields need to be initialized when the component is added.
	AnimatorComponent*			 mAnimator;
	std::vector<AnimationFrame*> mReel;
	int							 mCurrFrame;
	float						 mAccTime;
	bool						 mIsFinished;
};

