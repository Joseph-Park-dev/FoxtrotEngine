#pragma once
#include "FTShader.h"

#include <d3d11.h>
#include <wrl.h>

class FTGeometryShader :
	public FTShader
{
public:
	/// @brief Returns compiled HLSL pixel shader object.
	Microsoft::WRL::ComPtr<ID3D11GeometryShader>& GetShader();

public:
	/// @see FTShader::FTShader()
	FTGeometryShader(FTResourceDef& resDef, FoxtrotRenderer* renderer);

protected:
	/// @brief Loads meta data, and compiles the shader.
	void Process(FoxtrotRenderer* renderer) override;

	/// @see FTShader::CompileShader()
	void CompileShader(FoxtrotRenderer* renderer) override;

private:
	/// @brief The shader should remain compiled after initialization.
	Microsoft::WRL::ComPtr<ID3D11GeometryShader> mShader;
};
