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
#include <d3d11.h>

#include <Static/HashMap.h>
#include <FileSystem/FileIOHelper.h>

struct SemanticItem;

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
	ComPtr<ID3D11InputLayout>  mInputLayout;

private:
	/// @brief Resgister new D3D11_INPUT_ELEMENT_DESC to mInputElements
	void RegisterInputElementDesc(const char* semanticName, SemanticItem* item, UINT& offset);

#ifdef FOXTROT_EDITOR
public:
	void UpdateUI() override;

private:
	/// @brief Used to register input elements with GUI.
	FTDS::DynamicArray<SemanticItem*>* mSemanticItems;
#endif
};

namespace ChunkKey
{
	namespace FTVertexShader
	{
		constexpr const char* FT_VERTEX_SHADER	   = "FTVertexShader";
		constexpr const char* INPUT_ELEMENTS_COUNT = "Input Elements Count";
		constexpr const char* INPUT_ELEMENTS	   = "Input Elements";

		constexpr const char* SEMANTIC_NAME			  = "SemanticName";
		constexpr const char* SEMANTIC_INDEX		  = "SemanticIndex";
		constexpr const char* FORMAT				  = "Format";
		constexpr const char* INPUT_SLOT			  = "InputSlot";
		constexpr const char* OFFSET				  = "AlignedByteOffset";
		constexpr const char* INPUT_SLOT_CLASS		  = "InputSlotClass";
		constexpr const char* INSTANCE_DATA_STEP_RATE = "InstanceDataStepRate";

	} // namespace FTVertexShader
} // namespace ChunkKey

struct SemanticItem
{
	size_t					 NameIdx;
	FTDS::String			 Name;
	D3D11_INPUT_ELEMENT_DESC Desc;

	SemanticItem()
		: NameIdx(0)
		, Desc()
	{
		Desc.SemanticName		  = ChunkKey::NullVal::NULL_OBJECT;
		Desc.SemanticIndex		  = 0;
		Desc.InputSlot			  = 0;
		Desc.AlignedByteOffset	  = 0;
		Desc.InstanceDataStepRate = 0;
	}

	void SaveProperties(std::ofstream& ofs)
	{
		FileIOHelper::SaveString(ofs, ChunkKey::FTVertexShader::SEMANTIC_NAME, Name);
		FileIOHelper::SaveUnsignedInt(ofs, ChunkKey::FTVertexShader::SEMANTIC_INDEX, Desc.SemanticIndex);
		FileIOHelper::SaveInt(ofs, ChunkKey::FTVertexShader::FORMAT, Desc.Format);
		FileIOHelper::SaveUnsignedInt(ofs, ChunkKey::FTVertexShader::INPUT_SLOT, Desc.InputSlot);
		FileIOHelper::SaveUnsignedInt(ofs, ChunkKey::FTVertexShader::OFFSET, Desc.AlignedByteOffset);
		FileIOHelper::SaveInt(ofs, ChunkKey::FTVertexShader::INPUT_SLOT_CLASS, Desc.InputSlotClass);
		FileIOHelper::SaveUnsignedInt(ofs, ChunkKey::FTVertexShader::INSTANCE_DATA_STEP_RATE, Desc.InstanceDataStepRate);
	}

	void LoadProperties(std::ifstream& ifs)
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

#ifdef FOXTROT_EDITOR
	void UpdateUI()
	{
		static FTDS::String names[5] = { "POSITION", "POSITION_2D", "NORMAL", "COLOR", "TEXCOORD" };

		static FTDS::String formats[19] = { "DXGI_FORMAT_UNKNOWN",
											"DXGI_FORMAT_R32G32B32A32_TYPELESS",
											"DXGI_FORMAT_R32G32B32A32_FLOAT",
											"DXGI_FORMAT_R32G32B32A32_UINT",
											"DXGI_FORMAT_R32G32B32A32_SINT",
											"DXGI_FORMAT_R32G32B32_TYPELESS",
											"DXGI_FORMAT_R32G32B32_FLOAT",
											"DXGI_FORMAT_R32G32B32_UINT",
											"DXGI_FORMAT_R32G32B32_SINT",
											"DXGI_FORMAT_R16G16B16A16_TYPELESS",
											"DXGI_FORMAT_R16G16B16A16_FLOAT",
											"DXGI_FORMAT_R16G16B16A16_UNORM",
											"DXGI_FORMAT_R16G16B16A16_UINT",
											"DXGI_FORMAT_R16G16B16A16_SNORM",
											"DXGI_FORMAT_R16G16B16A16_SINT",
											"DXGI_FORMAT_R32G32_TYPELESS",
											"DXGI_FORMAT_R32G32_FLOAT",
											"DXGI_FORMAT_R32G32_UINT",
											"DXGI_FORMAT_R32G32_SINT" };

		static FTDS::String classifications[2] = {
			"D3D11_INPUT_PER_VERTEX_DATA",
			"D3D11_INPUT_PER_INSTANCE_DATA"
		};

		if (ImGui::BeginCombo("Name", Name.C_Str()))
		{
			for (size_t n = 0; n < IM_ARRAYSIZE(names); ++n)
			{
				bool isSelected = (NameIdx == n);
				if (ImGui::Selectable(names[n].C_Str(), isSelected))
				{
					NameIdx = n;
					Name	= names[NameIdx];
				}

				if (isSelected)
					ImGui::SetItemDefaultFocus();
			}
			ImGui::EndCombo();
		}

		CommandHistory::GetInstance()->UpdateUnsignedIntValue("Semantic Index", Desc.SemanticIndex);

		if (ImGui::BeginCombo("Format", formats[Desc.Format].C_Str()))
		{
			for (size_t n = 0; n < IM_ARRAYSIZE(formats); ++n)
			{
				bool isSelected = (Desc.Format == n);
				if (ImGui::Selectable(formats[n].C_Str(), isSelected))
					Desc.Format = static_cast<DXGI_FORMAT>(n);

				if (isSelected)
					ImGui::SetItemDefaultFocus();
			}
			ImGui::EndCombo();
		}

		CommandHistory::GetInstance()->UpdateUnsignedIntValue("Input Slot", Desc.InputSlot);
		CommandHistory::GetInstance()->UpdateUnsignedIntValue("Offset", Desc.AlignedByteOffset);

		if (ImGui::BeginCombo("Classification", classifications[Desc.InputSlotClass].C_Str()))
		{
			for (size_t n = 0; n < IM_ARRAYSIZE(classifications); ++n)
			{
				bool isSelected = (Desc.InputSlotClass == n);
				if (ImGui::Selectable(classifications[n].C_Str(), isSelected))
					Desc.InputSlotClass = static_cast<D3D11_INPUT_CLASSIFICATION>(n);

				if (isSelected)
					ImGui::SetItemDefaultFocus();
			}
			ImGui::EndCombo();
		}

		CommandHistory::GetInstance()->UpdateUnsignedIntValue("Instance Data Step Rate", Desc.InstanceDataStepRate);
	}
#endif // FOXTROT_EDITOR
};