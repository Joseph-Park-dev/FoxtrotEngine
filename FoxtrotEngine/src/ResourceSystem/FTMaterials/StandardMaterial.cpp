#include "StandardMaterial.h"

#include "Managers/LightManager.h"
#include "ResourceSystem/Light.h"
#include "Renderer/D3D11Utils.h"
#include "Renderer/Camera.h"
#include "ResourceSystem/Mesh.h"
#include "ResourceSystem/FTMaterials/StandardMaterial.h"
#include "Debugging/DebugFuncs.h"

#ifdef FOXTROT_EDITOR
#include "EditorUtils.h"
#endif // FOXTROT_EDIOR


// void StandardMaterial::AssignData(StandardMatData& standardDest, BlinnPhongData& blinnPhongDest)
//{
//	standardDest.EyeWorld	= mData->EyeWorld;
//	standardDest.UseTexture = mData->UseTexture;
//
//	for (size_t i = 0; i < GameData::MAX_LIGHTS; ++i)
//		standardDest.Lights[i] = mData->mLights[i];
//
//	standardDest.IndexColor = mData->mIndexColor;
//
//	blinnPhongDest.Ambient	 = mData->mBlinnPhongData.Ambient;
//	blinnPhongDest.Shininess = mData->mBlinnPhongData.Shininess;
//	blinnPhongDest.Diffuse	 = mData->mBlinnPhongData.Diffuse;
//	blinnPhongDest.Specular	 = mData->mBlinnPhongData.Specular;
// }

void StandardMaterial::CreatePixelConstBuffer(
	ComPtr<ID3D11Device>& device, ComPtr<ID3D11Buffer>& buffer)
{
	D3D11Utils::CreateConstantBuffer(device, *mData, buffer);
}

void StandardMaterial::UpdateBuffer(ComPtr<ID3D11DeviceContext>& context, ComPtr<ID3D11Buffer>& buffer)
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
	D3D11Utils::UpdateBuffer(context, *mData, buffer);
}

void StandardMaterial::LoadFromFile()
{
	std::ifstream ifs(RelativePath().C_Str());

	if (ifs)
	{
		FileIOHelper::BeginDataPackLoad(ifs, ChunkKey::STANDARD_MAT);

		FileIOHelper::LoadVector3(ifs, mData->BlinnPhongData.Specular);
		FileIOHelper::LoadVector3(ifs, mData->BlinnPhongData.Diffuse);
		FileIOHelper::LoadFloat(ifs, mData->BlinnPhongData.Shininess);
		FileIOHelper::LoadVector3(ifs, mData->BlinnPhongData.Ambient);
	}
	else 
		Debug::LogError(__LINE__, __FILE__, "Failed to load material from file");
}

StandardMaterial::StandardMaterial()
	: FTMaterial()
	, mData(DBG_NEW StandardMatData)
{
	FTDS::String name = FTDS::String("StandardMaterial") + FileTypes::MATERIAL;

#ifdef FOXTROT_EDITOR
	FTDS::String path = PATH_PROJECT + "Assets\\Materials\\" + name;
#else
	FTDS::String path = FTDS::String(".\\Assets\\Materials\\") + name;
#endif // FOXTROT_EDITOR


	SetFileName(name);
	SetRelativePath(path);
}

StandardMaterial::~StandardMaterial()
{
	delete mData;
}

void StandardMaterial::SaveProperties(std::ofstream& ofs)
{
	FileIOHelper::BeginDataPackSave(ofs, ChunkKey::STANDARD_MAT);
	FTResource::SaveProperties(ofs);
	FileIOHelper::EndDataPackSave(ofs, ChunkKey::STANDARD_MAT);
}

void StandardMaterial::LoadProperties(std::ifstream& ifs)
{
	std::pair<size_t, FTDS::String> pack = FileIOHelper::BeginDataPackLoad(ifs);
	FTResource::LoadProperties(ifs);
}

void StandardMaterial::SaveToFile()
{
	std::ofstream ofs(RelativePath().C_Str());

	if (ofs)
	{
		FileIOHelper::BeginDataPackSave(ofs, ChunkKey::STANDARD_MAT);

		FileIOHelper::SaveVector3(ofs, ChunkKey::BlinnPhong::AMBIENT, mData->BlinnPhongData.Ambient);
		FileIOHelper::SaveFloat(ofs, ChunkKey::BlinnPhong::SHININESS, mData->BlinnPhongData.Shininess);
		FileIOHelper::SaveVector3(ofs, ChunkKey::BlinnPhong::DIFFUSE, mData->BlinnPhongData.Diffuse);
		FileIOHelper::SaveVector3(ofs, ChunkKey::BlinnPhong::SPECULAR, mData->BlinnPhongData.Specular);

		FileIOHelper::EndDataPackSave(ofs, ChunkKey::STANDARD_MAT);
		FileIOHelper::SaveBufferToFile(ofs);

		printf("Material %s created to %s\n", FileName().C_Str(), RelativePath().C_Str());
	}
	else
		Debug::LogError(__LINE__, __FILE__, "Failed to save material to file");
}

#ifdef FOXTROT_EDITOR
void StandardMaterial::UpdateUI()
{
	ImGui::SeparatorText("Standard Mat Data");

	bool useTex = (bool)mData->UseTexture;
	CommandHistory::GetInstance()->UpdateBoolValue(ChunkKey::USE_TEXTURE, useTex);
	mData->UseTexture = (uint32_t)useTex;

	ImGui::SeparatorText("BlinnPhong Data");
	CommandHistory::GetInstance()->UpdateVector3Value(ChunkKey::BlinnPhong::AMBIENT, mData->BlinnPhongData.Ambient);
	CommandHistory::GetInstance()->UpdateFloatValue(ChunkKey::BlinnPhong::SHININESS, mData->BlinnPhongData.Shininess);
	CommandHistory::GetInstance()->UpdateVector3Value(ChunkKey::BlinnPhong::DIFFUSE, mData->BlinnPhongData.Diffuse);
	CommandHistory::GetInstance()->UpdateVector3Value(ChunkKey::BlinnPhong::SPECULAR, mData->BlinnPhongData.Specular);

	if (ImGui::Button("Save"))
		SaveToFile();
}
#endif
