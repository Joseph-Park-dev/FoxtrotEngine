// ----------------------------------------------------------------
// Foxtrot Engine 2D
// Copyright (C) 2025 JungBae Park. All rights reserved.
// 
// Released under the GNU General Public License v3.0
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------
/// <summary>
/// A class that holds the Sprite Animation.
/// This will be registered to the Animator Component assigned to 
/// an Actor.
/// </summary>

#pragma once
#include "ResourceSystem/FTBasicMeshGroup.h"

#include <vector>
#include <string>

#include "Core/TemplateFunctions.h"
#include "Math/FTMath.h"
#include "ResourceSystem/Tile.h"

class Tile;
class Animator;
class FTTexture;

struct AnimationFrame
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

	virtual void Update(float deltaTime);
	virtual	void Render(FoxtrotRenderer* renderer, FTTexture* texture)  override;

public:
	std::string&	GetName	 ();
	AnimationFrame* GetFrame ();
							 
	UINT GetTexKey			 ();
	UINT GetTileMapKey		 ();
	bool GetIsFinished		 ();
							 
	void SetName			 (std::string&& name);
	void SetFrame			 (int frameNumber);
	void SetFrameDuration	 (int frameNum, float duration);
	void SetAnimator		 (Animator* animator);
	void SetIsFinished		 (bool val);
	void SetIsRepeated		 (bool val);
							 
	void SetTexKey			 (UINT key);
	void SetTileMapKey		 (UINT key);

	void IncreaseIdx() { ++mCurrFrame; }

public:
	 FTSpriteAnimation();
	 FTSpriteAnimation(FTSpriteAnimation* other);
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
	int							 mMaxFrameIdx;

	UINT						 mTexKey;
	UINT						 mTileMapKey;
	
	// These fields need to be initialized when the component is added.
	Animator*			 mAnimator;
	std::vector<AnimationFrame*> mReel;
	int							 mCurrFrame;
	float						 mAccTime;
	bool						 mIsFinished;

private:
	bool FrameIsWithinIndexRange(int currentFrame);

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
	void UpdateMaxFrame();

#endif // FOXTROT_EDITOR
};

