#pragma once
#include "Math/FTMath.h"
#include "FileSystem/FileIOHelper.h"
#include "FileSystem/MaterialKeys.h"

struct FTMaterial
{
	FTVector3 Ambient	= FTVector3(0.1f);
	float	  Shininess = 1.0f;
	FTVector3 Diffuse	= FTVector3(0.5f);
	float	  dummy1;
	FTVector3 Specular	= FTVector3(0.5f);
	float	  dummy2;

	void SaveProperties(std::ofstream& ofs, const char* name)
	{
		FileIOHelper::BeginDataPackSave(ofs, FileTypes::MATERIAL);
		FileIOHelper::SaveString(ofs, MaterialKeys::NAME, name);

		FileIOHelper::SaveVector3(ofs, MaterialKeys::AMBIENT, this->Ambient);
		FileIOHelper::SaveVector3(ofs, MaterialKeys::AMBIENT, this->Diffuse);
		FileIOHelper::SaveVector3(ofs, MaterialKeys::AMBIENT, this->Specular);
		FileIOHelper::SaveFloat(ofs, MaterialKeys::AMBIENT, this->Shininess);

		FileIOHelper::EndDataPackSave(ofs, FileTypes::MATERIAL);
	}

	void LoadProperties(std::ifstream& ifs, const char* name)
	{
		std::string nameStr;
		FileIOHelper::BeginDataPackLoad(ifs, FileTypes::MATERIAL);
		FileIOHelper::LoadBasicString(ifs, nameStr);
		name = nameStr.c_str();

		FileIOHelper::LoadFloat(ifs, this->Shininess);
		FileIOHelper::LoadVector3(ifs, this->Specular);
		FileIOHelper::LoadVector3(ifs, this->Diffuse);
		FileIOHelper::LoadVector3(ifs, this->Ambient);
	}
};
