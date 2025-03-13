#include "FTPixelShader.h"

#include "Renderer/FoxtrotRenderer.h"
#include "Renderer/D3D11Utils.h"

void FTPixelShader::CompileShader(FoxtrotRenderer* renderer)
{
	D3D11Utils::CreatePixelShader(
		renderer->GetDevice(),
		ToWString(GetRelativePath()),
		mShader);
}

ComPtr<ID3D11PixelShader>& FTPixelShader::GetShader() { return mShader; }
ComPtr<ID3D11Buffer>&	   FTPixelShader::GetEffectsBuffer() { return mEffectsBuffer; }