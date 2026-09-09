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

#include "FTDS/Static/HashMap.h"
#include "FTDS/Dynamic/DynamicArray.h"
#include "FileSystem/NullKeys.h"

#ifdef FOXTROT_EDITOR
	#define IMGUI_DEFINE_MATH_OPERATORS
	#include <imgui.h>
	#include <magic_enum/magic_enum.hpp>
	#include "CommandHistory.h"
	#include "Utility/EditorHelper.h"
#endif // FOXTROT_EDITOR

namespace D3D11
{
	using namespace Common;
	class D3D11Renderer;
	struct SemanticItem;

	/// @brief A wrapper for HLSL vertex shaders.
	class FTVertexShader : public FTShader
	{
	public:
		static D3D11::ResType Type;

	public:
		/// @brief Serializes this object's persistent properties to a .chunk stream.
		/// @see FTResource::SaveProperties()
		/// @param ofs Output stream receiving the serialized data.
		/// @note Writes to the supplied stream at its current position.
		virtual void SaveProperties(std::ofstream& ofs) override;

		/// @brief Restores this object's persistent properties from a .chunk stream.
		/// @see FTResource::LoadProperties()
		/// @param ifs Input stream positioned at the expected data; reading advances its position.
		/// @note Advances the stream position and updates the destination state.
		virtual void LoadProperties(std::ifstream& ifs) override;

	public:
		/// @brief Returns compiled HLSL vertex shader object.
		/// @return Borrowed access to the shader.
		/// @note Changes through the returned reference affect this object's stored state.
		Microsoft::WRL::ComPtr<ID3D11VertexShader>& GetShader();

		/// @brief Returns the current input layout.
		/// @return Borrowed access to the input layout.
		/// @note Changes through the returned reference affect this object's stored state.
		Microsoft::WRL::ComPtr<ID3D11InputLayout>& GetInputLayout();

	public:
		/// @brief Initializes a vertex shader and its input-layout metadata.
		/// @see FTShader::FTShader()
		/// @param resDef Resource definition containing the filename and source path.
		/// @param renderer Renderer providing the graphics device and current render state.
		FTVertexShader(Common::FTResourceDef& resDef, void* renderer);
		/// @brief Releases the resources managed by this instance during destruction.
		~FTVertexShader() override;

	protected:
		/// @brief Compiles shader source into bytecode using the shader's configured entry point and profile.
		/// @see FTShader::CompileShader()
		/// @param resDef Resource definition containing the filename and source path.
		/// @param renderer Renderer providing the graphics device and current render state.
		/// @throws D3D11::DX::com_exception If a checked Direct3D operation fails.
		void CompileShader(Common::FTResourceDef& resDef, D3D11Renderer* renderer) override;

	private:
		/// @brief The shader should remain compiled after initialization.
		Microsoft::WRL::ComPtr<ID3D11VertexShader> mShader;
		Microsoft::WRL::ComPtr<ID3D11InputLayout>  mInputLayout;
		Common::FTDS::DynamicArray<SemanticItem*>* mSemanticItems;

#ifdef FOXTROT_EDITOR
	public:
		/// @brief Builds the editor controls for inspecting and modifying this object's state.
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
		Common::FTDS::String	 Name;
		D3D11_INPUT_ELEMENT_DESC Desc;

		/// @brief Initializes one vertex input semantic and its layout metadata.
		/// @note Initializes the NameIdx base or delegates to its constructor.
		SemanticItem()
			: NameIdx(0)
			, Desc()
		{
			Desc.SemanticName		  = Common::ChunkKey::NullVal::NULL_OBJECT;
			Desc.SemanticIndex		  = 0;
			Desc.InputSlot			  = 0;
			Desc.AlignedByteOffset	  = 0;
			Desc.InstanceDataStepRate = 0;
		}

		/// @brief Serializes this object's persistent properties to a .chunk stream.
		/// @param ofs Output stream receiving the serialized data.
		/// @note Writes to the supplied stream at its current position.
		void SaveProperties(::std::ofstream& ofs);
		/// @brief Restores this object's persistent properties from a .chunk stream.
		/// @param ifs Input stream positioned at the expected data; reading advances its position.
		/// @note Advances the stream position and updates the destination state.
		void LoadProperties(::std::ifstream& ifs);

#ifdef FOXTROT_EDITOR
		/// @brief Builds the editor controls for inspecting and modifying this object's state.
		void UpdateUI()
		{
			static Common::FTDS::String names[5] = { "POSITION", "NORMAL", "COLOR", "TEXCOORD", "PSIZE" };

			static Common::FTDS::String classifications[2] = {
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

			Editor::UPDATE_UNSIGNED_INT("Semantic Index", Desc.SemanticIndex);

			UINT format = static_cast<UINT>(Desc.Format);
			Editor::UPDATE_UNSIGNED_INT("DXGI_FORMAT", format);
			Desc.Format = static_cast<DXGI_FORMAT>(format);
			ImGui::Text("Current format : %s", magic_enum::enum_name(Desc.Format).data());

			Editor::UPDATE_UNSIGNED_INT("Input Slot", Desc.InputSlot);
			Editor::UPDATE_UNSIGNED_INT("Offset", Desc.AlignedByteOffset);

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

			Editor::UPDATE_UNSIGNED_INT("Instance Data Step Rate", Desc.InstanceDataStepRate);
		}
#endif // FOXTROT_EDITOR
	};
} // namespace D3D11
