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
		/// @see FTResource::SaveProperties()
		virtual void SaveProperties(std::ofstream& ofs) override {};

		/// @see FTResource::LoadProperties()
		virtual void LoadProperties(std::ifstream& ifs) override {};

	public:
		FTShader(Common::FTResourceDef& resDef);

	private:
		/// @brief Type of the shader (e.g vertex shader)
		ShaderType mType;

	protected:
		/// @brief Compiles shader base on its type.
		virtual void CompileShader(Common::FTResourceDef& resDef, D3D11Renderer* renderer) = 0;

		/// @brief Set current shader type during initialization.
		void SetType(ShaderType&& shaderType);

		/// @brief Loads the meta data for this wrapper.
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