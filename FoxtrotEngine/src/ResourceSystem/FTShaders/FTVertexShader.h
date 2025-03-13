#pragma once
#include "ResourceSystem/FTShaders/FTShader.h"

#include <vector>

class FTVertexShader : public FTShader
{
public:
	void CompileShader(FoxtrotRenderer* renderer) override;

public:
	ComPtr<ID3D11VertexShader>& GetShader();

	void RegisterInputElementDesc(D3D11_INPUT_ELEMENT_DESC& desc);

private:
	ComPtr<ID3D11VertexShader>			  mShader;
	ComPtr<ID3D11InputLayout>			  mInputLayout;
	std::vector<D3D11_INPUT_ELEMENT_DESC> mInputElements;
};
