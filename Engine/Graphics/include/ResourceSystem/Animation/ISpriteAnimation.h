// ----------------------------------------------------------------
// Foxtrot Engine 2D
// Copyright (C) 2025 JungBae Park. All rights reserved.
//
// Released under the GNU General Public License v3.0
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

#pragma once
#include "Mesh/IModel.h"

namespace GenericData
{
	class FTJSON;
} // namespace GenericData

namespace Graphics
{
	struct SpriteAnimData :
		Common::ResourceData
	{
		/// @brief A JSON file that holds the rect data on a spritesheet.
		GenericData::FTJSON* JSON = nullptr;

		/// @brief Frames-per-second for this animation.
		int FPS;

		/// @brief Should this animation be looped?
		bool IsRepeated;

		/// @brief the first frame index.
		int MinFrameIdx;

		/// @brief the last frame index.
		int MaxFrameIdx;
	};

	/// @brief A FTResource that holds a Sprite Animation.
	/// This will be registered to a Animator Component instance.
	class ISpriteAnimation :
		public IModel
	{
	public:
		/// @brief Returns Frames per second
		/// @return Current fps.
		virtual const int GetFPS() const = 0;

		/// @brief Returns the last frame index.
		/// @return Current max frame idx.
		virtual const int GetMaxFrameIdx() const = 0;

		/// @brief Returns the first frame index.
		/// @return Current min frame idx.
		virtual const int GetMinFrameIdx() const = 0;

		/// @brief Returns the frame count used by this isprite animation.
		/// @return Current frame count.
		virtual const size_t GetFrameCount() const = 0;

	public:
		/// @brief Completes destruction through the object's inheritance hierarchy.
		virtual ~ISpriteAnimation() override = 0;
	};

	namespace ChunkKey
	{
		namespace FTSpriteAnimation
		{
			constexpr const char* FT_SPRITE_ANIMATION = "FTSpriteAnimation";
			constexpr const char* JSON				  = "JSON";
			constexpr const char* SPRITE_SHEET		  = "Sprite Sheet";
			constexpr const char* SIZE_SCALE		  = "Size Scale";
			constexpr const char* FPS				  = "FPS";
			constexpr const char* IS_REPEATED		  = "Is Repeated";
			constexpr const char* MAX_FRAME_IDX		  = "Max Frame Index";
			constexpr const char* MIN_FRAME_IDX		  = "Min Frame Index";

		} // namespace FTSpriteAnimation
	} // namespace ChunkKey

	namespace SpriteSheetKeys
	{
		constexpr const char* BASE		  = "frames";
		constexpr const char* PROPERTIES  = "meta";
		constexpr const char* SIZE		  = "size";
		constexpr const char* FRAME		  = "frame";
		constexpr const char* SOURCE_SIZE = "spriteSourceSize";
		constexpr const char* ROTATED	  = "rotated";
		constexpr const char* TRIMMED	  = "trimmed";
		constexpr const char* PIVOT		  = "pivot";

		constexpr const char* X = "x";
		constexpr const char* Y = "y";
		constexpr const char* W = "w";
		constexpr const char* H = "h";

	} // namespace SpriteSheetKeys
} // namespace Graphics
