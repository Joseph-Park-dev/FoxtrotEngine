// ----------------------------------------------------------------
// Foxtrot Engine 2D
// Copyright (C) 2025 JungBae Park. All rights reserved.
//
// Released under the GNU General Public License v3.0
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

#pragma once
#include "IResource.h"

namespace Common
{
	struct FTResourceDef;
}

namespace Graphics
{
	class IRenderer;
}

namespace Graphics
{
	/// @brief A base abstract wrapper for HLSL Shaders.
	class IShader :
		public Common::IResource
	{
		/////////////////
		/// Chunk I/O ///
		/////////////////
	public:
		/// @brief Serializes this object's persistent properties to a .chunk stream.
		/// @see FTResource::SaveProperties()
		/// @param ofs Output stream receiving the serialized data.
		/// @note Writes to the supplied stream at its current position.
		virtual void SaveProperties(std::ofstream& ofs) override = 0;

		/// @brief Restores this object's persistent properties from a .chunk stream.
		/// @see FTResource::LoadProperties()
		/// @param ifs Input stream positioned at the expected data; reading advances its position.
		/// @note Advances the stream position and updates the destination state.
		virtual void LoadProperties(std::ifstream& ifs) override = 0;

		//////////////////////
		/// Initialization ///
		//////////////////////
	protected:
		/// @brief Compiles shader base on its type.
		/// @param resDef Resource definition containing the filename and source path.
		/// @param renderer Renderer providing the graphics device and current render state.
		virtual void CompileShader(Common::FTResourceDef& resDef, Graphics::IRenderer* renderer) = 0;

		/// @brief Loads the meta data for this wrapper.
		/// @param resDef Resource definition containing the filename and source path.
		virtual void LoadMetaData(Common::FTResourceDef& resDef) = 0;
	};

	namespace ChunkKey
	{
		namespace Shader
		{
			constexpr const char* FTSHADER = "FTShader";
			constexpr const char* NAME	   = "Name";
		} // namespace Shader
	} // namespace ChunkKey
} // namespace Graphics
