#pragma once
#include "Utility/SingletonMacro.h"

#include <wrl.h>
#include <d3d11.h>

#include "ResourceSystem/Light.h"

namespace D3D11
{
	namespace GameData
	{
		constexpr size_t MAX_LIGHTS = 3;
	}
	class FTCubemap;
	class Camera;
	class D3D11Renderer;

	class LightManager
	{
		SINGLETON(LightManager)

	public:
		/// @brief Returns the light used by this light manager.
		/// @param i Zero-based index.
		/// @return Borrowed access to the light.
		/// @note Changes through the returned reference affect this object's stored state.
		Light&		 GetLight(size_t i) const;
		/// @brief Returns the type used by this light manager.
		/// @param i Zero-based index.
		/// @return Borrowed access to the type.
		/// @note Changes through the returned reference affect this object's stored state.
		Light::TYPE& GetType(size_t i) const;
		/// @brief Tests the active condition for the current object.
		/// @param i Zero-based index.
		/// @return True if the operation succeeds or the tested condition holds; otherwise false.
		bool&		 IsActive(size_t i) const;

		// FTCubemap*										   GetCubeMap() const;
		//::Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> GetCubeMapDiffuse();
		//::Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> GetCubeMapSpecular();

	public:
		/// @brief Initializes the services and state required before this object's runtime lifecycle begins.
		/// @param renderer Renderer providing the graphics device and current render state.
		void Initialize(D3D11::D3D11Renderer* renderer);
		/// @brief Creates the cube-map texture resources used by the material.
		/// @param renderer Renderer providing the graphics device and current render state.
		void InitializeCubeMap(D3D11::D3D11Renderer* renderer);
		/// @brief Provides an empty lifecycle or extension hook for this implementation.
		/// @param renderer Renderer providing the graphics device and current render state.
		/// @param camInst Camera supplying the view and projection for this draw.
		void Render(D3D11::D3D11Renderer* renderer, Camera* camInst);

		/// @brief Provides an empty lifecycle or extension hook for this implementation.
		/// @param renderer Renderer providing the graphics device and current render state.
		void Reset(D3D11::D3D11Renderer* renderer);

	private:
		Light*		 mLights;
		Light::TYPE* mTypes;
		bool*		 mActiveStatus;

		FTCubemap* mCubemap;

	public:
		/// @brief Serializes this object's persistent properties to a .chunk stream.
		/// @param ofs Output stream receiving the serialized data.
		/// @note Writes to the supplied stream at its current position.
		void SaveProperties(std::ofstream& ofs);
		/// @brief Restores this object's persistent properties from a .chunk stream.
		/// @param ifs Input stream positioned at the expected data; reading advances its position.
		/// @note Advances the stream position and updates the destination state.
		void LoadProperties(std::ifstream& ifs);

#ifdef FOXTROT_EDITOR
	public:
		/// @brief Builds the controls for inspecting and editing scene lights.
		void DisplayLightMenu();
#endif
	};
} // namespace D3D11
