#include "StandardMaterial.h"

#include "Managers/LightManager.h"
#include "ResourceSystem/Light.h"
#include "Renderer/D3D11Utils.h"
#include "Renderer/Camera.h"
#include "ResourceSystem/Mesh.h"
#include "ResourceSystem/FTMaterials/StandardMaterial.h"

//void StandardMaterial::AssignData(StandardMatData& standardDest, BlinnPhongData& blinnPhongDest)
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
//}

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

StandardMaterial::StandardMaterial()
	: FTMaterial()
	, mData(new StandardMatData)
{
}

StandardMaterial::~StandardMaterial()
{
	delete mData;
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
}
#endif
