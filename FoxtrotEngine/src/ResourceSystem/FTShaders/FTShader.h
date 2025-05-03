#pragma once
#include "ResourceSystem/FTResource.h"

#include <wrl.h>
#include <d3d11.h>

class FoxtrotRenderer;

using namespace Microsoft::WRL;

enum class ShaderType
{
	VERTEX_SHADER,
	PIXEL_SHADER
};

class FTShader :
	public FTResource
{
public:
	virtual void CompileShader(FoxtrotRenderer* renderer) = 0;

public:
	const ShaderType& GetType() const;
	void			  SetType(ShaderType type);

public:
	FTShader();

private:
	ShaderType mType;

#ifdef FOXTROT_EDITOR
public:
	void			 SaveMetaFile();
	FoxtrotRenderer* GetRenderer();

public:
	FTShader(FoxtrotRenderer* renderer);

private:
	// Currently used in FTVertexShader::UpdateUI()
	// (Not necessary in Game)
	FoxtrotRenderer* mRenderer;
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