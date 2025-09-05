// ----------------------------------------------------------------
// Foxtrot Engine 2D
// Copyright (C) 2025 JungBae Park. All rights reserved.
//
// Released under the GNU General Public License v3.0
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

#pragma once
#include <ResourceSystem/FTShaders/FTShader.h>

#include <vector>

#include <Static/HashMap.h>

/// @brief A wrapper for HLSL vertex shaders.
class FTVertexShader : public FTShader
{
public:
	/// @see FTResource::SaveProperties()
	virtual void SaveProperties(std::ofstream& ofs) override;

	/// @see FTResource::LoadProperties()
	virtual void LoadProperties(std::ifstream& ifs) override;

public:
	/// @brief Returns compiled HLSL vertex shader object.
	ComPtr<ID3D11VertexShader>& GetShader();

	/// @brief Returns the current input layout.
	ComPtr<ID3D11InputLayout>& GetInputLayout();

public:
	/// @see FTShader::FTShader()
	FTVertexShader(FTResourceDef& resDef, FoxtrotRenderer* renderer);
	~FTVertexShader();

protected:
	/// @brief Loads meta data, and compiles the shader.
	void Process(FoxtrotRenderer* renderer) override;

	/// @see FTShader::CompileShader()
	void CompileShader(FoxtrotRenderer* renderer) override;

private:
	/// @brief The shader should remain compiled after initialization.
	ComPtr<ID3D11VertexShader> mShader;

	ComPtr<ID3D11InputLayout> mInputLayout;

	/// @brief Input elements use to compile vertex shader.
	std::vector<D3D11_INPUT_ELEMENT_DESC> mInputElements;

	/// @brief These strings will be combined with D3D11_INPUT_ELEMENT_DESC when compiling the vertex shader.
	std::vector<FTDS::String> mSemanticsName;

	/// @brief Is this vertex shader used for rendering spine animation?
	/// This should be defined due to the difference in rendering between Mesh and SpineMesh.
	bool mIsSpine;

private:
	/// @brief Resgister new D3D11_INPUT_ELEMENT_DESC to mInputElements
	void RegisterInputElementDesc(const char* semanticName, UINT& offset);

#ifdef FOXTROT_EDITOR
public:
	void UpdateUI() override;

private:
	/// @brief Used to register input elements with GUI.
	FTDS::HashMap<bool>* mSemanticsInclusion;
#endif
};

namespace ChunkKey
{
	namespace FTVertexShader
	{
		constexpr const char* FT_VERTEX_SHADER	   = "FTVertexShader";
		constexpr const char* INPUT_ELEMENTS_COUNT = "Input Elements Count";
		constexpr const char* INPUT_ELEMENTS	   = "Input Elements";
		constexpr const char* IS_SPINE_SHADER	   = "Is Spine Shader";

	} // namespace FTVertexShader
} // namespace ChunkKey