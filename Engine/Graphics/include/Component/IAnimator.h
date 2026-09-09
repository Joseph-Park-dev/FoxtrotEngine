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
		static inline const char* NAME = "Animator";
		/// @brief Returns the name used by this ianimator.
		/// @return Borrowed access to the name.
		virtual const char*		  GetName() override
		{
			return "Animator";
		}

	public:
		/// @brief Starts or selects animation playback.
		/// @param idx Zero-based element index.
		/// @param isRepeated Whether playback repeats after its final frame.
		virtual void Play(const size_t idx, bool isRepeated = true) = 0;
		/// @brief Stops animation playback.
		virtual void Stop()											= 0;

	public:
		/// @brief Returns the is finished used by this ianimator.
		/// @return Current value of the is finished flag.
		virtual bool GetIsFinished() const	 = 0;
		/// @brief Returns the curr frame idx used by this ianimator.
		/// @return Current curr frame idx.
		virtual int	 GetCurrFrameIdx() const = 0;

		/// @brief Updates the frame used by subsequent operations.
		/// @param frameNumber Replacement frame.
		virtual void SetFrame(int frameNumber) = 0;
		/// @brief Updates the is finished used by subsequent operations.
		/// @param val Replacement is finished.
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
