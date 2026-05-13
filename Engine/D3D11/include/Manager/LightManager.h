#pragma once
#include "Utility/SingletonMacro.h"

#include <wrl.h>
#include <d3d11.h>

#include "ResourceSystem/Light.h"

namespace GameData
{
	constexpr size_t MAX_LIGHTS = 3;
}

namespace D3D11
{
	class FTCubemap;
	class Camera;
	class D3D11Renderer;

	class LightManager
	{
		SINGLETON(LightManager)

	public:
		Light&		 GetLight(size_t i) const;
		Light::TYPE& GetType(size_t i) const;
		bool&		 IsActive(size_t i) const;

		// FTCubemap*										   GetCubeMap() const;
		//::Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> GetCubeMapDiffuse();
		//::Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> GetCubeMapSpecular();

	public:
		void Initialize(D3D11::D3D11Renderer* renderer);
		void InitializeCubeMap(D3D11::D3D11Renderer* renderer);
		void Render(D3D11::D3D11Renderer* renderer, Camera* camInst);

		void Reset(D3D11::D3D11Renderer* renderer);

	private:
		Light*		 mLights;
		Light::TYPE* mTypes;
		bool*		 mActiveStatus;

		FTCubemap* mCubemap;

	public:
		void SaveProperties(std::ofstream& ofs);
		void LoadProperties(std::ifstream& ifs);

#ifdef FOXTROT_EDITOR
	public:
		void DisplayLightMenu();
#endif
	};
} // namespace D3D11