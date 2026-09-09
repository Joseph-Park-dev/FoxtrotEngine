#include "Manager/LightManager.h"

#include "Debugging/DebugMemAlloc.h"
#include "ResourceSystem/Cubemap/FTCubemap.h"
#include "ResourceSystem/Mesh/GeometryGenerator.h"
#include "ResourceSystem/Light.h"
#include "Renderer/D3D11Renderer.h"
#include "Renderer/Camera.h"

#ifdef FOXTROT_EDITOR
	#include "EditorUtils.h"
#endif // FOXTROT_EDITOR

using Microsoft::WRL::ComPtr;

namespace D3D11
{
	/// @brief Initializes the scene-light collection and shader data.
	/// @note Initializes the :LightManager base or delegates to its constructor.
	LightManager::LightManager()
		: mLights(DBG_NEW Light[GameData::MAX_LIGHTS])
		, mTypes(DBG_NEW Light::TYPE[GameData::MAX_LIGHTS])
		, mActiveStatus(DBG_NEW bool[GameData::MAX_LIGHTS])
		, mCubemap(nullptr)
	{
	}

	/// @brief Releases the resources managed by this instance during destruction.
	LightManager::~LightManager()
	{
		delete[] mLights;
		delete[] mTypes;
		delete[] mActiveStatus;
		delete mCubemap;
	}

	/// @brief Returns the light used by this light manager.
	/// @param i Zero-based index.
	/// @return Borrowed access to the light.
	/// @note Changes through the returned reference affect this object's stored state.
	Light&		 LightManager::GetLight(size_t i) const { return mLights[i]; }
	/// @brief Returns the type used by this light manager.
	/// @param i Zero-based index.
	/// @return Borrowed access to the type.
	/// @note Changes through the returned reference affect this object's stored state.
	Light::TYPE& LightManager::GetType(size_t i) const { return mTypes[i]; }
	/// @brief Tests the active condition for the current object.
	/// @param i Zero-based index.
	/// @return True if the operation succeeds or the tested condition holds; otherwise false.
	bool&		 LightManager::IsActive(size_t i) const { return mActiveStatus[i]; }

	// FTCubemap* LightManager::GetCubeMap() const { return mCubemap; }

	// ComPtr<ID3D11ShaderResourceView> LightManager::GetCubeMapDiffuse()
	//{
	//	return mCubemap->GetDiffuseResView();
	// }

	// ComPtr<ID3D11ShaderResourceView> LightManager::GetCubeMapSpecular()
	//{
	//	return mCubemap->GetDiffuseResView();
	// }

	/// @brief Initializes the services and state required before this object's runtime lifecycle begins.
	/// @param renderer Renderer providing the graphics device and current render state.
	void LightManager::Initialize(D3D11::D3D11Renderer* renderer)
	{
		mTypes[0] = Light::TYPE::DIRECTIONAL;
		// InitializeCubeMap(renderer);
	}

	/// @brief Creates the cube-map texture resources used by the material.
	/// @param renderer Renderer providing the graphics device and current render state.
	void LightManager::InitializeCubeMap(D3D11::D3D11Renderer* renderer)
	{
		if (!mCubemap)
		{
			// mCubemap = DBG_NEW FTCubemap;
			// mCubemap->Initialize({ GeometryGenerator::MakeBox(20.f) }, renderer->GetDevice(), renderer->GetContext());
		}
	}

	/// @brief Provides an empty lifecycle or extension hook for this implementation.
	/// @param renderer Renderer providing the graphics device and current render state.
	/// @param camInst Camera supplying the view and projection for this draw.
	void LightManager::Render(D3D11::D3D11Renderer* renderer, Camera* camInst)
	{
		/*if (mCubemap)
		{
			mCubemap->CalcVCData(camInst);
			mCubemap->UpdateConstantBuffers(renderer->GetDevice(), renderer->GetContext());
			mCubemap->Render(renderer);
		}*/
	}

	/// @brief Provides an empty lifecycle or extension hook for this implementation.
	/// @param renderer Renderer providing the graphics device and current render state.
	void LightManager::Reset(D3D11::D3D11Renderer* renderer)
	{
		/*delete mCubemap;
		mCubemap = nullptr;

		InitializeCubeMap(renderer);
		mCubemap->SetTexture(texKey);*/
	}

	/// @brief Serializes this object's persistent properties to a .chunk stream.
	/// @param ofs Output stream receiving the serialized data.
	/// @note Writes to the supplied stream at its current position.
	void LightManager::SaveProperties(std::ofstream& ofs)
	{
		for (size_t i = 0; i < GameData::MAX_LIGHTS; ++i)
			mLights[i].SaveProperties(ofs, mTypes[i], mActiveStatus[i]);
		// mCubemap->SaveProperties(ofs);
	}

	/// @brief Restores this object's persistent properties from a .chunk stream.
	/// @param ifs Input stream positioned at the expected data; reading advances its position.
	/// @note Advances the stream position and updates the destination state.
	void LightManager::LoadProperties(std::ifstream& ifs)
	{
		// mCubemap->LoadProperties(ifs);
		for (size_t i = 0; i < GameData::MAX_LIGHTS; ++i)
			mLights[i].LoadProperties(ifs, mTypes[i], mActiveStatus[i]);
	}

#ifdef FOXTROT_EDITOR
	/// @brief Builds the controls for inspecting and editing scene lights.
	void LightManager::DisplayLightMenu()
	{
		ImGui::Begin("Lights");
		const char* lightTypesStr[3] = { "DIRECTIONAL", "POINT", "SPOT" };
		static int	indices[3];
		if (ImGui::BeginListBox(("Light " + std::to_string(0)).c_str(), ImVec2(-FLT_MIN, 200)))
		{
			ImGui::Checkbox(LightKey::IS_ACTIVE, &mActiveStatus[0]);
			::Editor::DisplayArrayAsCombo(LightKey::TYPE, lightTypesStr, Light::TYPE::END, indices[0]);
			mLights[0].UpdateUI();
			ImGui::EndListBox();
		}
		mTypes[0] = (Light::TYPE)indices[0];

		if (mCubemap)
			mCubemap->UpdateUI();
		ImGui::End();
	}
#endif // FOXTROT_EDITOR
} // namespace D3D11
