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
		/// @see FTResource::SaveProperties()
		virtual void SaveProperties(std::ofstream& ofs) override = 0;

		/// @see FTResource::LoadProperties()
		virtual void LoadProperties(std::ifstream& ifs) override = 0;

		//////////////////////
		/// Initialization ///
		//////////////////////
	protected:
		/// @brief Compiles shader base on its type.
		virtual void CompileShader(Common::FTResourceDef& resDef, Graphics::IRenderer* renderer) = 0;

		/// @brief Loads the meta data for this wrapper.
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