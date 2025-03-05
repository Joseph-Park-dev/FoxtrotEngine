#pragma once
#include "Core/SingletonMacro.h"
#include "ResourceSystem/Light.h"

namespace GameData
{
	constexpr size_t MAX_LIGHTS = 3;
}

class LightManager
{
	SINGLETON(LightManager)

public:
	Light&		 GetLight(size_t i) const;
	Light::TYPE& GetType(size_t i) const;
	bool&		 IsActive(size_t i) const;

public:
	void Initialize();

private:
	Light*		 mLights;
	Light::TYPE* mTypes;
	bool*		 mActiveStatus;

public:
	void SaveProperties(std::ofstream& ofs);
	void LoadProperties(std::ifstream& ifs);

#ifdef FOXTROT_EDITOR
public:
	void DisplayLightMenu();
#endif
};
