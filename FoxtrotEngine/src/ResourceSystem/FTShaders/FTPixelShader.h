#pragma once
#include "ResourceSystem/FTShaders/FTShader.h"

class FTPixelShader :
	public FTShader
{
public:
	void CompileShader(FoxtrotRenderer* renderer) override;

public:
	ComPtr<ID3D11PixelShader>& GetShader();
	ComPtr<ID3D11Buffer>&	   GetEffectsBuffer();

private:
	ComPtr<ID3D11PixelShader> mShader;
	// Additional Buffer to create an effect
	ComPtr<ID3D11Buffer> mEffectsBuffer;
};
