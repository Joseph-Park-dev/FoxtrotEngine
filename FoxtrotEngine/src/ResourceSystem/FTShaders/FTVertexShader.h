#pragma once
#include "ResourceSystem/FTShaders/FTShader.h"

#include <vector>

#include "Static/HashMap.h"

class FTVertexShader : public FTShader
{
public:
	// You may use this overriden menber function.
	void CompileShader(FoxtrotRenderer* renderer) override;

public:
	ComPtr<ID3D11VertexShader>& GetShader();
	ComPtr<ID3D11InputLayout>&	GetInputLayout();

	void RegisterInputElementDesc(const char* semanticName, UINT& offset);

public:
	FTVertexShader();
	~FTVertexShader();

private:
	ComPtr<ID3D11VertexShader> mShader;
	ComPtr<ID3D11InputLayout>  mInputLayout;

	std::vector<D3D11_INPUT_ELEMENT_DESC> mInputElements;
	std::vector<FTDS::String>			  mSemanticsName;

public:
	virtual void SaveProperties(std::ofstream& ofs) override;
	virtual void LoadProperties(std::ifstream& ifs) override;

#ifdef FOXTROT_EDITOR
public:
	void UpdateUI() override;

private:
	FTDS::HashMap<bool>* mSemanticsInclusion;
#endif
};

namespace ChunkKey
{
	constexpr const char* FT_VERTEX_SHADER	   = "FTVertexShader";
	constexpr const char* INPUT_ELEMENTS_COUNT = "Input Elements Count";
	constexpr const char* INPUT_ELEMENTS	   = "Input Elements";
} // namespace ChunkKey