#pragma once
#include "SingletonMacro.h"

namespace GameData
{
	constexpr size_t MAX_LIGHTS = 3;
}

class FTCubemap;
class Camera;
class Light;

class LightManager
{
	SINGLETON(LightManager)

public:
	Light&		 GetLight(size_t i) const;
	bool&		 IsActive(size_t i) const;

	FTCubemap* GetCubeMap() const;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> GetCubeMapDiffuse();
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> GetCubeMapSpecular();

public:
	void Initialize(FoxtrotRenderer* renderer);
	void InitializeCubeMap(FoxtrotRenderer* renderer);
	void Render(FoxtrotRenderer* renderer, Camera* camInst);

	void Reset(FoxtrotRenderer* renderer);

private:
	Light*		 mLights;
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
