#include "FTVertexShader.h"

#include "Renderer/FoxtrotRenderer.h"
#include "Renderer/D3D11Utils.h"

void FTVertexShader::CompileShader(FoxtrotRenderer* renderer)
{
	D3D11Utils::CreateVertexShaderAndInputLayout(
		renderer->GetDevice(),
		ToWString(GetRelativePath()),
		mInputElements,
		mShader,
		mInputLayout);
}

ComPtr<ID3D11VertexShader>& FTVertexShader::GetShader() { return mShader; }

void FTVertexShader::RegisterInputElementDesc(D3D11_INPUT_ELEMENT_DESC& desc)
{
	mInputElements.push_back(desc);
}