// ----------------------------------------------------------------
// Foxtrot Engine 2D
// Copyright (C) 2025 JungBae Park. All rights reserved.
//
// Released under the GNU General Public License v3.0
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------
/// <summary>
/// Takes care of all FTAnimations registered.
/// All types of animation are handled here as FTResource derived from
/// the same class, FTAnimation.
/// (sprite animation, spine animation, etc.)
/// </summary>

#pragma once
#include "Components/TileMapRenderer.h"

#include <unordered_map>
#include <string>

#include "Core/TemplateFunctions.h"

class FoxtrotRenderer;
class FTSpriteAnimation;

class Animator :
	public TileMapRenderer
{
public:
	virtual FTDS::String GetName() const override
	{
		return "Animator";
	}

public:
	void Play(const UINT key, bool isRepeated = true);
	void Stop();

public:
	bool GetIsFinished() const;
	int	 GetCurrFrameIdx() const;

	void SetFrame(int frameNumber);
	void SetIsFinished(bool val);

public:
	virtual void Initialize(FTCore* coreInstance) override;
	void		 LateUpdate(float deltaTime) override;
	void		 Render(FoxtrotRenderer* renderer) override;

	virtual void CloneTo(Actor* actor) override;

public:
	Animator(
		class Actor* owner,
		int			 updateOrder = DefaultVal::UPDATE_ORDER);
	~Animator() override;

private:
	std::vector<FTSpriteAnimation*> mLoadedAnim;
	FTSpriteAnimation*				mCurrAnim;
	int								mCurrFrameIdx;
	float							mAccTime;
	bool							mIsFinished;
	bool							mIsRepeated;

public:
	virtual void SaveProperties(std::ofstream& ofs) override;
	virtual void LoadProperties(std::ifstream& ifs) override;

private:
	void UpdateFrame(float deltaTime);
	bool IndexOutOfRange(int minIdx, int maxIdx);

#ifdef FOXTROT_EDITOR
public:
	virtual void EditorUpdate(float deltaTime) override;
	virtual void EditorUIUpdate() override;
	void		 EditorRender(FoxtrotRenderer* renderer) override;

private:
	void UpdatePlayAnim();
	void UpdatePlayList();

#endif // FOXTROT_EDITOR
};

namespace ChunkKey
{
	constexpr const char* LOADED_KEYS = "Loaded Keys";
}