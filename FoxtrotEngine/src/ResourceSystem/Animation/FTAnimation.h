#pragma once
#include "ResourceSystem/FTBasicMeshGroup.h"

class Animator;

class FTAnimation : public FTBasicMeshGroup
{
public:
	enum class AnimationType
	{
		NOT_ASSIGNED,
		SPRITE_ANIMATION
	};

public:
	int GetMaxFrameIdx() const;
	int GetMinFrameIdx() const;

	void SetFrameDuration(int frameNum, float duration);

public:
	virtual void Initialize(
		FTDS::DynamicArray<FTMeshData>&& meshData,
		ComPtr<ID3D11Device>&			 device,
		ComPtr<ID3D11DeviceContext>&	 context) override;

public:
	FTAnimation();
	FTAnimation(FTAnimation* other);

private:
	// These fields need to be loaded from .chunk file
	AnimationType mType;
	float		  mAnimFPS;
	bool		  mIsRepeated;
	int			  mMinFrameIdx;
	int			  mMaxFrameIdx;

public:
	virtual void SaveProperties(std::ofstream& ofs) override;
	virtual void LoadProperties(std::ifstream& ifs) override;

#ifdef FOXTROT_EDITOR
public:
	// Note that this member function is not overriden.
	// Values can be modified only in Animator Component.
	void UpdateUI();

#endif // FOXTROT_EDITOR
};

namespace ChunkKey
{
	namespace Animation
	{
		constexpr const char* TYPE			= "Type";
		constexpr const char* FPS			= "FPS";
		constexpr const char* IS_REPEATED	= "Is Repeated";
		constexpr const char* MAX_FRAME_IDX = "Max Frame Index";
		constexpr const char* MIN_FRAME_IDX = "Min Frame Index";

		constexpr const char* LOADED_KEYS = "Loaded Keys";
	} // namespace Animation
} // namespace ChunkKey