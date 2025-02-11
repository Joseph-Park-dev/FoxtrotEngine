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
	for (size_t i = 0; i < GameData::MAX_LIGHTS; ++i)
	{
		if (ImGui::BeginListBox(std::string("Light " + std::to_string(i)).c_str(), ImVec2(-FLT_MIN, 200)))
		{
			ImGui::Checkbox(LightKeys::IS_ACTIVE, &mActiveStatus[i]);
			FTEditorUtils::DisplayArrayAsCombo(LightKeys::TYPE, lightTypesStr, Light::TYPE::END, indices[i]);
			mLights[i].UpdateUI();
			ImGui::EndListBox();
		}
		mTypes[i] = (Light::TYPE)indices[i];
	}
	ImGui::TreePop();
	ImGui::End();
}
#endif // FOXTROT_EDITOR