#include "FTVertexShader.h"

#include "Renderer/FoxtrotRenderer.h"
#include "Renderer/D3D11Utils.h"
#include "Dynamic/RecordNode.h"

void FTVertexShader::Process(FoxtrotRenderer* renderer)
{
	LoadMetaData();
	CompileShader(renderer);
	FTResource::Process();
}

void FTVertexShader::CompileShader(FoxtrotRenderer* renderer)
{
	if (mSemanticsName.size() < 1)
		return;
	mShader.Reset();

	const wchar_t* fileName = GetRelativePath().WC_Str();
	DX::ThrowIfFailed(
		D3D11Utils::CreateVertexShaderAndInputLayout(
			renderer->GetDevice(),
			fileName,
			mInputElements,
			mShader,
			mInputLayout));
	delete[] fileName;
}

void FTVertexShader::RegisterInputElementDesc(const char* semanticName, UINT& offset)
{
	D3D11_INPUT_ELEMENT_DESC desc;
	if (FTDS::StringEqual(semanticName, "TEXCOORD"))
	{
		desc = { semanticName, 0, DXGI_FORMAT_R32G32_FLOAT, 0, offset, D3D11_INPUT_PER_VERTEX_DATA, 0 };
		if (!mIsSpine)
			offset += 4 * 2;
	}
	else if (FTDS::StringEqual(semanticName, "POSITION_2D"))
	{
		desc = { "POSITION", 0, DXGI_FORMAT_R32G32_FLOAT, 0, offset, D3D11_INPUT_PER_VERTEX_DATA, 0 };
		if (!mIsSpine)
			offset += 4 * 2;
	}
	else
	{
		desc = { semanticName, 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, offset, D3D11_INPUT_PER_VERTEX_DATA, 0 };
		if (!mIsSpine)
			offset += 4 * 3;
	}

	mInputElements.push_back(desc);
}

FTVertexShader::FTVertexShader(FTResourceDef& resDef, FoxtrotRenderer* renderer)
	: FTShader(resDef, renderer)
	, mIsSpine(false)
#ifdef FOXTROT_EDITOR
	, mSemanticsInclusion(DBG_NEW FTDS::HashMap<bool>(5))
#endif // FOXTROT_EDITOR
{

#ifdef FOXTROT_EDITOR
	FTDS::String semantics[5] = { "POSITION", "POSITION_2D", "NORMAL", "COLOR", "TEXCOORD" };
	for (size_t i = 0; i < 5; ++i)
	{
		if (!mSemanticsInclusion->At(semantics[i]))
			mSemanticsInclusion->Insert(semantics[i], false);
	}

#endif // FOXTROT_EDITOR

	SetType(ShaderType::VERTEX_SHADER);
	Process(renderer);
}

FTVertexShader::~FTVertexShader()
{
#ifdef FOXTROT_EDITOR
	delete mSemanticsInclusion;
#endif
}

void FTVertexShader::SaveProperties(std::ofstream& ofs)
{
	FileIOHelper::BeginDataPackSave(ofs, ChunkKey::FTVertexShader::FT_VERTEX_SHADER);
	FTResource::SaveProperties(ofs);

	for (size_t i = 0; i < mSemanticsName.size(); ++i)
	{
		FTDS::String key = FTDS::String(ChunkKey::FTVertexShader::INPUT_ELEMENTS);
		key.Append(" ");
		key.Append(std::to_string(i).c_str());
		FileIOHelper::SaveString(ofs, key.C_Str(), mSemanticsName.at(i));
	}

	FileIOHelper::SaveSize(ofs, ChunkKey::FTVertexShader::INPUT_ELEMENTS_COUNT, mSemanticsName.size());

	FileIOHelper::SaveBool(ofs, ChunkKey::FTVertexShader::IS_SPINE_SHADER, mIsSpine);

	FileIOHelper::EndDataPackSave(ofs, ChunkKey::FTVertexShader::FT_VERTEX_SHADER);
}

void FTVertexShader::LoadProperties(std::ifstream& ifs)
{
	FileIOHelper::BeginDataPackLoad(ifs, ChunkKey::FTVertexShader::FT_VERTEX_SHADER);

	FileIOHelper::LoadBool(ifs, mIsSpine);

	size_t count = 0;
	FileIOHelper::LoadSize(ifs, count);

	for (UINT i = 0; i < count; ++i)
	{
		FTDS::String semanticN = {};
		FileIOHelper::LoadBasicString(ifs, semanticN);
		mSemanticsName.push_back(semanticN);
	}
	std::reverse(mSemanticsName.begin(), mSemanticsName.end());

	UINT offset = 0;
	for (FTDS::String& str : mSemanticsName)
	{
		RegisterInputElementDesc(str.C_Str(), offset);

#ifdef FOXTROT_EDITOR
		mSemanticsInclusion->At(str)->Value() = true;

#endif // FOXTROT_EDITOR
	}

	if (mIsSpine)
	{
		for (size_t i = 0; i < mInputElements.size(); ++i)
			mInputElements.at(i).InputSlot = i;
	}

	return FTResource::LoadProperties(ifs);
}

ComPtr<ID3D11VertexShader>& FTVertexShader::GetShader() { return mShader; }
ComPtr<ID3D11InputLayout>&	FTVertexShader::GetInputLayout() { return mInputLayout; }

#ifdef FOXTROT_EDITOR
void FTVertexShader::UpdateUI()
{
	ImGui::SeparatorText("Input Elements");
	FTDS::String semantics[5] = { "POSITION", "POSITION_2D", "NORMAL", "COLOR", "TEXCOORD" };

	for (size_t i = 0; i < 5; ++i)
	{
		bool val = mSemanticsInclusion->At(semantics[i])->Value();
		CommandHistory::GetInstance()->UpdateBoolValue(semantics[i].C_Str(), val);
		mSemanticsInclusion->At(semantics[i])->Value() = val;
	}

	size_t i = 0;
	printf("--------------------------------\n");
	mSemanticsInclusion->IterateAllValues([&](bool bl) {
		printf(semantics[i].C_Str());
		printf(" ");
		LogBool(bl);
		++i;
	});
	printf("--------------------------------\n");

	CommandHistory::GetInstance()->UpdateBoolValue("Is Spine shader", mIsSpine);

	if (ImGui::Button("Update Input Elements"))
	{
		mSemanticsName.clear();
		mInputElements.clear();

		for (size_t i = 0; i < 5; ++i)
		{
			FTDS::String& key = semantics[i];
			if (mSemanticsInclusion->At(key)->Value())
				mSemanticsName.push_back(key);
		}

		static UINT offset = 0;
		for (FTDS::String& str : mSemanticsName)
			RegisterInputElementDesc(str.C_Str(), offset);

		if (mIsSpine)
		{
			for (size_t i = 0; i < mInputElements.size(); ++i)
				mInputElements.at(i).InputSlot = i;
		}

		SaveMetaData();
	}
}
#endif