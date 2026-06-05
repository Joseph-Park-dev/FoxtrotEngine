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
#include "ISpriteRenderer.h"

namespace Graphics
{
	class IAnimator :
		public ISpriteRenderer
	{
	public:
		virtual void Play(const size_t idx, bool isRepeated = true) = 0;
		virtual void Stop()											= 0;

	public:
		virtual bool GetIsFinished() const	 = 0;
		virtual int	 GetCurrFrameIdx() const = 0;

		virtual void SetFrame(int frameNumber) = 0;
		virtual void SetIsFinished(bool val)   = 0;
	};

	namespace ChunkKey
	{
		namespace FTSpriteAnimator
		{
			constexpr const char* LOADED_KEYS = "Loaded Keys";

		} // namespace FTSpriteAnimator
	} // namespace ChunkKey
} // namespace Graphics