#include "StandardMaterial.h"

#include "Managers/LightManager.h"
#include "ResourceSystem/Light.h"
#include "Renderer/D3D11Utils.h"
#include "Renderer/Camera.h"
#include "ResourceSystem/Mesh.h"
#include "Debugging/DebugFuncs.h"

#ifdef FOXTROT_EDITOR
	#include <EditorUtils.h>
#endif

void StandardMaterial::CreatePixelConstBuffer(ComPtr<ID3D11Device>& device)
{
	D3D11Utils::CreateConstantBuffer(device, *mData, GetPCBuf());
}

void StandardMaterial::UpdateBuffer(ComPtr<ID3D11DeviceContext>& context)
{
	Matrix&& viewMat  = Camera::GetInstance()->GetViewRow();
	Vector3	 eyeWorld = Vector3::Transform(Vector3(0.0f), viewMat.Invert());

	for (size_t i = 0; i < Light::TYPE::END; ++i)
	{
		if (LightManager::GetInstance()->GetType(0) == (Light::TYPE)i)
			mData->Lights[i] = LightManager::GetInstance()->GetLight(0);
		else
			mData->Lights[i].Strength *= 0.0f;
	}

	mData->EyeWorld = eyeWorld;
	D3D11Utils::UpdateBuffer(context, *mData, GetPCBuf());
}

StandardMaterial::StandardMaterial(FTResourceDef& resDef, FoxtrotRenderer* renderer)
	: FTMaterial(resDef, renderer)
	, mData(DBG_NEW StandardMatData)
{
	Process(renderer);
}

StandardMaterial::~StandardMaterial()
{
	delete mData;
}

void StandardMaterial::SaveProperties(std::ofstream& ofs)
{
	FileIOHelper::BeginDataPackSave(ofs, ChunkKey::StandardMat::STANDARD_MAT);

	FTResource::SaveProperties(ofs);
	FileIOHelper::SaveVector3(ofs, ChunkKey::BlinnPhong::AMBIENT, mData->BlinnPhongData.Ambient);
	FileIOHelper::SaveFloat(ofs, ChunkKey::BlinnPhong::SHININESS, mData->BlinnPhongData.Shininess);
	FileIOHelper::SaveVector3(ofs, ChunkKey::BlinnPhong::DIFFUSE, mData->BlinnPhongData.Diffuse);
	FileIOHelper::SaveVector3(ofs, ChunkKey::BlinnPhong::SPECULAR, mData->BlinnPhongData.Specular);

	FileIOHelper::EndDataPackSave(ofs, ChunkKey::StandardMat::STANDARD_MAT);
}

void StandardMaterial::LoadProperties(std::ifstream& ifs)
{
	FileIOHelper::BeginDataPackLoad(ifs, ChunkKey::StandardMat::STANDARD_MAT);

	FileIOHelper::LoadVector3(ifs, mData->BlinnPhongData.Specular);
	FileIOHelper::LoadVector3(ifs, mData->BlinnPhongData.Diffuse);
	FileIOHelper::LoadFloat(ifs, mData->BlinnPhongData.Shininess);
	FileIOHelper::LoadVector3(ifs, mData->BlinnPhongData.Ambient);

	FTResource::LoadProperties(ifs);
}

#ifdef FOXTROT_EDITOR
void StandardMaterial::UpdateUI()
{
	ImGui::SeparatorText("Standard Mat Data");

	CommandHistory::GetInstance()->UpdateBoolValue(ChunkKey::StandardMat::USE_TEXTURE, mData->UseTexture);

	ImGui::SeparatorText("BlinnPhong Data");
	CommandHistory::GetInstance()->UpdateVector3Value(ChunkKey::BlinnPhong::AMBIENT, mData->BlinnPhongData.Ambient);
	CommandHistory::GetInstance()->UpdateFloatValue(ChunkKey::BlinnPhong::SHININESS, mData->BlinnPhongData.Shininess);
	CommandHistory::GetInstance()->UpdateVector3Value(ChunkKey::BlinnPhong::DIFFUSE, mData->BlinnPhongData.Diffuse);
	CommandHistory::GetInstance()->UpdateVector3Value(ChunkKey::BlinnPhong::SPECULAR, mData->BlinnPhongData.Specular);
}
#endif
