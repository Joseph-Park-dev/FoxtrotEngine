#pragma once
#include "ResourceSystem/FTResource.h"

#include "Math/FTMath.h"

struct MaterialData;

class FTMaterial : public FTResource
{
public:
	void		AssignData(MaterialData& matData);
	static void AssignNull(MaterialData& matData);

public:
	FTMaterial();

private:
	FTVector3 mAmbient;
	float	  mShininess;
	FTVector3 mDiffuse;
	FTVector3 mSpecular;

public:
	virtual void SaveProperties(std::ofstream& ofs, UINT key);
	virtual UINT LoadProperties(std::ifstream& ifs);

#ifdef FOXTROT_EDITOR
	// public:
	//	// Modifying the Material values is not needed in the game.
	//	FTVector3& GetAmbientRef();
	//	float&	   GetShininessRef();
	//	FTVector3& GetDiffuseRef();
	//	FTVector3& GetSpecularRef();

public:
	void Save();
	void UpdateUI() override;
#endif // FOXTROT_EDITOR
};

struct MaterialData
{
	FTVector3 Ambient	= FTVector3(0.1f);
	float	  Shininess = 1.0f;
	FTVector3 Diffuse	= FTVector3(0.5f);
	float	  dummy1	= 0.f;
	FTVector3 Specular	= FTVector3(0.5f);
	float	  dummy2	= 0.f;
};

namespace ChunkKey
{
	namespace Material
	{
		constexpr const char* TYPE = "Material";
		constexpr const char* NAME = "Name";

		constexpr const char* AMBIENT	= "Ambient";
		constexpr const char* SHININESS = "Shininess";
		constexpr const char* DIFFUSE	= "Diffuse";
		constexpr const char* SPECULAR	= "Specular";

		constexpr const unsigned int BASIC_MATERIAL = 1;
	} // namespace Material
} // namespace ChunkKey