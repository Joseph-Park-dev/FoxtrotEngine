// ----------------------------------------------------------------
// Foxtrot Engine 2D
// Copyright (C) 2025 JungBae Park. All rights reserved.
//
// Released under the GNU General Public License v3.0
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

#pragma once
#include "IResource.h"

namespace Graphics
{
	/// @brief A class that holds the image data.
	/// This can be used as a single sprite, a texture pack for a tilemap, and
	/// a spritesheet for an animation, etc.
	class ITexture :
		public Common::IResource
	{
		////////////////////////////
		/// Accessors / Mutators ///
		////////////////////////////
	public:
		/// @brief Get original pixel-width of the image. This shall not be edited after the ITexture is created.
		/// @return Current width.
		virtual const unsigned int GetWidth() const = 0;

		/// @brief Get original pixel-height of the image. This shall not be edited after the ITexture is created.
		/// @return Current height.
		virtual const unsigned int GetHeight() const = 0;

		/////////////////
		/// Chunk I/O ///
		/////////////////
	public:
		/// @brief Saves resource properties into a file.
		/// @param ofs A stream to a .chunk file
		/// @note Writes to the supplied stream at its current position.
		virtual void SaveProperties(std::ofstream& ofs) override = 0;

		/// @brief Loads resource properties into an instance.
		/// @param ifs A stream from a .chunk file
		/// @note Advances the stream position and updates the destination state.
		virtual void LoadProperties(std::ifstream& ifs) override = 0;

		///////////////////////////////////
		/// Const/Destructors & Copying ///
		///////////////////////////////////
	public:
		/// @brief Completes destruction through the object's inheritance hierarchy.
		virtual ~ITexture() override = 0;
	};

	namespace ChunkKey
	{
		namespace FTTexture
		{
			constexpr const char* FT_TEXTURE = "FTTexture";
			constexpr const char* WIDTH		 = "Width";
			constexpr const char* HEIGHT	 = "Height";

		} // namespace FTTexture
	} // namespace ChunkKey
} // namespace Graphics
