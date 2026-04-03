#include "StandardMaterial.h"

#include "Manager/ResourceManager.h"
#include "ResourceSystem/Light.h"
#include "Renderer/D3D11Utils.h"
#include "Renderer/Camera.h"
#include "ResourceSystem/Mesh/Mesh.h"
#include "Debugging/DebugFuncs.h"

#ifdef FOXTROT_EDITOR
	#include <EditorUtils.h>
#endif

namespace D3D11
{
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

	StandardMaterial::StandardMaterial(FTResourceDef& resDef, D3D11Renderer* renderer)
		: FTMaterial(resDef, renderer)
		, mData(DBG_NEW StandardMatData)
	{
	}

	StandardMaterial::~StandardMaterial()
	{
		delete mData;
	}

	void StandardMaterial::SaveProperties(std::ofstream& ofs)
	{
		FileIOHelper::BeginDataPackSave(ofs, ChunkKey::StandardMat::STANDARD_MAT);

		FTResource::SaveProperties(ofs);
		FileIOHelper::SaveBool(ofs, ChunkKey::StandardMat::USE_TEXTURE, mData->UseTexture);
		FileIOHelper::SaveFloat(ofs, ChunkKey::StandardMat::ALPHA_TRIM, mData->AlphaTrim);
		FileIOHelper::SaveVector4(ofs, ChunkKey::StandardMat::COLOR, mData->Color);

		FileIOHelper::EndDataPackSave(ofs, ChunkKey::StandardMat::STANDARD_MAT);
	}

	void StandardMaterial::LoadProperties(std::ifstream& ifs)
	{
		FileIOHelper::BeginDataPackLoad(ifs, ChunkKey::StandardMat::STANDARD_MAT);

		bool useTex;
		FileIOHelper::LoadBool(ifs, useTex);
		FileIOHelper::LoadFloat(ifs, mData->AlphaTrim);
		FileIOHelper::LoadVector4(ifs, mData->Color);

		FTResource::LoadProperties(ifs);
		mData->UseTexture = (uint32_t)useTex;
	}

#ifdef FOXTROT_EDITOR
	void StandardMaterial::UpdateUI()
	{
		ImGui::SeparatorText("Standard Mat Data");

		bool useTex = mData->UseTexture;
		CommandHistory::GetInstance()->UpdateBoolValue(ChunkKey::StandardMat::USE_TEXTURE, useTex);
		mData->UseTexture = useTex;

		CommandHistory::GetInstance()->UpdateFloatValue(ChunkKey::StandardMat::ALPHA_TRIM, mData->AlphaTrim);
		CommandHistory::GetInstance()->UpdateVector4Value(ChunkKey::StandardMat::COLOR, mData->Color);
	}
#endif
} // namespace D3D11