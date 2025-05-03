#pragma once
#include "Core/SingletonMacro.h"
#include "ResourceSystem/Light.h"

namespace GameData
{
	constexpr size_t MAX_LIGHTS = 3;
}

class FTCubemap;
class Camera;

class LightManager
{
	SINGLETON(LightManager)

public:
	Light&		 GetLight(size_t i) const;
	Light::TYPE& GetType(size_t i) const;
	bool&		 IsActive(size_t i) const;

	FTCubemap* GetCubeMap() const;
	ComPtr<ID3D11ShaderResourceView> GetCubeMapDiffuse();
	ComPtr<ID3D11ShaderResourceView> GetCubeMapSpecular();

public:
	void Initialize(FoxtrotRenderer* renderer);
	void InitializeCubeMap(FoxtrotRenderer* renderer);
	void Render(FoxtrotRenderer* renderer, Camera* camInst);

	void Reset(FoxtrotRenderer* renderer);

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
