#pragma once
#include "ResourceSystem/FTMaterials/FTMaterial.h"

template <typename SHADER>
class StandardMaterial : public FTMaterial<SHADER>
{
public:
	void AssignData(MaterialData& matData);
	void AssignNull(MaterialData& matData);

public:
	StandardMaterial();

private:
	FTVector3 Ambient = FTVector3(0.1f);
	float	  Shininess = 1.0f;
	FTVector3 Diffuse = FTVector3(0.5f);
	FTVector3 Specular = FTVector3(0.5f);

public:
	virtual void SaveProperties(std::ofstream& ofs, UINT key) override;
	virtual UINT LoadProperties(std::ifstream& ifs) override;

#ifdef FOXTROT_EDITOR
	void UpdateUI() override;
#endif
};

namespace ChunkKey
{
	namespace Material
	{
		constexpr const char* AMBIENT = "Ambient";
		constexpr const char* SHININESS = "Shininess";
		constexpr const char* DIFFUSE = "Diffuse";
		constexpr const char* SPECULAR = "Specular";

		constexpr const unsigned int BASIC_MATERIAL = 1;
	}
}