#pragma once
#include "ResourceSystem/FTShaders/FTShader.h"

#include <vector>

class FTVertexShader : public FTShader
{
public:
	// You may use this overriden menber function.
	void CompileShader(FoxtrotRenderer* renderer) override;

public:
	ComPtr<ID3D11VertexShader>& GetShader();

	void RegisterInputElementDesc(const char* semanticName, D3D11_INPUT_ELEMENT_DESC& desc);

public:
	FTVertexShader();

private:
	ComPtr<ID3D11VertexShader>			  mShader;
	ComPtr<ID3D11InputLayout>			  mInputLayout;

	std::vector<D3D11_INPUT_ELEMENT_DESC> mInputElements;
	std::vector<const char*>			  mSemanticNames;

public:
	virtual void SaveProperties(std::ofstream& ofs) override;
	virtual void LoadProperties(std::ifstream& ifs) override;

#ifdef FOXTROT_EDITOR
public:
	void UpdateUI() override;

private:
	bool mSemanticsInclusion[4];
#endif
};

namespace ChunkKey
{
	constexpr const char* FT_VERTEX_SHADER	= "FTVertexShader";
	constexpr const char* INPUT_ELEMENTS_COUNT = "Input Elements Count";
	constexpr const char* INPUT_ELEMENTS = "Input Elements";
} // namespace ChunkKey