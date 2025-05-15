#pragma once
#include "ResourceSystem/FTShaders/FTShader.h"

class FTPixelShader :
	public FTShader
{
public:
	// You may use this overriden menber function.
	void CompileShader(FoxtrotRenderer* renderer) override;

public:
	ComPtr<ID3D11PixelShader>& GetShader();

public:
	FTPixelShader();

private:
	ComPtr<ID3D11PixelShader> mShader;

public:
	virtual void SaveProperties(std::ofstream& ofs) override;
	virtual void LoadProperties(std::ifstream& ifs) override;

#ifdef FOXTROT_EDITOR
public:
	virtual void UpdateUI() override;
#endif
};

namespace ChunkKey
{
	constexpr const char* FT_PIXEL_SHADER = "FTPixelShader";
}