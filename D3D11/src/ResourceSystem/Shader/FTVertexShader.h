// ----------------------------------------------------------------
// Foxtrot Engine 2D
// Copyright (C) 2025 JungBae Park. All rights reserved.
//
// Released under the GNU General Public License v3.0
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

#pragma once
#include "ResourceSystem/Shader/FTShader.h"

#include <d3d11.h>
#include <wrl.h>

#include "Static/HashMap.h"
#include "Dynamic/DynamicArray.h"
#include "FileSystem/NullKeys.h"

#ifdef FOXTROT_EDITOR
	#include <magic_enum/include/magic_enum/magic_enum.hpp>
	#include "CommandHistory.h"
#endif // FOXTROT_EDITOR

namespace D3D11
{
	class D3D11Renderer;
	struct SemanticItem;

	/// @brief A wrapper for HLSL vertex shaders.
	class FTVertexShader : public FTShader
	{
	public:
		static D3D11::ResType Type;

	public:
		/// @see FTResource::SaveProperties()
		virtual void SaveProperties(std::ofstream& ofs) override;

		/// @see FTResource::LoadProperties()
		virtual void LoadProperties(std::ifstream& ifs) override;

	public:
		/// @brief Returns compiled HLSL vertex shader object.
		Microsoft::WRL::ComPtr<ID3D11VertexShader>& GetShader();

		/// @brief Returns the current input layout.
		Microsoft::WRL::ComPtr<ID3D11InputLayout>& GetInputLayout();

	public:
		/// @see FTShader::FTShader()
		FTVertexShader(Core::FTResourceDef& resDef, D3D11Renderer* renderer);
		~FTVertexShader() override;

	protected:
		/// @see FTShader::CompileShader()
		void CompileShader(Core::FTResourceDef& resDef, D3D11Renderer* renderer) override;

	private:
		/// @brief The shader should remain compiled after initialization.
		Microsoft::WRL::ComPtr<ID3D11VertexShader> mShader;
		Microsoft::WRL::ComPtr<ID3D11InputLayout>  mInputLayout;
		Core::FTDS::DynamicArray<SemanticItem*>*   mSemanticItems;

#ifdef FOXTROT_EDITOR
	public:
		void UpdateUI() override;
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
		Core::FTDS::String		 Name;
		D3D11_INPUT_ELEMENT_DESC Desc;

		SemanticItem()
			: NameIdx(0)
			, Desc()
		{
			Desc.SemanticName		  = Core::ChunkKey::NullVal::NULL_OBJECT;
			Desc.SemanticIndex		  = 0;
			Desc.InputSlot			  = 0;
			Desc.AlignedByteOffset	  = 0;
			Desc.InstanceDataStepRate = 0;
		}

		void SaveProperties(::std::ofstream& ofs);
		void LoadProperties(::std::ifstream& ifs);

#ifdef FOXTROT_EDITOR
		void UpdateUI()
		{
			static FTDS::String names[5] = { "POSITION", "NORMAL", "COLOR", "TEXCOORD", "PSIZE" };

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

			UINT format = static_cast<UINT>(Desc.Format);
			CommandHistory::GetInstance()->UpdateUnsignedIntValue("DXGI_FORMAT", format);
			Desc.Format = static_cast<DXGI_FORMAT>(format);
			ImGui::Text("Current format : %s", magic_enum::enum_name(Desc.Format).data());

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
} // namespace D3D11