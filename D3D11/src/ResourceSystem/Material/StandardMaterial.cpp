#include "StandardMaterial.h"

#include "Manager/ResourceManager.h"
#include "ResourceSystem/Light.h"
#include "Renderer/D3D11Utils.h"
#include "Renderer/D3D11Renderer.h"
#include "Renderer/Camera.h"
#include "ResourceSystem/Mesh/Mesh.h"
#include "Debugging/DebugFuncs.h"

#ifdef FOXTROT_EDITOR
	#include <EditorUtils.h>
	#include <CommandHistory.h>
	#include <EditorHelper.h>
#endif

namespace D3D11
{
	using Microsoft::WRL::ComPtr;
	ResType StandardMaterial::Type = ResType::MATERIAL;

	void StandardMaterial::CreatePixelConstBuffer(ComPtr<ID3D11Device>& device)
	{
		D3D11Utils::CreateConstantBuffer(device, *mData, GetPCBuf());
	}

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

	StandardMaterial::StandardMaterial(Core::FTResourceDef& resDef, D3D11Renderer* renderer)
		: FTMaterial(resDef)
		, mData(DBG_NEW StandardMatData)
	{
		CreatePixelConstBuffer(renderer->GetDevice());
	}

	StandardMaterial::~StandardMaterial()
	{
		delete mData;
	}

	void StandardMaterial::SaveProperties(std::ofstream& ofs)
	{
		Core::FileIOHelper::BeginDataPackSave(ofs, ChunkKey::StandardMat::STANDARD_MAT);

		Core::FileIOHelper::SaveString(ofs, Core::ChunkKey::FTResource::FILE_NAME, this->GetFileName());
		Core::FileIOHelper::SaveBool(ofs, ChunkKey::StandardMat::USE_TEXTURE, mData->UseTexture);
		Core::FileIOHelper::SaveFloat(ofs, ChunkKey::StandardMat::ALPHA_TRIM, mData->AlphaTrim);
		Core::FileIOHelper::SaveVector4(ofs, ChunkKey::StandardMat::COLOR, mData->Color);

		Core::FileIOHelper::EndDataPackSave(ofs, ChunkKey::StandardMat::STANDARD_MAT);
	}

	void StandardMaterial::LoadProperties(std::ifstream& ifs)
	{
		Core::FileIOHelper::BeginDataPackLoad(ifs, ChunkKey::StandardMat::STANDARD_MAT);

		Core::FTDS::String fileName;
		bool			   useTex;
		Core::FileIOHelper::LoadBool(ifs, useTex);
		Core::FileIOHelper::LoadFloat(ifs, mData->AlphaTrim);
		Core::FileIOHelper::LoadVector4(ifs, mData->Color);
		Core::FileIOHelper::LoadBasicString(ifs, fileName);

		mData->UseTexture = (uint32_t)useTex;
	}

#ifdef FOXTROT_EDITOR
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