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
	bool		GetIsFinished() const;
	int			GetCurrFrameIdx() const;

	void SetFrame(int frameNumber);
	void SetFrameDuration(int frameNum, float duration);
	void SetAnimator(Animator* animator);
	void SetIsFinished(bool val);
	void SetIsRepeated(bool val);
	void IncreaseIdx();

public:
	virtual void Update(float deltaTime);
	virtual void Render(FoxtrotRenderer* renderer) override;

public:
	FTAnimation();
	FTAnimation(FTAnimation* other);

protected:
	virtual void InitializeMeshes(
		ComPtr<ID3D11Device>&	 device,
		std::vector<FTMeshData>& meshes) override;

	/// Moves onto the next frame as the mAccTime exceeds the frame duration.
	void UpdateFrame(float deltaTime, float frameDuration);

private:
	// These fields need to be loaded from .chunk file
	AnimationType mType;
	float		  mAnimFPS;
	bool		  mIsRepeated;
	int			  mMaxFrameIdx;

	// These fields need to be initialized when the component is added.
	Animator* mAnimator;
	int		  mCurrFrameIdx;
	float	  mAccTime;
	bool	  mIsFinished;

private:
	bool FrameIsWithinIndexRange(int currentFrame);

public:
	virtual void SaveProperties(std::ofstream& ofs, UINT key) override;
	virtual UINT LoadProperties(std::ifstream& ifs) override;

#ifdef FOXTROT_EDITOR
public:
	// Note that this member function is not overriden.
	// Values can be modified only in Animator Component.
	void UpdateUI();

private:
	void UpdateIsRepeated();
	void UpdateMaxFrame();

#endif // FOXTROT_EDITOR
};

namespace ChunkKey
{
	namespace Animation
	{
		constexpr const char* NAME			= "Name";
		constexpr const char* TYPE			= "Type";
		constexpr const char* FPS			= "FPS";
		constexpr const char* IS_REPEATED	= "Is Repeated";
		constexpr const char* MAX_FRAME_IDX = "Max Frame Index";

	} // namespace Animation
} // namespace ChunkKey