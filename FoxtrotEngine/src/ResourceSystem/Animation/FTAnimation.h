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

	void SetFrameDuration(int frameNum, float duration);

public:
	FTAnimation();
	FTAnimation(FTAnimation* other);

protected:
	virtual void InitializeMeshes(
		ComPtr<ID3D11Device>&	 device,
		std::vector<FTMeshData>& meshes) override;

private:
	// These fields need to be loaded from .chunk file
	AnimationType mType;
	float		  mAnimFPS;
	bool		  mIsRepeated;
	int			  mMaxFrameIdx;

public:
	virtual void SaveProperties(std::ofstream& ofs, UINT key) override;
	virtual UINT LoadProperties(std::ifstream& ifs) override;

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

	} // namespace Animation
} // namespace ChunkKey