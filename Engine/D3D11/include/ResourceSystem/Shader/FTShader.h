// ----------------------------------------------------------------
// Foxtrot Engine 2D
// Copyright (C) 2025 JungBae Park. All rights reserved.
//
// Released under the GNU General Public License v3.0
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

#pragma once
#include "ResourceSystem/D3D11Resource.h"

namespace Core
{
	class FoxtrotRenderer;
}

namespace D3D11
{
	class D3D11Renderer;

	enum class ShaderType
	{
		VERTEX_SHADER,
		PIXEL_SHADER,
		GEOMETRY_SHADER
	};

	/// @brief A base abstract wrapper for HLSL Shaders.
	class FTShader :
		public D3D11Resource
	{
	public:
		/// @brief Provides an empty lifecycle or extension hook for this implementation.
		/// @see FTResource::SaveProperties()
		/// @param ofs Output stream receiving the serialized data.
		/// @note Writes to the supplied stream at its current position.
		virtual void SaveProperties(std::ofstream& ofs) override {};

		/// @brief Provides an empty lifecycle or extension hook for this implementation.
		/// @see FTResource::LoadProperties()
		/// @param ifs Input stream positioned at the expected data; reading advances its position.
		/// @note Advances the stream position and updates the destination state.
		virtual void LoadProperties(std::ifstream& ifs) override {};

	public:
		/// @brief Initializes shader metadata and compilation state.
		/// @param resDef Resource definition containing the filename and source path.
		FTShader(Common::FTResourceDef& resDef);

	private:
		/// @brief Type of the shader (e.g vertex shader)
		ShaderType mType;

	protected:
		/// @brief Compiles shader base on its type.
		/// @param resDef Resource definition containing the filename and source path.
		/// @param renderer Renderer providing the graphics device and current render state.
		virtual void CompileShader(Common::FTResourceDef& resDef, D3D11Renderer* renderer) = 0;

		/// @brief Set current shader type during initialization.
		/// @param shaderType Replacement type.
		void SetType(ShaderType&& shaderType);

		/// @brief Loads the meta data for this wrapper.
		/// @param resDef Resource definition containing the filename and source path.
		void LoadMetaData(Common::FTResourceDef& resDef);

#ifdef FOXTROT_EDITOR
	public:
		/// @brief Saves meta data for this wrapper.
		void SaveMetaData();

#endif
	};

	namespace ChunkKey
	{
		namespace Shader
		{
			constexpr const char* FTSHADER = "FTShader";
			constexpr const char* NAME	   = "Name";
		} // namespace Shader
	} // namespace ChunkKey
} // namespace D3D11
