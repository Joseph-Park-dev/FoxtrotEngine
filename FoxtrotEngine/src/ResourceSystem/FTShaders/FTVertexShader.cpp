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
	if (mSemanticItems->IsEmpty())
		return;

	D3D11_INPUT_ELEMENT_DESC* inputDesc = DBG_NEW D3D11_INPUT_ELEMENT_DESC[mSemanticItems->GetSize()];
	for (size_t i = 0; i < mSemanticItems->GetSize(); ++i)
	{
		inputDesc[i]			  = mSemanticItems->At(i)->Desc;
		inputDesc[i].SemanticName = mSemanticItems->At(i)->Name.C_Str();
	}

	mShader.Reset();

	const wchar_t* fileName = GetRelativePath().WC_Str();
	DX::ThrowIfFailed(
		D3D11Utils::CreateVertexShaderAndInputLayout(
			renderer->GetDevice(),
			fileName,
			inputDesc,
			mSemanticItems->GetSize(),
			mShader,
			mInputLayout));
	delete[] fileName;
	delete[] inputDesc;
}

FTVertexShader::FTVertexShader(FTResourceDef& resDef, FoxtrotRenderer* renderer)
	: FTShader(resDef, renderer)
	, mShader(nullptr)
	, mInputLayout(nullptr)
#ifdef FOXTROT_EDITOR
	, mSemanticItems(DBG_NEW FTDS::DynamicArray<SemanticItem*>())
#endif // FOXTROT_EDITOR
{
	SetType(ShaderType::VERTEX_SHADER);
	Process(renderer);
}

FTVertexShader::~FTVertexShader()
{
#ifdef FOXTROT_EDITOR
	mSemanticItems->IterateArray([](SemanticItem* item) {
		delete item;
		item = nullptr;
	});

	delete mSemanticItems;
#endif
}

void FTVertexShader::SaveProperties(std::ofstream& ofs)
{
	FileIOHelper::BeginDataPackSave(ofs, ChunkKey::FTVertexShader::FT_VERTEX_SHADER);
	FTResource::SaveProperties(ofs);

	FileIOHelper::BeginDataPackSave(ofs, ChunkKey::FTVertexShader::INPUT_ELEMENTS);

	for (auto iter = mSemanticItems->Begin(); iter != mSemanticItems->End(); ++iter)
	{
		FileIOHelper::BeginDataPackSave(ofs, (*iter)->Desc.SemanticName);
		(*iter)->SaveProperties(ofs);
		FileIOHelper::EndDataPackSave(ofs, (*iter)->Desc.SemanticName);
	}

	FileIOHelper::EndDataPackSave(ofs, ChunkKey::FTVertexShader::INPUT_ELEMENTS);

	FileIOHelper::EndDataPackSave(ofs, ChunkKey::FTVertexShader::FT_VERTEX_SHADER);
}

void FTVertexShader::LoadProperties(std::ifstream& ifs)
{
	FileIOHelper::BeginDataPackLoad(ifs, ChunkKey::FTVertexShader::FT_VERTEX_SHADER);

	size_t count = FileIOHelper::BeginDataPackLoad(ifs, ChunkKey::FTVertexShader::INPUT_ELEMENTS).first;
	mSemanticItems->Reserve(count);

	for (size_t i = 0; i < count; ++i)
	{
		FileIOHelper::BeginDataPackLoad(ifs).first;
		SemanticItem* item = DBG_NEW SemanticItem;
		item->LoadProperties(ifs);
		mSemanticItems->PushBack(item);
	}

	mSemanticItems->Reverse();

	FTResource::LoadProperties(ifs);
}

ComPtr<ID3D11VertexShader>& FTVertexShader::GetShader() { return mShader; }
ComPtr<ID3D11InputLayout>&	FTVertexShader::GetInputLayout() { return mInputLayout; }

#ifdef FOXTROT_EDITOR
void FTVertexShader::UpdateUI()
{
	ImGui::SeparatorText("Input Elements");
	if (ImGui::Button("Add Input Desc"))
		mSemanticItems->PushBack(DBG_NEW SemanticItem);

	for (size_t i = 0; i < mSemanticItems->GetSize(); ++i)
	{
		ImGui::PushID(mSemanticItems->At(i));

		if (ImGui::CollapsingHeader(mSemanticItems->At(i)->Name.C_Str()))
			mSemanticItems->At(i)->UpdateUI();

		if (ImGui::ArrowButton("##Up", ImGuiDir::ImGuiDir_Up))
		{
			if (0 < i)
				mSemanticItems->Swap(i - 1, i);
		}
		ImGui::SameLine();
		if (ImGui::ArrowButton("##Down", ImGuiDir::ImGuiDir_Down))
		{
			if (i < mSemanticItems->GetSize() - 1)
				mSemanticItems->Swap(i + 1, i);
		}

		if (ImGui::Button("Delete"))
		{
			delete mSemanticItems->At(i);
			mSemanticItems->At(i) = nullptr;
			mSemanticItems->Erase(i);
		}

		ImGui::PopID();
	}

	if (ImGui::Button("Update"))
		SaveMetaData();
}
#endif