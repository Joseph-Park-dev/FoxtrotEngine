#include "FTVertexShader.h"

#include "Renderer/FoxtrotRenderer.h"
#include "Renderer/D3D11Utils.h"
#include "Dynamic/RecordNode.h"

void FTVertexShader::CompileShader(FoxtrotRenderer* renderer)
{
	if (mSemanticsName.size() < 1)
		return;
	mShader.Reset();

	const wchar_t* fileName = RelativePath().WC_Str();
	DX::ThrowIfFailed(
		D3D11Utils::CreateVertexShaderAndInputLayout(
			renderer->GetDevice(),
			fileName,
			mInputElements,
			mShader,
			mInputLayout));
	delete[] fileName;
}

ComPtr<ID3D11VertexShader>& FTVertexShader::GetShader() { return mShader; }
ComPtr<ID3D11InputLayout>& FTVertexShader::GetInputLayout() { return mInputLayout; }

void FTVertexShader::RegisterInputElementDesc(const char* semanticName, UINT& offset)
{
	D3D11_INPUT_ELEMENT_DESC desc;
	if (FTDS::StringEqual(semanticName, "TEXCOORD"))
	{
		desc = { semanticName, 0, DXGI_FORMAT_R32G32_FLOAT, 0, offset, D3D11_INPUT_PER_VERTEX_DATA, 0 };
		offset += 4 * 2;
	}
	else
	{
		desc = { semanticName, 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, offset, D3D11_INPUT_PER_VERTEX_DATA, 0 };
		offset += 4 * 3;
	}

	mInputElements.push_back(desc);
}

FTVertexShader::FTVertexShader()
	: FTShader()
	, mSemanticsInclusion(DBG_NEW FTDS::HashChainMap<bool>(4))
{
	SetType(ShaderType::VERTEX_SHADER);
}

FTVertexShader::~FTVertexShader()
{
	delete mSemanticsInclusion;
}

void FTVertexShader::SaveProperties(std::ofstream& ofs)
{
	FileIOHelper::BeginDataPackSave(ofs, ChunkKey::FT_VERTEX_SHADER);
	FTResource::SaveProperties(ofs);

	for (size_t i = 0; i < mSemanticsName.size(); ++i)
	{
		FTDS::String key = FTDS::String(ChunkKey::INPUT_ELEMENTS);
		key.Append(" ");
		key.Append(std::to_string(i).c_str());
		FileIOHelper::SaveString(ofs, key.C_Str(), mSemanticsName.at(i));
	}

	FileIOHelper::SaveSize(ofs, ChunkKey::INPUT_ELEMENTS_COUNT, mSemanticsName.size());

	FileIOHelper::EndDataPackSave(ofs, ChunkKey::FT_VERTEX_SHADER);
}

void FTVertexShader::LoadProperties(std::ifstream& ifs)
{
	FileIOHelper::BeginDataPackLoad(ifs, ChunkKey::FT_VERTEX_SHADER);

	size_t count = 0;
	FileIOHelper::LoadSize(ifs, count);

	for (UINT i = 0; i < count; ++i)
	{
		FTDS::String semanticN = {};
		FileIOHelper::LoadBasicString(ifs, semanticN);
		mSemanticsName.push_back(semanticN);
	}
	std::reverse(mSemanticsName.begin(), mSemanticsName.end());
	
	static UINT offset = 0;
	for (FTDS::String& str : mSemanticsName)
		RegisterInputElementDesc(str.C_Str(), offset);

#ifdef FOXTROT_EDITOR
	FTDS::String semantics[4] = { "POSITION", "NORMAL", "COLOR", "TEXCOORD" };

	for (FTDS::String& semanticN : mSemanticsName)
		mSemanticsInclusion->Insert(semanticN, true);

	for (size_t i = 0; i < 4; ++i)
	{
		if (!mSemanticsInclusion->At(semantics[i]))
			mSemanticsInclusion->Insert(semantics[i], false);
	}
#endif // FOXTROT_EDITOR

	return FTResource::LoadProperties(ifs);
}

#ifdef FOXTROT_EDITOR
void FTVertexShader::UpdateUI()
{
	ImGui::SeparatorText("Input Elements");
	FTDS::String semantics[4] = { "POSITION", "NORMAL", "COLOR", "TEXCOORD" };

	for (size_t i = 0; i < 4; ++i)
	{
		if (!mSemanticsInclusion->At(semantics[i]))
			mSemanticsInclusion->Insert(semantics[i], false);

		bool val = mSemanticsInclusion->At(semantics[i])->Value();
		CommandHistory::GetInstance()->UpdateBoolValue(semantics[i].C_Str(), val);
		mSemanticsInclusion->At(semantics[i])->Value() = val;
	}

	if (ImGui::Button("Update Input Elements"))
	{
		mSemanticsName.clear();
		mInputElements.clear();

		for (size_t i = 0; i < 4; ++i)
		{
			FTDS::String& key = semantics[i];
			if (mSemanticsInclusion->At(key)->Value())
				mSemanticsName.push_back(key);
		}

		static UINT offset = 0;
		for (FTDS::String& str : mSemanticsName)
			RegisterInputElementDesc(str.C_Str(), offset);

		SaveMetaFile();
		CompileShader(GetRenderer());
	}
}
#endif