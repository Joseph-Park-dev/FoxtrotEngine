#include "LightManager.h"

#include "Debugging/DebugMemAlloc.h"
#include "ResourceSystem/FTCubemap/FTCubemap.h"
#include "ResourceSystem/GeometryGenerator.h"
#include "Renderer/FoxtrotRenderer.h"
#include "Renderer/Camera.h"

#ifdef FOXTROT_EDITOR
	#include "EditorUtils.h"
#endif // FOXTROT_EDITOR

LightManager::LightManager()
	: mLights(DBG_NEW Light[GameData::MAX_LIGHTS])
	, mTypes(DBG_NEW Light::TYPE[GameData::MAX_LIGHTS])
	, mActiveStatus(DBG_NEW bool[GameData::MAX_LIGHTS])
	, mCubemap(nullptr)
{
}

LightManager::~LightManager()
{
	delete[] mLights;
	delete[] mTypes;
	delete[] mActiveStatus;
	delete mCubemap;
}

Light&		 LightManager::GetLight(size_t i) const { return mLights[i]; }
Light::TYPE& LightManager::GetType(size_t i) const { return mTypes[i]; }
bool&		 LightManager::IsActive(size_t i) const { return mActiveStatus[i]; }

FTCubemap* LightManager::GetCubeMap() const { return mCubemap; }

void LightManager::Initialize(FoxtrotRenderer* renderer)
{
	mTypes[0] = Light::TYPE::DIRECTIONAL;
	InitializeCubeMap(renderer);
}

void LightManager::InitializeCubeMap(FoxtrotRenderer* renderer)
{
	if (!mCubemap)
	{
		mCubemap = DBG_NEW FTCubemap;
		mCubemap->Initialize({ GeometryGenerator::MakeBox(20.f) }, renderer->GetDevice(), renderer->GetContext());
	}
}

void LightManager::Render(FoxtrotRenderer* renderer, Camera* camInst)
{
	if (mCubemap)
	{
		mCubemap->CalcVCData(camInst);
		mCubemap->UpdateConstantBuffers(renderer->GetDevice(), renderer->GetContext());
		mCubemap->Render(renderer);
	}
}

void LightManager::Reset(FoxtrotRenderer* renderer)
{
	UINT texKey = mCubemap->GetTexKey();

	delete mCubemap;
	mCubemap = nullptr;

	InitializeCubeMap(renderer);
	mCubemap->SetTexture(texKey);
}

void LightManager::SaveProperties(std::ofstream& ofs)
{
	for (size_t i = 0; i < GameData::MAX_LIGHTS; ++i)
		mLights[i].SaveProperties(ofs, mTypes[i], mActiveStatus[i]);
	mCubemap->SaveProperties(ofs, ChunkKey::NullVal::VALUE_NOT_ASSIGNED);
}

void LightManager::LoadProperties(std::ifstream& ifs)
{
	mCubemap->LoadProperties(ifs);
	for (size_t i = 0; i < GameData::MAX_LIGHTS; ++i)
		mLights[i].LoadProperties(ifs, mTypes[i], mActiveStatus[i]);
}

#ifdef FOXTROT_EDITOR
void LightManager::DisplayLightMenu()
{
	ImGui::Begin("Lights");
	const char* lightTypesStr[3] = { "DIRECTIONAL", "POINT", "SPOT" };
	static int	indices[3];
	if (ImGui::BeginListBox(std::string("Light " + std::to_string(0)).c_str(), ImVec2(-FLT_MIN, 200)))
	{
		ImGui::Checkbox(LightKey::IS_ACTIVE, &mActiveStatus[0]);
		FTEditorUtils::DisplayArrayAsCombo(LightKey::TYPE, lightTypesStr, Light::TYPE::END, indices[0]);
		mLights[0].UpdateUI();
		ImGui::EndListBox();
	}
	mTypes[0] = (Light::TYPE)indices[0];

	if(mCubemap)
		mCubemap->UpdateUI();
	ImGui::End();
}
#endif // FOXTROT_EDITOR