#include "ResourceSystem/Material/StandardMaterial.h"

#include "Manager/ResourceManager.h"
#include "ResourceSystem/Light.h"
#include "Utility/D3D11Utils.h"
#include "Renderer/D3D11Renderer.h"
#include "Renderer/Camera.h"
#include "ResourceSystem/Mesh/Mesh.h"
#include "Debugging/DebugFuncs.h"

#ifdef FOXTROT_EDITOR
	#include "EditorUtils.h"
	#include "CommandHistory.h"
	#include "Utility/EditorHelper.h"
#endif

namespace D3D11
{
	using namespace Common;
	using Microsoft::WRL::ComPtr;
	ResType StandardMaterial::Type = ResType::MATERIAL;

	/// @brief Creates the pixel-shader constant buffer used by this shader.
	/// @param device Direct3D device used to create GPU resources.
	void StandardMaterial::CreatePixelConstBuffer(ComPtr<ID3D11Device>& device)
	{
		D3D11Utils::CreateConstantBuffer(device, *mData, GetPCBuf());
	}

	/// @brief Uploads the current CPU data to its GPU buffer.
	/// @param context Context associated with this operation.
	void StandardMaterial::UpdateBuffer(ComPtr<ID3D11DeviceContext>& context)
	{
		/*for (size_t i = 0; i < Light::TYPE::END; ++i)
		{
			if (LightManager::GetInstance()->GetType(0) == (Light::TYPE)i)
				mData->Lights[i] = LightManager::GetInstance()->GetLight(0);
			else
				mData->Lights[i].Strength *= 0.0f;
		}*/

		D3D11Utils::UpdateBuffer(context, *mData, GetPCBuf());
	}

	/// @brief Initializes the standard material's shader and texture state.
	/// @param resDef Resource definition containing the filename and source path.
	/// @param renderer Renderer providing the graphics device and current render state.
	/// @note Initializes the :StandardMaterial base or delegates to its constructor.
	StandardMaterial::StandardMaterial(Common::FTResourceDef& resDef, D3D11Renderer* renderer)
		: FTMaterial(resDef)
		, mData(DBG_NEW StandardMatData)
	{
		CreatePixelConstBuffer(renderer->GetDevice());
	}

	/// @brief Releases the resources managed by this instance during destruction.
	StandardMaterial::~StandardMaterial()
	{
		delete mData;
	}

	/// @brief Serializes this object's persistent properties to a .chunk stream.
	/// @param ofs Output stream receiving the serialized data.
	/// @note Writes to the supplied stream at its current position.
	void StandardMaterial::SaveProperties(std::ofstream& ofs)
	{
		Common::FileIOHelper::BeginDataPackSave(ofs, ChunkKey::StandardMat::STANDARD_MAT);

		Common::FileIOHelper::SaveString(ofs, Common::ChunkKey::FTResource::FILE_NAME, this->GetFileName());
		Common::FileIOHelper::SaveBool(ofs, ChunkKey::StandardMat::USE_TEXTURE, mData->UseTexture);
		Common::FileIOHelper::SaveFloat(ofs, ChunkKey::StandardMat::ALPHA_TRIM, mData->AlphaTrim);
		Common::FileIOHelper::SaveVector4(ofs, ChunkKey::StandardMat::COLOR, mData->Color);

		Common::FileIOHelper::EndDataPackSave(ofs, ChunkKey::StandardMat::STANDARD_MAT);
	}

	/// @brief Restores this object's persistent properties from a .chunk stream.
	/// @param ifs Input stream positioned at the expected data; reading advances its position.
	/// @note Advances the stream position and updates the destination state.
	void StandardMaterial::LoadProperties(std::ifstream& ifs)
	{
		Common::FileIOHelper::BeginDataPackLoad(ifs, ChunkKey::StandardMat::STANDARD_MAT);

		Common::FTDS::String fileName;
		bool				 useTex;
		Common::FileIOHelper::LoadBool(ifs, useTex);
		Common::FileIOHelper::LoadFloat(ifs, mData->AlphaTrim);
		Common::FileIOHelper::LoadVector4(ifs, mData->Color);
		Common::FileIOHelper::LoadBasicString(ifs, fileName);

		mData->UseTexture = (uint32_t)useTex;
	}

#ifdef FOXTROT_EDITOR
	/// @brief Builds the editor controls for inspecting and modifying this object's state.
	void StandardMaterial::UpdateUI()
	{
		ImGui::SeparatorText("Standard Mat Data");

		bool useTex = mData->UseTexture;
		::Editor::UPDATE_BOOL(ChunkKey::StandardMat::USE_TEXTURE, useTex);
		mData->UseTexture = useTex;

		::Editor::UPDATE_FLOAT(ChunkKey::StandardMat::ALPHA_TRIM, mData->AlphaTrim);
		::Editor::UPDATE_VEC4(ChunkKey::StandardMat::COLOR, mData->Color);
	}
#endif
} // namespace D3D11
