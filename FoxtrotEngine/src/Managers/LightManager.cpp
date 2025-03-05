#include "LightManager.h"

#include "Debugging/DebugMemAlloc.h"

#ifdef FOXTROT_EDITOR
	#include "EditorUtils.h"
#endif // FOXTROT_EDITOR

LightManager::LightManager()
	: mLights(DBG_NEW Light[GameData::MAX_LIGHTS])
	, mTypes(DBG_NEW Light::TYPE[GameData::MAX_LIGHTS])
	, mActiveStatus(DBG_NEW bool[GameData::MAX_LIGHTS])
{
}

LightManager::~LightManager()
{
	delete[] mLights;
	delete[] mTypes;
	delete[] mActiveStatus;
}

Light&		 LightManager::GetLight(size_t i) const { return mLights[i]; }
Light::TYPE& LightManager::GetType(size_t i) const { return mTypes[i]; }
bool&		 LightManager::IsActive(size_t i) const { return mActiveStatus[i]; }

void LightManager::Initialize()
{
	mTypes[0] = Light::TYPE::DIRECTIONAL;
}

void LightManager::SaveProperties(std::ofstream& ofs)
{
	for (size_t i = 0; i < GameData::MAX_LIGHTS; ++i)
		mLights[i].SaveProperties(ofs, mTypes[i], mActiveStatus[i]);
}

void LightManager::LoadProperties(std::ifstream& ifs)
{
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
	ImGui::End();
}
#endif // FOXTROT_EDITOR