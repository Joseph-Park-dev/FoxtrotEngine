#include "ResourceSystem/Shader/FTVertexShader.h"

#include "Renderer/D3D11Renderer.h"
#include "Utility/D3D11Utils.h"
#include "FileSystem/FileIOHelper.h"
#include "Manager/ResourceManager.h"
#include "FTDS/Dynamic/RecordNode.h"

namespace D3D11
{
	using namespace Common;
	using namespace Core;
	ResType FTVertexShader::Type = ResType::VERTEX_SHADER;

	void FTVertexShader::CompileShader(Common::FTResourceDef& resDef, D3D11Renderer* renderer)
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

		Common::FTDS::String path	   = resDef.Path;
		const wchar_t*		 wcharPath = path.WC_Str();
		DX::ThrowIfFailed(
			D3D11Utils::CreateVertexShaderAndInputLayout(
				renderer->GetDevice(),
				wcharPath,
				inputDesc,
				mSemanticItems->GetSize(),
				mShader,
				mInputLayout));
		delete[] wcharPath;
		delete[] inputDesc;
	}

	FTVertexShader::FTVertexShader(Common::FTResourceDef& resDef, D3D11Renderer* renderer)
		: D3D11::FTShader(resDef)
		, mShader(nullptr)
		, mInputLayout(nullptr)
#ifdef FOXTROT_EDITOR
		, mSemanticItems(DBG_NEW Common::FTDS::DynamicArray<SemanticItem*>())
#endif // FOXTROT_EDITOR
	{
		SetType(ShaderType::VERTEX_SHADER);
		LoadMetaData(resDef);
		CompileShader(resDef, renderer);
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
		D3D11Resource::SaveProperties(ofs);

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

		D3D11Resource::LoadProperties(ifs);
	}

	Microsoft::WRL::ComPtr<ID3D11VertexShader>& FTVertexShader::GetShader() { return mShader; }
	Microsoft::WRL::ComPtr<ID3D11InputLayout>&	FTVertexShader::GetInputLayout() { return mInputLayout; }

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

	void SemanticItem::SaveProperties(std::ofstream& ofs)
	{
		FileIOHelper::SaveString(ofs, ChunkKey::FTVertexShader::SEMANTIC_NAME, Name);
		FileIOHelper::SaveUnsignedInt(ofs, ChunkKey::FTVertexShader::SEMANTIC_INDEX, Desc.SemanticIndex);
		FileIOHelper::SaveInt(ofs, ChunkKey::FTVertexShader::FORMAT, Desc.Format);
		FileIOHelper::SaveUnsignedInt(ofs, ChunkKey::FTVertexShader::INPUT_SLOT, Desc.InputSlot);
		FileIOHelper::SaveUnsignedInt(ofs, ChunkKey::FTVertexShader::OFFSET, Desc.AlignedByteOffset);
		FileIOHelper::SaveInt(ofs, ChunkKey::FTVertexShader::INPUT_SLOT_CLASS, Desc.InputSlotClass);
		FileIOHelper::SaveUnsignedInt(ofs, ChunkKey::FTVertexShader::INSTANCE_DATA_STEP_RATE, Desc.InstanceDataStepRate);
	}

	void SemanticItem::LoadProperties(std::ifstream& ifs)
	{
		FileIOHelper::LoadUnsignedInt(ifs, Desc.InstanceDataStepRate);

		int val = 0;
		FileIOHelper::LoadInt(ifs, val);
		Desc.InputSlotClass = static_cast<D3D11_INPUT_CLASSIFICATION>(val);

		FileIOHelper::LoadUnsignedInt(ifs, Desc.AlignedByteOffset);
		FileIOHelper::LoadUnsignedInt(ifs, Desc.InputSlot);

		FileIOHelper::LoadInt(ifs, val);
		Desc.Format = static_cast<DXGI_FORMAT>(val);

		FileIOHelper::LoadUnsignedInt(ifs, Desc.SemanticIndex);

		FileIOHelper::LoadBasicString(ifs, Name);
	}
} // namespace D3D11