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
		mInputLayout
	);
}

ComPtr<ID3D11VertexShader>& FTVertexShader::GetShader() { return mShader; }

void FTVertexShader::RegisterInputElementDesc(const char* semanticName, D3D11_INPUT_ELEMENT_DESC& desc)
{
	mSemanticNames.push_back(semanticName);
	mInputElements.push_back(desc);
}

FTVertexShader::FTVertexShader()
	: FTShader()
#ifdef FOXTROT_EDITOR
	, mSemanticsInclusion{ false, false, false, false }
#endif // FOXTROT_EDITOR
{
	SetType(ShaderType::VERTEX_SHADER);
}

void FTVertexShader::SaveProperties(std::ofstream& ofs)
{
	FileIOHelper::BeginDataPackSave(ofs, ChunkKey::FT_VERTEX_SHADER);
	FTResource::SaveProperties(ofs);

	for (size_t i = 0; i < mSemanticNames.size(); ++i)
		FileIOHelper::SaveString(ofs, ChunkKey::INPUT_ELEMENTS + i, mSemanticNames.at(i));

	FileIOHelper::SaveSize(ofs, ChunkKey::INPUT_ELEMENTS_COUNT, mSemanticNames.size());

	FileIOHelper::EndDataPackSave(ofs, ChunkKey::FT_VERTEX_SHADER);
}

void FTVertexShader::LoadProperties(std::ifstream& ifs)
{
	FileIOHelper::BeginDataPackLoad(ifs, ChunkKey::FT_VERTEX_SHADER);

	size_t count = 0;
	FileIOHelper::LoadSize(ifs, count);

	for (size_t i = 0; i < count; ++i)
	{
		std::string semanticN = {};
		FileIOHelper::LoadBasicString(ifs, semanticN);

		D3D11_INPUT_ELEMENT_DESC desc;
		if (semanticN == "TEXCOORD")
		{
			desc = { semanticN.c_str(), 0, DXGI_FORMAT_R32G32_FLOAT, 0, 4 * 3 * (UINT)i, D3D11_INPUT_PER_VERTEX_DATA, 0 };
		}
		else
		{
			desc = { semanticN.c_str(), 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 4 * 3 * (UINT)i, D3D11_INPUT_PER_VERTEX_DATA, 0 };
		}
		RegisterInputElementDesc(semanticN.c_str(), desc);
	}
	return LoadProperties(ifs);
}

#ifdef FOXTROT_EDITOR
void FTVertexShader::UpdateUI()
{
	ImGui::SeparatorText("Input Elements");
	const char* semantics[4] = { "POSITION", "NORMAL", "COLOR", "TEXCOORD" };

	for (size_t i = 0; i < 4; ++i)
		CommandHistory::GetInstance()->UpdateBoolValue(semantics[i], mSemanticsInclusion[i]);

	if (ImGui::Button("Update Input Elements"))
	{
		mSemanticNames.clear();
		for (size_t i = 0; i < 4; ++i)
		{
			if (mSemanticsInclusion[i])
				mSemanticNames.push_back(semantics[i]);
		}
		mShader.Reset();
		CompileShader(GetRenderer());
	}
}
#endif