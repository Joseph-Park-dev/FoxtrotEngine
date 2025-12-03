// ----------------------------------------------------------------
// Foxtrot Engine 2D
// Copyright (C) 2025 JungBae Park. All rights reserved.
//
// Released under the GNU General Public License v3.0
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

#pragma once
#include <ResourceSystem/FTResource.h>

class FoxtrotRenderer;

enum class ShaderType
{
	VERTEX_SHADER,
	PIXEL_SHADER,
	GEOMETRY_SHADER
};

/// @brief A base abstract wrapper for HLSL Shaders.
class FTShader :
	public FTResource
{
public:
	/// @brief Loads the meta data and compiles the shader.
	FTShader(FTResourceDef& resDef, FoxtrotRenderer* renderer);

private:
	/// @brief Type of the shader (e.g vertex shader)
	ShaderType mType;

protected:
	/// @brief Compiles shader base on its type.
	virtual void CompileShader(FoxtrotRenderer* renderer) = 0;

	/// @brief Set current shader type during initialization.
	void SetType(ShaderType&& shaderType);

	/// @brief Loads the meta data for this wrapper.
	void LoadMetaData();

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